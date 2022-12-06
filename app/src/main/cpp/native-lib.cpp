#include <jni.h>
#include <string>
#include <opencv2/core.hpp>
#include <android/bitmap.h>
#include <android/log.h>

#include "utils.h"
#include "imageProcessing.h"

/* Identify sudoku returns an image with the sudoku contour highlighted in green and an array with
 * the coordinates of the sudoku contour. */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_identifySudoku(JNIEnv *env, jobject thiz, jobject inputBitmap, jobject outputBitmap)
{
    //Converting Bitmap to matrix
    cv::Mat inputMatrix;
    bitmapToMat(env, inputBitmap, inputMatrix, 0);

    //Identifying sudoku
    cv::Mat outputMatrix;
    std::vector<cv::Point> contour;
    identifySudoku(inputMatrix, outputMatrix, contour);

    //Converting matrix back to Bitmap and contour to float array
    matToBitmap(env, outputMatrix, outputBitmap, false);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_solveSudoku(JNIEnv *env, jobject thiz,  jobject inputBitmap, jobject outputBitmap)
{
    //Converting Bitmap to matrix
    cv::Mat inputMatrix;
    bitmapToMat(env, inputBitmap, inputMatrix, 0);

    //Identifying sudoku
    cv::Mat previewMatrix;
    std::vector<cv::Point> contour;
    identifySudoku(inputMatrix, previewMatrix, contour);

    std::vector<cv::Point2f> convertedContour;
    intToFloatContour(contour, convertedContour);

    //Cutting sudoku
    cv::Mat outputMatrix = inputMatrix;
    warpSudoku(inputMatrix, outputMatrix, convertedContour);

    //Identifying lines
    std::vector<Pixel*> lines;
    identifyLines(inputMatrix, outputMatrix, lines);
    //Finding intersections
    cv::Point2i* intersections[100];
    findIntersections(lines, intersections);
    //Displaying intersections
    displayIntersections(outputMatrix, intersections);
    //Cut cells
    cv::Mat* cells[81];
    cutCells(inputMatrix, cells, intersections);

    cv::Mat* digits[81];
    cutDigits(cells, digits);
    __android_log_print(ANDROID_LOG_ERROR, "cutDigits", "digits(3)[%d][%d]", (*digits[2]).size().width, (*digits[2]).size().height);
    for(int col = 0; col < (*digits[2]).size().width; col++)
    {
        for(int row = 0; row < (*digits[2]).size().height; row++)
        {
            __android_log_print(ANDROID_LOG_ERROR, "cutDigits", "Test [%d][%d]", col, row);
            outputMatrix.at<uint32_t>(row, col) = (*digits[2]).at<uint8_t>(row, col);
            __android_log_print(ANDROID_LOG_ERROR, "cutDigits", "Bug");
        }
    }
    __android_log_print(ANDROID_LOG_ERROR, "cutDigits", "Test 3");
    cv::circle(outputMatrix, cv::Point2i((*digits[2]).size().width, (*digits[2]).size().height), 4, cv::Scalar(255, 0, 0), -1);
    matToBitmap(env, outputMatrix, outputBitmap , false);
}