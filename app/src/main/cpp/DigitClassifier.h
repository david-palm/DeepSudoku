#include <opencv2/core.hpp>
#include <fdeep/fdeep.hpp>

class DigitClassifier
{
public:
    DigitClassifier(const long fdeepModelPointer);
    ~DigitClassifier();

    void classifySudoku(cv::Mat* (&digits)[81], int (&predictions)[9][9]);
    int classifyDigit(cv::Mat* digit);
private:
    const fdeep::model* m_Model;
};


