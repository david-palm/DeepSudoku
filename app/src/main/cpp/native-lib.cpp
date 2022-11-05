#include <jni.h>
#include <string>
#include <opencv2/core.hpp>
#include <android/bitmap.h>

#include "utils.h"

using namespace cv;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_blur(JNIEnv *env, jobject thiz, jobject image, jobject output) {
    Mat src;
    bitmapToMat(env, image, src, 0);
    myBlur(src, 50);
    matToBitmap(env, src, output, false);
}