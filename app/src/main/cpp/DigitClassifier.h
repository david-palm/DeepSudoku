#pragma once
#include <opencv2/core.hpp>
#include <fdeep/fdeep.hpp>
#include <future>
#include <array>

#include "Sudoku.h"

class DigitClassifier
{
public:
    DigitClassifier(const long aiModelPointer);
    ~DigitClassifier();

    void classifySudoku(cv::Mat* (&digits)[81], Sudoku &sudoku);
private:
    int classifyDigit(cv::Mat* digit);
    void addDigitToPredictions(cv::Mat* digit, int* cell);
private:
    const fdeep::model* m_Model;
    std::vector<std::future<void>> m_Futures;
};


