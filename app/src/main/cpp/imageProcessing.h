#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <android/bitmap.h>
#include "HoughAccumulator.h"



void identifySudoku(cv::Mat& input, cv::Mat& output, std::vector<cv::Point>& contour, bool padding = true, int kernelSize = 41);
void warpSudoku(cv::Mat& input, cv::Mat& warped, std::vector<cv::Point2f>& contour);
void identifyLines(cv::Mat& input, cv::Mat& output, std::vector<Pixel*>& lines);
bool findIntersections(std::vector<Pixel*>& lines, cv::Point2i* (&intersections)[100]);
void displayIntersections(cv::Mat& inputOutput, cv::Point2i* (&intersections)[100]);
void cutCells(cv::Mat& input, cv::Mat (&cells)[81], cv::Point2i* (&intersections)[100]);