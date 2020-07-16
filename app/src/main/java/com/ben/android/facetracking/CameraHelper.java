package com.ben.android.facetracking;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.view.SurfaceHolder;
import android.widget.ImageButton;

import java.util.List;

public class CameraHelper implements Camera.PreviewCallback {
    public Builder builder;
    private  Camera mCamera;
    private byte[] callbackBuffer;

    public CameraHelper(Builder builder) {
        this.builder = builder;
    }
    public void startPreview() {
        try {
            mCamera = Camera.open(builder.getCameraId());
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewFormat(ImageFormat.NV21);
            parameters.setPreviewSize(builder.getWidth(),builder.getHeight());
            mCamera.setParameters(parameters);
            callbackBuffer = new byte[builder.getWidth() * builder.getHeight() * 3/2];
            mCamera.addCallbackBuffer(callbackBuffer);
            mCamera.setPreviewCallbackWithBuffer(this);
            SurfaceTexture sufaceTexture = new SurfaceTexture(1);
            mCamera.setPreviewTexture(sufaceTexture);
            mCamera.startPreview();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void stopPreview() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }

    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
        if (mCamera != null && builder.previewCallback!=null) {
            builder.previewCallback.onPreviewFrame(bytes, camera);
            mCamera.addCallbackBuffer(callbackBuffer);
        }
    }

    public static class Builder {
        private int width;
        private int height;
        private int cameraId;
        private Camera.PreviewCallback previewCallback;

        public int getWidth() {
            return width;
        }

        public Builder width(int width) {
            this.width = width;
            return this;
        }

        public int getHeight() {
            return height;
        }

        public Builder height(int height) {
            this.height = height;
            return this;
        }


        public int getCameraId() {
            return cameraId;
        }

        public Builder cameraId(int cameraId) {
            this.cameraId = cameraId;
            return this;
        }

        public Builder callback(Camera.PreviewCallback callback) {
            this.previewCallback = callback;
            return this;
        }

        public CameraHelper build() {
            return new CameraHelper(this);
        }


    }
}
