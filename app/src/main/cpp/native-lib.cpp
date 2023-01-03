#include <jni.h>
#include <string>

#include <android/bitmap.h>
#include <android/log.h>
#include <unistd.h>

#include <opencv2/core.hpp>
#include <fdeep/fdeep.hpp>

#include "utils/cvUtils.h"
#include "utils/jniUtils.h"
#include "imageProcessing/ImageProcessor.h"
#include "kerasModel.h"
#include "SudokuSolver.h"
#include "utils/performance.h"
#include "DigitClassifier.h"
/* Identify sudoku returns an image with the sudoku contour highlighted in green and an array with
 * the coordinates of the sudoku contour. */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_identifySudoku(JNIEnv *env, jobject thiz, jobject inputBitmap, jobject outputBitmap)
{
    //Converting Bitmap to matrix
    cv::Mat inputMatrix;
    cvUtils::bitmapToMat(env, inputBitmap, inputMatrix, 0);

    ImageProcessor imageProcessor(inputMatrix);

    //Identifying sudoku
    cv::Mat outputMatrix;

    imageProcessor.previewSudoku(outputMatrix);

    //Converting matrix back to Bitmap and contour to float array
    cvUtils::matToBitmap(env, outputMatrix, outputBitmap, false);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_solveSudoku(JNIEnv *env, jobject thiz, jlong kerasModelPointer, jobject inputBitmap, jintArray sudoku, jintArray solvedSudoku)
{
    //Converting Bitmap to matrix
    cv::Mat inputMatrix;
    cvUtils::bitmapToMat(env, inputBitmap, inputMatrix, 0);

    cv::Mat output;
    ImageProcessor imageProcessor(inputMatrix);
    imageProcessor.previewSudoku(output);

    cv::Mat* digits[81];
    imageProcessor.cutDigits(digits);

    DigitClassifier classifier(kerasModelPointer);

    int predictions[9][9];
    classifier.classifySudoku(digits, predictions);

    jniUtils::ArrayToJintArray(env, predictions, sudoku);
    SudokuSolver solver = SudokuSolver();
    solver.solve(predictions, predictions);
    jniUtils::ArrayToJintArray(env, predictions, solvedSudoku);

}



extern "C"
JNIEXPORT jlong JNICALL
        Java_com_example_deepsudoku_MainActivityKt_initKerasModel(JNIEnv *env, jclass clazz)
{
    const fdeep::model model = fdeep::read_model_from_string(modelJSON);
    return (long) new fdeep::model(model);
}
