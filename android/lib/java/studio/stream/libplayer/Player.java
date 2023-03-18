package studio.stream.libplayer;

import org.freedesktop.gstreamer.GStreamer;
import android.content.Context;

public class Player{
    private long pointer;

    private native static void nativeClassInit();
    private native void nativeInit();
    private native void nativeStart(Object surface);
    private native void nativeStop();
    private native void nativeRelease();

    public static void init(Context context) throws Exception {
        GStreamer.init(context);
    }

    static {
        System.loadLibrary("gstreamer_android");
        System.loadLibrary("player");
        System.loadLibrary("player_android");
        nativeClassInit();
    }

    public Player(){
        nativeInit();
    }

    public void start(Object surface){
        nativeStart(surface);
    }

    public void stop(){
        nativeStop();
    }

    protected void finalize() throws Throwable {
        nativeRelease();
    }

}