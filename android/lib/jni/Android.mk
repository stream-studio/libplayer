LOCAL_PATH := $(call my-dir)
LIBPLAYER_ROOT_REL := ../../..
LIBPLAYER_ROOT_ABS := $(LOCAL_PATH)/../../..

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LOCAL_PATH)/PlayerAndroid.cpp \

LOCAL_MODULE := player_android
LOCAL_CPP_FEATURES += exceptions


LOCAL_SHARED_LIBRARIES := player
LOCAL_LDLIBS := -llog -landroid

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  $(LIBPLAYER_ROOT_REL)/src/libplayer.cpp \

LOCAL_C_INCLUDES += \
  $(LIBPLAYER_ROOT_ABS)/src

LOCAL_EXPORT_C_INCLUDES := \
  $(LIBPLAYER_ROOT_ABS)/src

LOCAL_MODULE := player
LOCAL_CPP_FEATURES += exceptions

LOCAL_SHARED_LIBRARIES := gstreamer_android
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

ifndef GSTREAMER_ROOT_ANDROID
$(error GSTREAMER_ROOT_ANDROID is not defined!)
endif

ifeq ($(TARGET_ARCH_ABI),armeabi)
GSTREAMER_ROOT        := $(GSTREAMER_ROOT_ANDROID)/arm
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
GSTREAMER_ROOT        := $(GSTREAMER_ROOT_ANDROID)/armv7
else ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
GSTREAMER_ROOT        := $(GSTREAMER_ROOT_ANDROID)/arm64
else ifeq ($(TARGET_ARCH_ABI),x86)
GSTREAMER_ROOT        := $(GSTREAMER_ROOT_ANDROID)/x86
else ifeq ($(TARGET_ARCH_ABI),x86_64)
GSTREAMER_ROOT        := $(GSTREAMER_ROOT_ANDROID)/x86_64
else
$(error Target arch ABI not supported: $(TARGET_ARCH_ABI))
endif

GSTREAMER_NDK_BUILD_PATH  := $(GSTREAMER_ROOT)/share/gst-android/ndk-build/
GSTREAMER_PLUGINS         := coreelements videotestsrc opensles opengl
GSTREAMER_EXTRA_DEPS      := gstreamer-video-1.0 gobject-2.0
GSTREAMER_EXTRA_LIBS      := -liconv
include $(GSTREAMER_NDK_BUILD_PATH)/gstreamer-1.0.mk