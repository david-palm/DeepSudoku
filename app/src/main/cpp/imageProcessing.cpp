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
        cv::GaussianBlur(output, output, cv::Size(kernelSize, kernelSize), 3);

        //Converting image to binary using adaptive threshold
        cv::adaptiveThreshold(output, output, 255,
                              cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                              cv::THRESH_BINARY_INV,199, 25);

        //Image can be dilated and eroded to close gaps in lines
        if(dilating) cv::dilate(output, output,\
               cv::Mat(cv::Size(5,5), CV_8U),\
               cv::Point(-1, -1), 2);

        if(eroding) cv::erode(output, output,\
               cv::Mat(cv::Size(5,5), CV_8U),\
               cv::Point(-1, -1), 2);

    };

    /* Helper function to sort contours by largest area */
    auto sortBySmallestArea = [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
    {
        return cv::contourArea(contour1) > cv::contourArea(contour2);
    };

    /* Identifies sudoku contour inside image returns the approximation of it. */
    auto getSudokuContour = [&]()
    {
        //Finding all contours in the image
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(output, contours, hierarchy, cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_SIMPLE);

        //Sort all contours by largest area to get the approximation
        std::sort(contours.begin(), contours.end(), sortBySmallestArea);
        std::vector<cv::Point> approximation;

        for(std::vector<cv::Point> contour: contours)
        {
            //Approximate the contour
            cv::approxPolyDP(contour, approximation,
                             0.08 * cv::arcLength(contour, true), true);
            if(approximation.size() == 4) break;
        }
        //Displaying approximated contour
        cv::drawContours(input, std::vector<std::vector<cv::Point>> { approximation },
                         0, cv::Scalar(0,255,0),25);
    };
    prepareImage();
    getSudokuContour();

}
