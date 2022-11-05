#include <jni.h>
#include <string>
#include <opencv2/core.hpp>
#include <android/bitmap.h>

#include "utils.h"
#include "imageProcessing.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_blur(JNIEnv *env, jobject thiz, jobject image, jobject output) {
    cv::Mat inputMat;
    cv::Mat outputMat = inputMat;
    bitmapToMat(env, image, inputMat, 0);
    cutSudoku(inputMat, outputMat);
    matToBitmap(env, inputMat, output, false);
}