#pragma once

#include <android/bitmap.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

void bitmapToMat(JNIEnv *env, jobject bitmap, cv::Mat& dst, jboolean needUnPremultiplyAlpha);
void matToBitmap(JNIEnv* env, cv::Mat src, jobject bitmap, jboolean needPremultiplyAlpha);

void myFlip(cv::Mat src);
void myBlur(cv::Mat src, float sigma);