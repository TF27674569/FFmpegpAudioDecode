package com.sample.audiod.ffmpegp;

/**
 * Description :
 * <p>
 * Created : TIAN FENG
 * Date : 2019/3/21
 * Email : 27674569@qq.com
 * Version : 1.0
 */
public class AudioUtils {

    static {
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avformat-56");
        System.loadLibrary("avutil-54");
        System.loadLibrary("postproc-53");
        System.loadLibrary("swresample-1");
        System.loadLibrary("swscale-3");
        System.loadLibrary("ffmpegp_audio_decode");
    }


    public static native void decode(String input,String output);
}

