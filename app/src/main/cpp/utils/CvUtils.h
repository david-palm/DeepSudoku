#pragma once

#include <android/bitmap.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace cvUtils
{
    void bitmapToMat(JNIEnv *env, jobject bitmap, cv::Mat &dst, jboolean needUnPremultiplyAlpha);

    void matToBitmap(JNIEnv *env, cv::Mat src, jobject bitmap, jboolean needPremultiplyAlpha);

    void intToFloatContour(std::vector<cv::Point> &src, std::vector<cv::Point2f> &dst);

    void cutImage(cv::Mat &input, cv::Mat &output, cv::Point2i topLeft, cv::Point2i bottomRight,
                  cv::Point2i offset = cv::Point2i(0, 0));
}