#pragma once

#include <jni.h>
#include <opencv2/core.hpp>
#include <android/bitmap.h>
#include <android/log.h>

#include "../utils/cvUtils.h"
#include "HoughAccumulator.h"
#include "ImageProcessingExceptions.h"

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
    cv::Mat m_WarpedSudoku;
    std::vector<Pixel*> lines;
    cv::Point2i* intersections[100];
    cv::Mat* cells[81];
    cv::Mat digits[81];

public:
    ImageProcessor(cv::Mat& input);
    ImageProcessor(JNIEnv* env, jobject& input);

    void previewSudoku(cv::Mat& output) throw(ImageProcessingException);
    void previewSudoku(JNIEnv* env, jobject& output) throw(ImageProcessingException);
    void cutDigits(cv::Mat* (&digits)[81]) throw(ImageProcessingException);
private:
    void identifySudoku(int kernelSize = 41) throw(ContourNotFoundException);
    void showSudoku(cv::Mat& output) throw(ContourNotFoundException);
    void warpSudoku() throw(ImageNotWarpedException);
    void identifyLines() throw(LinesNotFoundException);
    void showLines(cv::Mat& output) throw(LinesNotFoundException);
    void calculateIntersections() throw(IntersectionsNotFoundException);
    void showIntersections(cv::Mat& output) throw(IntersectionsNotFoundException);
    void cutCells() throw(CellsNotCutException);
    void showCells(cv::Mat& output, float scale = 5) throw(CellsNotCutException);
    void extractDigits(cv::Mat* (&digits)[81]) throw(DigitsNotExtractedException);
};
