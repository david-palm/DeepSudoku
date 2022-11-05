#pragma once

#include <android/bitmap.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

void myFlip(cv::Mat src);
void myBlur(cv::Mat src, float sigma);