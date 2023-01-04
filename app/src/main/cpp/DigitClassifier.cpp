#include "DigitClassifier.h"

DigitClassifier::DigitClassifier(const long aiModelPointer)
{
    m_Model = (fdeep::model*) aiModelPointer;
}

DigitClassifier::~DigitClassifier()
{
}

void DigitClassifier::classifySudoku(cv::Mat* (&digits)[81], int (&predictions)[9][9])
{
    for(int row = 0; row < 9; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            predictions[row][col] = classifyDigit(digits[row + col * 9]);
        }
    }
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