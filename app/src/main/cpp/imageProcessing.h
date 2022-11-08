#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <android/bitmap.h>



void identifySudoku(cv::Mat& input, cv::Mat& output, std::vector<cv::Point>& contour, bool padding = true, int kernelSize = 41);
void warpSudoku(cv::Mat& input, cv::Mat& warped, std::vector<cv::Point2f>& contour);
void createGradientImages(cv::Mat& input, cv::Mat& gradientX, cv::Mat& gradientY, int kernelSize = 25);