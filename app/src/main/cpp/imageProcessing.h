#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <android/bitmap.h>

void cutSudoku(cv::Mat& input, cv::Mat& output, bool padding = true, int kernelSize = 41);
