#include "libplayer.h"

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

typedef struct{
    GstElement *pipeline;
    GstElement *video_sink;
    GstElement *audio_sink;

    gintptr video_handle;
    

    GMainContext *context;       /* GLib context used to run the main loop */
    GMainLoop *main_loop;        /* GLib main loop */    
} PlayerPrivateContext;


static gboolean
message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
    PlayerPrivateContext* context = (PlayerPrivateContext*)user_data;
    
    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_ERROR:{

          break;
        }
        case GST_MESSAGE_WARNING:{

            break;
        }
        case GST_MESSAGE_EOS: {
            g_print ("Got EOS\n");
            break;
        }
        case GST_MESSAGE_ELEMENT: {
            const GstStructure *s = gst_message_get_structure (message);

            if (gst_structure_has_name (s, "GstBinForwarded"))
            {
                GstMessage *forward_msg = NULL;
                
                gst_structure_get (s, "message", GST_TYPE_MESSAGE, &forward_msg, NULL);
                
                
                
                if (GST_MESSAGE_TYPE (forward_msg) == GST_MESSAGE_EOS)
                {
                    g_print ("EOS from element %s\n",
                            GST_OBJECT_NAME (GST_MESSAGE_SRC (forward_msg)));
                    

                }
                gst_message_unref (forward_msg);
                
            }
            break;
        }
        default:
            break;
    }

  return TRUE;
}

PlayerCtx libplayer_init(){
    gst_init(NULL, NULL);

    PlayerPrivateContext* context = static_cast<PlayerPrivateContext*>(malloc(sizeof(PlayerPrivateContext)));
    context->pipeline = gst_pipeline_new(NULL);
    
    GstElement* video_src = gst_element_factory_make("videotestsrc", NULL);
    
    context->video_sink = gst_element_factory_make("glimagesink", NULL);
    g_object_set(context->video_sink, "sync", FALSE, NULL);
    
    gst_bin_add_many(GST_BIN_CAST(context->pipeline), video_src, context->video_sink, NULL);
    

    gst_element_link(video_src,  context->video_sink);
    gst_element_set_state(context->pipeline, GST_STATE_READY);
    
    return context;
}

void libplayer_play(PlayerCtx ctx){

}

void libplayer_pause(PlayerCtx ctx){

}


void libplayer_start(PlayerCtx ctx, void* view){
        
    GstBus *bus;

    PlayerPrivateContext* context = (PlayerPrivateContext*)ctx;
        
    context->context = g_main_context_new ();
    g_main_context_push_thread_default(context->context);
    
    
    /* Create a GLib Main Loop and set it to run */
    GST_DEBUG ("Entering main loop...");
    if (view != NULL){
        gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(context->video_sink), (guintptr) view);
    }

    gst_element_set_state(context->pipeline, GST_STATE_PLAYING);
    
    context->main_loop = g_main_loop_new (context->context, FALSE);
    
    bus = gst_element_get_bus (context->pipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(message_cb), context);
    
    g_main_loop_run (context->main_loop);
    
    GST_DEBUG ("Exited main loop");
    context->video_handle = (gintptr) NULL;
    g_main_loop_unref (context->main_loop);
    context->main_loop = NULL;

    /* Free resources */
    g_main_context_pop_thread_default(context->context);
    g_main_context_unref (context->context);
    gst_element_set_state (context->pipeline, GST_STATE_NULL);
    gst_object_unref (context->pipeline);
    context->pipeline = NULL;
    
    gst_object_unref(context->pipeline);
    free(context);

}

void libplayer_stop(PlayerCtx ctx){
    PlayerPrivateContext* context = (PlayerPrivateContext*)ctx;
    g_main_loop_quit(context->main_loop);
}
