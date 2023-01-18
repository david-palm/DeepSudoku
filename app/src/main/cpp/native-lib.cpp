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
#include "Sudoku.h"
/* Identify sudoku returns an image with the sudoku contour highlighted in green and an array with
 * the coordinates of the sudoku contour. */
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_deepsudoku_ImageViewFragment_identifySudoku(JNIEnv *env, jobject thiz, jobject inputBitmap, jobject outputBitmap)
{
    ImageProcessor *imageProcessor = new ImageProcessor(env, inputBitmap);
    try
    {
        imageProcessor->previewSudoku(env, outputBitmap);
    }
    catch(ContourNotFoundException& exception)
    {
        jclass clazz = env->FindClass("com/example/deepsudoku/exceptions/ContourNotFoundException");
        if (clazz != NULL) {
            env->ThrowNew(clazz, "Sudoku contour could not be found!");
        }
        env->DeleteLocalRef(clazz);
    }
    catch(ImageNotWarpedException& exception)
    {
        jclass clazz = env->FindClass("com/example/deepsudoku/exceptions/ImageNotWarpedException");
        if (clazz != NULL) {
            env->ThrowNew(clazz, "Image could not be warped!");
        }
        env->DeleteLocalRef(clazz);
    }
    catch(LinesNotFoundException& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "Creating JAVA exception!");
        jclass clazz = env->FindClass("com/example/deepsudoku/exceptions/LinesNotFoundException");
        if (clazz != NULL) {
            env->ThrowNew(clazz, "Lines can not be identified!");
        }
        env->DeleteLocalRef(clazz);
    }
    catch(...)
    {
        jclass clazz = env->FindClass("java/lang/Exception");
        if (clazz != NULL) {
            env->ThrowNew(clazz, "An error occurred while creating the preview!");
        }
        env->DeleteLocalRef(clazz);
    }
    return (long) imageProcessor;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_deepsudoku_ImageViewFragment_solveSudoku(JNIEnv *env, jobject thiz, jlong aiModelPointer, jlong imageProcessorPointer, jintArray sudoku, jintArray solvedSudoku)
{
    ImageProcessor* imageProcessor =  (ImageProcessor*) imageProcessorPointer;

    cv::Mat* digits[81];

    try
    {
        __android_log_print(ANDROID_LOG_ERROR, "SolveSudoku", "Test");
        imageProcessor->cutDigits(digits);
        DigitClassifier classifier(aiModelPointer);

        Sudoku sudokuObject = Sudoku();
        classifier.classifySudoku(digits, sudokuObject);

        jniUtils::ArrayToJintArray(env, sudokuObject.m_ScannedDigits, sudoku);
        SudokuSolver solver = SudokuSolver();
        //solver.print(sudokuObject.m_ScannedDigits);
        solver.solve(sudokuObject.m_ScannedDigits, sudokuObject.m_ScannedDigits);
        jniUtils::ArrayToJintArray(env, sudokuObject.m_ScannedDigits, solvedSudoku);

    }
    catch(std::exception &exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "SolveSudoku", "Exception catched!");
        std::array<std::array<int, 9>,9> dummy = std::array<std::array<int, 9>,9>();
        jniUtils::ArrayToJintArray(env, dummy, sudoku);
        jniUtils::ArrayToJintArray(env, dummy, solvedSudoku);
        __android_log_print(ANDROID_LOG_ERROR, "SolveSudoku", "An error with code  occurred!");
    }
}



extern "C"
JNIEXPORT jlong JNICALL
        Java_com_example_deepsudoku_MainActivityKt_initAiModel(JNIEnv *env, jclass clazz)
{
    const fdeep::model model = fdeep::read_model_from_string(modelJSON);
    return (long) new fdeep::model(model);
}
