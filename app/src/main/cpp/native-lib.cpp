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
    std::vector<cv::Point2i*> intersections;
    findIntersections(lines, intersections);
    __android_log_print(ANDROID_LOG_ERROR, "TRACKERS", "Intersections: %d", intersections.size());
    //Displaying intersections
    displayIntersections(outputMatrix, intersections);

    
    matToBitmap(env, outputMatrix, outputBitmap, false);
}