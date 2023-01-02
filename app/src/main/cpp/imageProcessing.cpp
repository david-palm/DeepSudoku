#include "imageProcessing.h"
#include "HoughAccumulator.h"
#include "performance.h"

#include <android/log.h>

void cutImage(cv::Mat& input, cv::Mat& output, cv::Point2i topLeft, cv::Point2i bottomRight, cv::Point2i offset = cv::Point2i(0, 0))
{
    // x and y coordinates are switched for the top left and bottom right points
    for(int col = 0; col < abs(bottomRight.x - topLeft.x); col++)
    {
        for(int row = 0; row < abs(bottomRight.y- topLeft.y); row++)
        {
            output.at<uint8_t>(offset.x + row, offset.y + col) = input.at<uint8_t>(row + topLeft.y, col + topLeft.x);
        }
    }
};

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

        cv::line(output, pt1, pt2, cv::Scalar(0, 0, 205), 4);
    }
}

bool findIntersections(std::vector<Pixel*>& lines, cv::Point2i* (&intersections)[100])
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
    int numberOfIntersections = 0;
    auto intersectionAlreadyExists = [&] (cv::Point2i& intersection) -> bool
    {
        for(int i = 0; i < numberOfIntersections; i++)
        {
            if(intersection.x == (*intersections[i]).x && intersection.y == (*intersections[i]).y)
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
                    intersections[numberOfIntersections] = intersection;
                    numberOfIntersections++;
                }

            }

        }
    }

    return numberOfIntersections == 100;

}

void displayIntersections(cv::Mat& inputOutput, cv::Point2i* (&intersections)[100])
{
    for(cv::Point2i* intersection : intersections)
    {
        cv::circle(inputOutput, (*intersection), 10, cv::Scalar(0, 255, 0), -1);

    }
}


void cutCells(cv::Mat& input, cv::Mat* (&cells)[81], cv::Point2i* (&intersections)[100])
{
    auto binarizeImage = [&] (cv::Mat& output, int gaussKernelSize = 41)
    {
        cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(output, output, cv::Size(gaussKernelSize, gaussKernelSize), 3);
        cv::adaptiveThreshold(output, output, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                              cv::THRESH_BINARY_INV, 199, 25);
        cv::dilate(output, output, cv::Mat::ones(5, 5, CV_8U), cv::Point2i(-1, -1), 2);
        cv::erode(output, output, cv::Mat::ones(5, 5, CV_8U), cv::Point2i(-1, -1), 2);
    };

    auto convertTo2dArray = [&] (cv::Point2i* (&sortedIntersections)[10][10])
    {
        auto sortByX = [] (cv::Point2i* point1, cv::Point2i* point2) -> bool
        {
            return (*point1).x < (*point2).x;
        };

        auto sortByY = [] (cv::Point2i* point1, cv::Point2i* point2) -> bool
        {
            return (*point1).y < (*point2).y;
        };

        //Sorting coordinates by y coordinate (x and y are flipped)
        std::sort(std::begin(intersections), std::end(intersections), sortByX);
        //Sorting half-sorted coordinates by row (x coordinate)
        for(int row = 0; row < 10; row++)
        {
            for(int col = 0; col < 10; col++)
            {
                sortedIntersections[row][col] = intersections[10 * row + col];
            }
            std::sort(std::begin(sortedIntersections[row]), std::end(sortedIntersections[row]), sortByY);
        }
    };

    cv::Mat binaryImage;
    binarizeImage(binaryImage);

    cv::Point2i* sortedIntersections[10][10];
    convertTo2dArray(sortedIntersections);

    //Cut image
    for(int row = 0; row < 9; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            int width = abs((*sortedIntersections[row + 1][col + 1]).y - (*sortedIntersections[row][col]).y);
            int height = abs((*sortedIntersections[row + 1][col + 1]).x - (*sortedIntersections[row][col]).x);
            cv::Mat* cell = new cv::Mat(height, width, CV_8UC1);
            cutImage(binaryImage, (*cell), (*sortedIntersections[row][col]), *sortedIntersections[row + 1][col + 1]);
            cells[row * 9 + col] = cell;
        }
    }

}

void cutDigits(cv::Mat* (&cells)[81], cv::Mat* (&digits)[81])
{
    Timer("Cut digits");
    for(int i = 0; i < 81; i++)
    {
        //Resizing cell
        cv::Mat* cell = new cv::Mat(50, 50, CV_8U);
        cv::resize((*cells[i]).clone(), (*cell), cv::Size(50, 50), 0, 0, cv::INTER_CUBIC);
        //Find all contours in cell
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours((*cell), contours, hierarchy, cv::RETR_TREE,
                         cv::CHAIN_APPROX_SIMPLE);

        cv::Mat digit = cv::Mat::zeros((*cell).size(), CV_8U);
        cv::Mat* digitContour = new cv::Mat();

        //Iterating over all contours to find the digit contour
        for(std::vector<cv::Point>& contour: contours)
        {
            //Get bounding rectangle of contour to identify the digit contour
            cv::Rect_<int> boundingRectangle = cv::boundingRect(contour);
            cv::Point2i topLeft(boundingRectangle.x, boundingRectangle.y);
            cv::Point2i bottomRight(boundingRectangle.x + boundingRectangle.width, boundingRectangle.y + boundingRectangle.height);
            if((boundingRectangle.height < 46.0) && (boundingRectangle.width < 45.0))
            {
                if( ((float) cv::contourArea(contour) > 70.0f) && (430.0f > (float) cv::contourArea(contour)))
                {
                    if((cv::arcLength(contour, true) > 59.0f) && (cv::arcLength(contour, true) < 183.0f))
                    {
                        double aspectRatio = (double) boundingRectangle.width / (double) boundingRectangle.height;
                        if((aspectRatio > 0.2) && (aspectRatio < 9.0))
                        {
                            if(boundingRectangle.height > 0 && boundingRectangle.width > 0)
                            {
                                digitContour = new cv::Mat(cv::Size(boundingRectangle.width, boundingRectangle.height), CV_8UC1);
                                cutImage((*cell), (*digitContour), topLeft, bottomRight);
                            }
                            else
                            {
                                digitContour = new cv::Mat(cv::Size(50, 50), CV_8UC1);
                                continue;
                            }
                            cv::rectangle((*cell), topLeft, bottomRight, cv::Scalar(255, 255, 255), 2);
                            break;
                        }
                    }
                }
            }
        }

        cv::Point2i offset((*cell).size() / 2 - (*digitContour).size() / 2);

        //Copying digitContour into digit
        digit.setTo(cv::Scalar(0));
        if(((*digitContour).size().width < 50) && ((*digitContour).size().width > 5) && (*digitContour).size().height > 0)
        {
            for(int col = 0; col < (*digitContour).size().width; col++)
            {
                for(int row = 0; row < (*digitContour).size().height; row++)
                {
                    if((*digitContour).at<uint8_t>(row, col) > 0)
                        digit.at<uint8_t>(row + offset.y, col + offset.x) = (*digitContour).at<uint8_t>(row, col);
                }
            }
        }
        delete digitContour;
        //Resizing digit for neural network
        cv::Mat* resizedDigit = new cv::Mat(28, 28, CV_8UC1);
        cv::resize(digit.clone(), (*resizedDigit), cv::Size(28, 28), 0, 0, cv::INTER_AREA);

        digits[i] = resizedDigit;
    }
}