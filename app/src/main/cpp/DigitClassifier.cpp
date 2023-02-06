#include "DigitClassifier.h"
#include <future>
#include <android/log.h>

#include "SudokuSolver.h"

DigitClassifier::DigitClassifier(const long aiModelPointer)
{
    m_Model = (fdeep::model*) aiModelPointer;
}

DigitClassifier::~DigitClassifier()
{
}

void DigitClassifier::addDigitToPredictions(cv::Mat* digit, int* cell)
{
    *cell = this->classifyDigit(digit);

    __android_log_print(ANDROID_LOG_DEBUG, "DigitClassifier", "Classified digit as %d", *cell);
}

void DigitClassifier::classifySudoku(cv::Mat* (&digits)[81], Sudoku &sudoku)
{
    //TODO: Initialize futures here
    {
        std::vector<std::future<void>> futures;
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                futures.push_back(
                        std::async(std::launch::async, &DigitClassifier::addDigitToPredictions,
                                   this, digits[row + col * 9], &sudoku.m_ScannedDigits[row][col]));
                //addDigitToPredictions(digits[row + col * 9], &sudoku.m_ScannedDigits[row][col]);
                //__android_log_print(ANDROID_LOG_DEBUG, "DigitClassifier", "2Classified digit as %d", predictions[row][col]);
            }
        }
    }
    SudokuSolver s;
    s.print(sudoku.m_ScannedDigits);
}

int DigitClassifier::classifyDigit(cv::Mat* digit)
{
    //Converting OpenCV matrix to frugally-deep tensor
    const auto input = fdeep::tensor_from_bytes((*digit).ptr(),
                                                static_cast<std::size_t>((*digit).rows),
                                                static_cast<std::size_t>((*digit).cols),
                                                static_cast<std::size_t>((*digit).channels()),
                                                0.0f, 1.0f);

    const auto result = m_Model->predict({input});
    const std::vector<float> vec = result[0].to_vector();
    int prediction = -1;
    for (int i = 0; i < 9; i++) {
        if (vec[i] > 0.7f) {
            prediction = i;
            break;
        }
    }
    return prediction + 1;
}