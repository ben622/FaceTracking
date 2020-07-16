package com.ben.android.facetracking;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import java.util.logging.FileHandler;

public class MainActivity extends AppCompatActivity implements Camera.PreviewCallback {

    private SurfaceView surfaceView;
    private CameraHelper cameraHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.sufaceView);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
                initlizateEnvironment();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

            }
        });
    }
    private void initlizateEnvironment() {
        cameraHelper = new CameraHelper.Builder()
                .width(1920)
                .height(1080)
                .callback(this)
                .cameraId(Camera.CameraInfo.CAMERA_FACING_FRONT)
                .build();
        FaceTrackingHelper.nativeInit(AndroidUtilities.copyAssetsFileToSdcard(this, "lbpcascade_frontalface.xml"));
        FaceTrackingHelper.nativeSetSurface(surfaceView.getHolder().getSurface());
        cameraHelper.startPreview();
    }


    @Override
    protected void onStop() {
        super.onStop();
        cameraHelper.stopPreview();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FaceTrackingHelper.nativeRelease();
    }

    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
        FaceTrackingHelper.nativePostCamera(bytes, cameraHelper.builder.getWidth(), cameraHelper.builder.getHeight(), cameraHelper.builder.getCameraId());
    }

}
