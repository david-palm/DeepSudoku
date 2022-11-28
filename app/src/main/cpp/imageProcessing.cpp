#include "imageProcessing.h"
#include "HoughAccumulator.h"

#include <android/log.h>

/* Identifies sudoku in input image and returns an array of four points that make up the sudoku.
 * Padding can be set to true in order to avoid cutting parts of the sudoku of when the page is warped.
 * KernelSize sets the height and width of the kernel in pixels that is used to blur the image with,
 * before converting it to a binary image.
*/
void identifySudoku(cv::Mat& input, cv::Mat& output, std::vector<cv::Point>& contour, bool padding, int kernelSize )
{
    cv::Mat binaryImage;
    /* Converts the image to binary. */
    auto prepareImage = [&](bool dilating = true, bool eroding = true)
    {
        //Image is convert to grayscale and blurred before conversion to binary image
        cv::cvtColor(input, binaryImage, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(binaryImage, binaryImage, cv::Size(kernelSize, kernelSize), 3);

        //Converting image to binary using adaptive threshold
        cv::adaptiveThreshold(binaryImage, binaryImage, 255,
                              cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                              cv::THRESH_BINARY_INV,199, 25);

        //Image can be dilated and eroded to close gaps in lines
        if(dilating) cv::dilate(binaryImage, binaryImage,\
               cv::Mat(cv::Size(5,5), CV_8U),\
               cv::Point(-1, -1), 2);

        if(eroding) cv::erode(binaryImage, binaryImage,\
               cv::Mat(cv::Size(5,5), CV_8U),\
               cv::Point(-1, -1), 2);

    };

    /* Identifies sudoku contour inside image returns the approximation of it. */
    auto getSudokuContour = [&](std::vector<cv::Point>& approximation)
    {
        /* Helper function to sort contours by largest area */
        auto sortBySmallestArea = [](std::vector<cv::Point>& contour1, std::vector<cv::Point>& contour2) -> bool
        {
            return cv::contourArea(contour1) > cv::contourArea(contour2);
        };

        //Finding all contours in the image
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(binaryImage, contours, hierarchy, cv::RETR_EXTERNAL,
                         cv::CHAIN_APPROX_SIMPLE);

        //Sort all contours by largest area to get the approximation
        std::sort(contours.begin(), contours.end(), sortBySmallestArea);

        for(std::vector<cv::Point>& contour: contours)
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
            auto sortByXGreater = [] (cv::Point& point1, cv::Point& point2) -> bool
            { return point1.x > point2.x; };
            auto sortByXLower = [] (cv::Point& point1, cv::Point& point2) -> bool
            { return point1.x < point2.x; };
            auto sortByYGreater = [] (cv::Point& point1, cv::Point& point2) -> bool
            { return point1.y < point2.y; };

            sortedPoints.insert(sortedPoints.begin(), points.begin(), points.end());
            //Points are first sorted by Y coordinate
            std::sort(sortedPoints.begin(), sortedPoints.end(), sortByYGreater);
            //Then the top two and bottom two points are sorted by X coordinate
            std::sort(sortedPoints.begin(), sortedPoints.begin() + 2, sortByXGreater);
            std::sort(sortedPoints.end() - 2, sortedPoints.end(), sortByXLower);
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

        //Clamping values if points are outside of image
        for(cv::Point& point : paddedApproximation)
        {
            point.x = (point.x < 0) ? 0 : point.x;
            point.y = (point.y < 0) ? 0 : point.y;

            point.x = (point.x > input.cols) ? input.cols : point.x;
            point.y = (point.y > input.rows) ? input.rows : point.y;
        }

    };

    std::vector<cv::Point> approximation;
    std::vector<cv::Point> paddedApproximation;

    prepareImage();
    getSudokuContour(approximation);
    addPadding(approximation, paddedApproximation);

    output = input.clone();
    //Displaying approximated contour
    cv::drawContours(output, std::vector<std::vector<cv::Point>> { approximation },
                     0, cv::Scalar(0,255,0),25);

    //Displaying approximated contour
    cv::drawContours(output, std::vector<std::vector<cv::Point>> { paddedApproximation },
                     0, cv::Scalar(0,0,255),25);
    contour = paddedApproximation;
}

/* Warps the sudoku to fill image. Contour points need to be in following order: tr, tl, bl, br. */
void warpSudoku(cv::Mat& input, cv::Mat& warped, std::vector<cv::Point2f>& contour)
{

    std::vector<cv::Point2f> corners = { cv::Point2f(warped.cols, 0),
                                         cv::Point2f(0, 0),
                                         cv::Point2f(0, warped.rows),
                                         cv::Point2f(warped.cols, warped.rows) };
    cv::Mat transform = cv::getPerspectiveTransform(contour, corners);
    cv::warpPerspective(input, warped, transform, warped.size());
}

void identifyLines(cv::Mat& input, cv::Mat& output, std::vector<Pixel*>& lines )
{

    /* Creates x and y gradient images from input */

    auto createGradientImages = [&] (cv::Mat& gradientX, cv::Mat& gradientY, int kernelSize = 25)
    {
        /* Converts image to grayscale and blurs it */
        auto prepareImage = [&] (cv::Mat& output)
        {
            cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(output, output, cv::Size(kernelSize, kernelSize), 3);
        };

        /* Filters images with sobel kernel to create gradient images */
        auto filterImage = [&] (cv::Mat& preparedImage, cv::Mat& gradientX, cv::Mat& gradientY)
        {
            float kernelValuesX[9] = {  (- 1.0f / 8.0f), 0, (1.0f / 8.0f),
                                        (- 2.0f / 8.0f), 0, (2.0f / 8.0f),
                                        (- 1.0f / 8.0f), 0, (1.0f / 8.0f) };
            cv::Mat sobelKernelX(3, 3, CV_32F, kernelValuesX);
            float kernelValuesY[9] = {  (1.0f / 8.0f), (2.0f / 8.0f), (1.0f / 8.0f),
                                        0, 0, 0,
                                        (- 1.0f / 8.0f), (- 2.0f / 8.0f), (- 1.0f / 8.0f) };
            cv::Mat sobelKernelY(3, 3, CV_32F, kernelValuesY);
            cv::filter2D(preparedImage, gradientX, CV_8UC1, sobelKernelX);
            cv::filter2D(preparedImage, gradientY, CV_8UC1, sobelKernelY);
        };

        cv::Mat preparedImage;
        prepareImage(preparedImage);

        filterImage(preparedImage, gradientX, gradientY);
    };

    cv::Mat gradientX, gradientY;
    createGradientImages(gradientX, gradientY);

    HoughAccumulator acc(gradientX, gradientY, M_PI / 720.0, 1);
    acc.fill();
    acc.normalize();
    lines = acc.getLines();

    //Creating line image
    output = input;
    for(Pixel* line : lines)
    {
        double a = cos((*line).theta);
        double b = sin((*line).theta);

        double x0 = a * (*line).rho;
        double y0 = b * (*line).rho;

        cv::Point2i pt1((int) (y0 + input.size().height * a), (int) (x0 + input.size().width * (-b)));
        cv::Point2i pt2((int) (y0 - input.size().height * a), (int) (x0 - input.size().width * (-b)));

        cv::line(output, pt1, pt2, cv::Scalar(255, 0, 0), 4);
    }
}

void findIntersections(std::vector<Pixel*>& lines, std::vector<cv::Point2i*>& intersections)
{
    auto getIntersection = [&] (Pixel& line1, Pixel& line2) -> cv::Point2i*
    {
        //Helper functions that returns true if two values are in threshold
        auto isClose = [] (double value1, double value2, double threshold) -> bool
        {
            return abs(value1 - value2) < threshold;
        };

        //Lines do not have an intersection when they have the same angle
        if(isClose(line1.theta, line2.theta, 0.2))
        {
            return NULL;
        }
        //If lines are not parallel they intersect at a 90 degree angle
        else
        {
            if(line1.theta == 0)
                return new cv::Point2i(line2.rho, line1.rho);
            if(line2.theta == 0)
                return new cv::Point2i(line1.rho, line2.rho);
        }
    };

    auto intersectionAlreadyExists = [&] (cv::Point2i& intersection) -> bool
    {
        for(cv::Point2i* existingIntersection : intersections)
        {
            if(intersection.x == (*existingIntersection).x && intersection.y == (*existingIntersection).y)
                return true;
        }
        return false;
    };

    __android_log_print(ANDROID_LOG_ERROR, "TRACKERS", "Lines: %d", lines.size());

    //Comparing all lines with each other to find all intersections
    for(Pixel* line1 : lines)
    {
        for(Pixel* line2 : lines)
        {
            cv::Point2i* intersection = getIntersection((*line1), (*line2));
            if(intersection != NULL)
            {
                if(!intersectionAlreadyExists(*intersection))
                {
                    intersections.push_back(intersection);
                }
            }
        }
    }
}

void displayIntersections(cv::Mat& inputOutput, std::vector<cv::Point2i*>& intersections)
{
    for(cv::Point2i* intersection : intersections)
    {
        cv::circle(inputOutput, (*intersection), 25, cv::Scalar(0, 0, 255), -1);

    }
}

void cutCells(cv::Mat& input, cv::Mat (&cells)[81], cv::Point2i* (&intersections)[100])
{
    auto binarizeImage = [&] (cv::Mat& output, int gaussKernelSize = 41)
    {
        cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(output, output, cv::Size(gaussKernelSize, gaussKernelSize), 3);
        cv::adaptiveThreshold(output, output, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                              cv::THRESH_BINARY_INV, 199, 25);
        cv::dilate(output, output, cv::Mat::ones(5, 5, CV_8UC1), cv::Point2i(-1, -1), 2);
        cv::erode(output, output, cv::Mat::ones(5, 5, CV_8UC1), cv::Point2i(-1, -1), 2);
    };

    cv::Mat binaryImage;
    binarizeImage(binaryImage);




}