#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>

#define LOGS_ENABLED true
#define PRINT_TAG "tracking"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_VERBOSE,PRINT_TAG,FORMAT,__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,PRINT_TAG,FORMAT,__VA_ARGS__)
using namespace cv;

class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {}

    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);
    }

    virtual ~CascadeDetectorAdapter() {
    }

private:
    CascadeDetectorAdapter();

    cv::Ptr<cv::CascadeClassifier> Detector;
};

DetectionBasedTracker *tracker = 0;
ANativeWindow *nativeWindow = 0;


extern "C"
JNIEXPORT void JNICALL
Java_com_ben_android_facetracking_FaceTrackingHelper_nativeInit(JNIEnv *env, jclass clazz,
                                                                jstring model) {
    const char *path = env->GetStringUTFChars(model, JNI_FALSE);
    if (tracker) {
        tracker->stop();
        delete (tracker);
        tracker = 0;
    }
    //创建跟踪器
    cv::Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(path));
    cv::Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(
            makePtr<CascadeClassifier>(path));
    //OpenCV使用的跟踪器
    DetectionBasedTracker::Parameters DetectorParams;
    tracker = new DetectionBasedTracker(mainDetector, trackingDetector, DetectorParams);
    //启动追踪器
    tracker->run();

    env->ReleaseStringUTFChars(model, path);

}extern "C"
JNIEXPORT void JNICALL
Java_com_ben_android_facetracking_FaceTrackingHelper_nativeSetSurface(JNIEnv *env, jclass clazz,
                                                                      jobject surface) {
    if (nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
    }
    nativeWindow = ANativeWindow_fromSurface(env, surface);


}extern "C"
JNIEXPORT void JNICALL
Java_com_ben_android_facetracking_FaceTrackingHelper_nativePostCamera(JNIEnv *env, jclass clazz,
                                                                      jbyteArray jdata, jint w,
                                                                      jint h, jint camera_id) {
    jbyte *data = env->GetByteArrayElements(jdata, JNI_FALSE);
    //data --> Mat
    Mat src(h + h / 2, w, CV_8UC1, data);
    Mat gray;
    //NV21  ---> RGBA
    cvtColor(src, src, COLOR_YUV2RGBA_NV21);
    //rotate
    rotate(src, src, camera_id == 1 ? ROTATE_90_COUNTERCLOCKWISE : ROTATE_90_CLOCKWISE);
    //颜色转换
    cvtColor(src, gray, COLOR_RGBA2GRAY);
    //直方图均衡
    equalizeHist(gray, gray);
    //开始人脸追踪
    std::vector<Rect> faces;
    tracker->process(gray);
    tracker->getObjects(faces);
    for (Rect face : faces) {
        rectangle(src, face, Scalar(255, 0, 255));
    }
    //render
    if (nativeWindow) {
            ANativeWindow_setBuffersGeometry(nativeWindow, src.cols, src.rows, WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_Buffer buffer;
            ANativeWindow_lock(nativeWindow, &buffer, 0);
            //copy pixle.
            //1pixel == RGBA == 4
            //memcpy(buffer.bits, src.data, buffer.stride * buffer.height * 4 );

            uint8_t *dst_data = static_cast<uint8_t *>(buffer.bits);
            //（RGBA） * 4
            int dst_linesize = buffer.stride * 4;
            for (int i = 0; i < buffer.height; ++i) {
                memcpy(dst_data + i * dst_linesize, src.data + i * src.cols * 4, dst_linesize);
            }
            //unlock
            ANativeWindow_unlockAndPost(nativeWindow);
        } else {
            LOGE("%s", "to do nothing");
    }
    src.release();
    gray.release();
    env->ReleaseByteArrayElements(jdata, data, 0);


}extern "C"
JNIEXPORT void JNICALL
Java_com_ben_android_facetracking_FaceTrackingHelper_nativeRelease(JNIEnv *env, jclass clazz) {
    // TODO: implement nativeRelease()
    tracker->stop();
    delete (tracker);
    tracker = 0;
}