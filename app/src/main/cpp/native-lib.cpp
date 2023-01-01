#include <jni.h>
#include <string>
#include <opencv2/core.hpp>
#include <android/bitmap.h>
#include <android/log.h>

#include "utils.h"
#include "imageProcessing.h"

#include <fdeep/fdeep.hpp>
#include <unistd.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "kerasModel.h"
#include "sudokuSolving.h"
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
Java_com_example_deepsudoku_ImageViewFragment_solveSudoku(JNIEnv *env, jobject thiz, jobject inputBitmap, jobject outputBitmap, jintArray sudoku, jintArray solvedSudoku)
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

    //Displaying cut digits
    /*
    int scaleFactor = 5;
    for(int i = 0; i < 81; i++) {
        for (int col = 0; col < (*digits[i]).size().width * scaleFactor; col++) {
            for (int row = 0; row < (*digits[i]).size().height * scaleFactor; row++) {
                outputMatrix.at<uint32_t>(row + (i % 9) * ((*digits[i]).size().height + 1) * scaleFactor, col + (i / 9) * ((*digits[i]).size().width + 1) * scaleFactor) = (*digits[i]).at<uint8_t>(row / scaleFactor, col / scaleFactor);
            }

        }
        outputMatrix.at<uint32_t>(29 + (i / 9) * 29 * 5, 29 + (i % 9) * 29 * 5) = 255;
    }
    */

    int predictions[9][9];
    for(int row = 0; row < 9; row++) {
        for(int col = 0; col < 9; col++) {
            const auto input = fdeep::tensor_from_bytes((*digits[row + col * 9]).ptr(),
                                                        static_cast<std::size_t>((*digits[row + col * 9]).rows),
                                                        static_cast<std::size_t>((*digits[row + col * 9]).cols),
                                                        static_cast<std::size_t>((*digits[row + col * 9]).channels()),
                                                        0.0f, 1.0f);
            const auto result = fdeepModel.predict({input});
            const std::vector<float> vec = result[0].to_vector();
            int prediction = -1;
            for (int i = 0; i < 9; i++) {
                if (vec[i] > 0.7f) {
                    prediction = i;
                    break;
                }
            }
            predictions[row][col] = prediction + 1;
        }
    }
    for(int row = 0; row < 9; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            jint elements[] = { predictions[row][col] };
            env->SetIntArrayRegion(sudoku, row * 9 + col, 1,
                                   elements);
        }
    }
    printSudoku(predictions);
    solveSudoku(predictions, predictions);
    printSudoku(predictions);

    for(int row = 0; row < 9; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            jint elements[] = { predictions[row][col] };
            env->SetIntArrayRegion(solvedSudoku, row * 9 + col, 1,
                                   elements);
        }
    }

    matToBitmap(env, outputMatrix, outputBitmap , false);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_deepsudoku_MainActivity_initKerasModel(JNIEnv *env, jobject thiz) {
    const fdeep::model fdeepModel = fdeep::read_model_from_string(model);
    return (long) new fdeep::model(fdeepModel);
}