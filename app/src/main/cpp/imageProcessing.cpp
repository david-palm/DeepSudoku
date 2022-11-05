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

    /* Identifies sudoku contour inside image returns the approximation of it. */
    auto getSudokuContour = [&](std::vector<cv::Point>& approximation)
    {
        /* Helper function to sort contours by largest area */
        auto sortBySmallestArea = [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) -> bool
        {
            return cv::contourArea(contour1) > cv::contourArea(contour2);
        };

        //Finding all contours in the image
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(output, contours, hierarchy, cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_SIMPLE);

        //Sort all contours by largest area to get the approximation
        std::sort(contours.begin(), contours.end(), sortBySmallestArea);

        for(std::vector<cv::Point> contour: contours)
        {
            //Approximate the contour
            cv::approxPolyDP(contour, approximation,
                             0.08 * cv::arcLength(contour, true), true);
            if(approximation.size() == 4) break;
        }

    };

    /* Adds padding to the contour to ensure that the sudoku is not cut off on warped pages */
    auto addPadding = [&](std::vector<cv::Point>& approximation, std::vector<cv::Point>& paddedApproximation)
    {
        auto calculateDistance = [] (cv::Point& point1, cv::Point& point2) -> double
        {
            int aSquared = (point1.x - point2.x) * (point1.x - point2.x);
            int bSquared = (point1.y - point2.y) * (point1.y - point2.y);
            return sqrt(aSquared + bSquared);
        };

        /* Sorts the four points of the approximation (top right, top left, bottom left, bottom right) */
        auto sortPoints = [] (std::vector<cv::Point>& points, std::vector<cv::Point>& sortedPoints)
        {
            auto sortByXCoordinate = [] (cv::Point& point1, cv::Point& point2) -> bool
            { return point1.x > point2.x; };
            auto sortByYCoordinate = [] (cv::Point& point1, cv::Point& point2) -> bool
            { return point1.y < point2.y; };

            sortedPoints.insert(sortedPoints.begin(), points.begin(), points.end());
            //Points are first sorted by Y coordinate
            std::sort(sortedPoints.begin(), sortedPoints.end(), sortByYCoordinate);
            //Then the top two and bottom two points are sorted by X coordinate
            std::sort(sortedPoints.begin(), sortedPoints.begin() + 2, sortByXCoordinate);
            std::sort(sortedPoints.end() - 1, sortedPoints.end(), sortByXCoordinate);
        };

        double distance = calculateDistance(approximation[0], approximation[3]);
        double cellSize = distance / 9;

        sortPoints(approximation, paddedApproximation);

        //Adding padding in proportion to size of sudoku cells
        paddedApproximation[0].x += (int) (cellSize * 0.2);
        paddedApproximation[1].x -= (int) (cellSize * 0.1);
        paddedApproximation[2].x -= (int) (cellSize * 0.1);
        paddedApproximation[3].x += (int) (cellSize * 0.2);

        paddedApproximation[0].y -= (int) (cellSize * 0.3);
        paddedApproximation[1].y -= (int) (cellSize * 0.3);
        paddedApproximation[2].y += (int) (cellSize * 0.37);
        paddedApproximation[3].y += (int) (cellSize * 0.37);
    };

    std::vector<cv::Point> approximation;
    std::vector<cv::Point> paddedApproximation;

    prepareImage();
    getSudokuContour(approximation);
    addPadding(approximation, paddedApproximation);

    //Displaying approximated contour
    cv::drawContours(input, std::vector<std::vector<cv::Point>> { approximation },
                     0, cv::Scalar(0,255,0),25);

    //Displaying approximated contour
    cv::drawContours(input, std::vector<std::vector<cv::Point>> { paddedApproximation },
                     0, cv::Scalar(0,0,255),25);


}
