#pragma once

#include <opencv2/core.hpp>
#include <android/bitmap.h>
#include <jni.h>

#include "../utils/cvUtils.h"
#include "HoughAccumulator.h"

class Line
{
public:
    Line();
};


class ImageProcessor
{

public:
    cv::Mat m_Input;
private:
    std::vector<cv::Point> m_SudokuContour;
    std::vector<cv::Point2f> m_PaddedSudokuContour;
    std::vector<Pixel*> lines;
    cv::Point2i* intersections[100];
    cv::Mat* cells[81];
    cv::Mat digits[81];

public:
    ImageProcessor(cv::Mat& input);
    ImageProcessor(JNIEnv* env, jobject& input);

    void previewSudoku(cv::Mat& output);
    void previewSudoku(JNIEnv* env, jobject& output);
    void cutDigits(cv::Mat* (&digits)[81]);
private:
    void identifySudoku(int kernelSize = 41);
    void showSudoku(cv::Mat& output);
    void warpSudoku(cv::Mat& output);
    void identifyLines();
    void showLines(cv::Mat& output);
    void calculateIntersections();
    void showIntersections(cv::Mat& output);
    void cutCells();
    void extractDigits(cv::Mat* (&digits)[81]);
};
