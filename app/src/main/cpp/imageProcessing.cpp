#include "imageProcessing.h"

/* Identifies sudoku in input image and returns an image with the sudoku warped to fit the screen.
 * Padding can be set to true in order to avoid cutting parts of the sudoku of if the page is warped.
 * KernelSize sets the height and width of the kernel in pixels that is used to blur the image with,
 * before converting it to a binary image.
*/
void cutSudoku(cv::Mat& input, cv::Mat& output, bool padding, int kernelSize)
{
    /* Converts the image to binary. */
    auto prepareImage = [&](bool dilating = true, bool eroding = true)
    {
        //Image is convert to grayscale and blurred before conversion to binary image
        cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(input, output, cv::Size(kernelSize, kernelSize), 3);

        //Converting image to binary using adaptive threshold
        cv::adaptiveThreshold(input, output, 255,
                              cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                              cv::THRESH_BINARY_INV,199, 25);

        //Image can be dilated and eroded to close gaps in lines
        if(dilating) cv::dilate(input, output,\
               cv::Mat(cv::Size(5,5), CV_8U),\
               cv::Point(-1, -1), 2);

        if(eroding) cv::erode(input, output,\
               cv::Mat(cv::Size(5,5), CV_8U),\
               cv::Point(-1, -1), 2);

    };
    prepareImage();
}
