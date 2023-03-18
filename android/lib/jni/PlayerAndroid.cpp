#include <jni.h>

#include "libplayer.h"
#include <gst/gst.h>
#include <android/log.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <pthread.h>

/*
 * These macros provide a way to store the native pointer to CustomData, which might be 32 or 64 bits, into
 * a jlong, which is always 64 bits, without warnings.
 * From Gstreamer example source code : 
 */
#if GLIB_SIZEOF_VOID_P == 8
# define GET_CUSTOM_DATA(env, thiz, fieldID) (PlayerAndroidCtx*)env->GetLongField ( thiz, fieldID)
# define SET_CUSTOM_DATA(env, thiz, fieldID, data) env->SetLongField (thiz, fieldID, (jlong)data)
#else
# define GET_CUSTOM_DATA(env, thiz, fieldID) (PlayerAndroidCtx*)(jint)(env)->GetLongField (thiz, fieldID)
# define SET_CUSTOM_DATA(env, thiz, fieldID, data) env->SetLongField (thiz, fieldID, (jlong)(jint)data)
#endif

GST_DEBUG_CATEGORY_STATIC (debug_category);
#define GST_CAT_DEFAULT debug_category

/* These global variables cache values which are not changing during execution */
static pthread_t gst_app_thread;
static pthread_key_t current_jni_env;
static JavaVM *java_vm;
static jfieldID pointer_field_id;

typedef struct _PlayerAndroidCtx {
    jobject app;            /* Application instance, used to call its methods. A global reference is kept. */
    PlayerCtx context;
    ANativeWindow *native_window; /* The Android native window where video will be rendered */
} PlayerAndroidCtx;


static JNIEnv* attach_current_thread (void) {
    JNIEnv *env;
    JavaVMAttachArgs args;

    //GST_DEBUG ("Attaching thread %p", g_thread_self ());
    args.version = JNI_VERSION_1_6;
    args.name = NULL;
    args.group = NULL;

    if (java_vm->AttachCurrentThread (&env, &args) < 0) {
        //GST_ERROR ("Failed to attach current thread");
        return NULL;
    }

    return env;
}

/* Unregister this thread from the VM */
static void detach_current_thread (void *env) {

    java_vm->DetachCurrentThread ();
}


/* Retrieve the JNI environment for this thread */
static JNIEnv *get_jni_env (void) {
    JNIEnv *env;

    if ((env = static_cast<JNIEnv *>(pthread_getspecific(current_jni_env))) == NULL) {
        env = attach_current_thread ();
        pthread_setspecific (current_jni_env, env);
    }
    return env;
}

static void *app_function (void *userdata) {
    JavaVMAttachArgs args;
    GstBus *bus;
    JNIEnv *env = get_jni_env ();
    __android_log_print(ANDROID_LOG_INFO, "studio", "Stream Player Android Library start");
    PlayerAndroidCtx *data = static_cast<PlayerAndroidCtx*>(userdata);
    libplayer_start(data->context, data->native_window);

}

extern "C" JNIEXPORT void Java_studio_stream_libplayer_Player_nativeClassInit(JNIEnv* env, jclass klass) {
        pointer_field_id = env->GetFieldID (klass, "pointer", "J");

        if (!pointer_field_id) {
        /* We emit this message through the Android log instead of the GStreamer log because the later
         * has not been initialized yet.
         */
            __android_log_print (ANDROID_LOG_ERROR, "studio", "The calling class does not implement all necessary interface methods");
        }
}


extern "C" JNIEXPORT void JNICALL Java_studio_stream_libplayer_Player_nativeInit(JNIEnv *env, jobject thiz){
    __android_log_print(ANDROID_LOG_INFO, "studio", "Stream Player Init");

    PlayerAndroidCtx *data = static_cast<PlayerAndroidCtx*>(malloc(sizeof(PlayerAndroidCtx)));
    PlayerCtx pCtx = libplayer_init();
    data->context = pCtx;
    data->app = env->NewGlobalRef(thiz);
    SET_CUSTOM_DATA(env, thiz, pointer_field_id, data);
}


extern "C" JNIEXPORT void JNICALL Java_studio_stream_libplayer_Player_nativeStart(JNIEnv *env, jobject thiz, jobject surface){
    PlayerAndroidCtx *data = GET_CUSTOM_DATA(env, thiz, pointer_field_id);
    if (!data){
        return;
    }

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface (env, surface);
    data->native_window = nativeWindow;

    pthread_create (&gst_app_thread, NULL, &app_function, data);
}


extern "C" JNIEXPORT void JNICALL Java_studio_stream_libplayer_Player_nativeStop(JNIEnv *env, jobject thiz){
}

extern "C" JNIEXPORT void JNICALL Java_studio_stream_libplayer_Player_nativeRelease(JNIEnv *env, jobject thiz){
    PlayerAndroidCtx *data = GET_CUSTOM_DATA (env, thiz, pointer_field_id);
    ANativeWindow_release (data->native_window);
    data->native_window = NULL;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved){
    setenv("GST_DEBUG", "*:5", 1);
    setenv("GST_DEBUG_NO_COLOR", "1", 1);
    JNIEnv *env = NULL;

    java_vm = vm;

    if (vm->GetEnv ((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print (ANDROID_LOG_ERROR, "tutorial-3",
                             "Could not retrieve JNIEnv");
        return 0;
    }
    pthread_key_create (&current_jni_env, detach_current_thread);

    return JNI_VERSION_1_6;
}
