package com.ben.android.facetracking;

import android.view.Surface;

public class FaceTrackingHelper {
    static {
        System.loadLibrary("facetracking");
    }

    //初始化追踪器模型相关
    public native static void nativeInit(String model);

    public native static void nativeSetSurface(Surface surface);

    public native static void nativePostCamera(byte[] data, int w, int h, int cameraId);
    //资源释放
    public native static void nativeRelease();
}
