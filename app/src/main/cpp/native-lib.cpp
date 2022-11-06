#include <jni.h>
#include <string>
#include <opencv2/core.hpp>
#include <android/bitmap.h>

#include "utils.h"
#include "imageProcessing.h"

cv::Mat input;
std::vector<cv::Point> contour;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_identifySudoku(JNIEnv *env, jobject thiz, jobject image, jobject output)
{
    cv::Mat sudokuPreview = input;
    bitmapToMat(env, image, input, 0);
    identifySudoku(input, sudokuPreview, contour);
    matToBitmap(env, input, output, false);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_solveSudoku(JNIEnv *env, jobject thiz, jobject output)
{

}