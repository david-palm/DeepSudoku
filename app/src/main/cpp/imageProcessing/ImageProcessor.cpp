#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(cv::Mat& input) :
m_Input(input)
{
}

ImageProcessor::ImageProcessor(JNIEnv* env, jobject& input)
{
    //Converting Bitmap to matrix
    cvUtils::bitmapToMat(env, input, m_Input, 0);
}

void ImageProcessor::previewSudoku(cv::Mat& output) throw(ImageProcessingException)
{
    try {
        cv::Mat outputMatrix;
        cv::Mat warpedSudoku;
        identifySudoku();
        showSudoku(output);
        warpSudoku();
        identifyLines();
        showLines(output);
    }
    catch(ImageProcessingException &exception)
    {
        throw;
    }
}

void ImageProcessor::previewSudoku(JNIEnv* env, jobject& output) throw(ImageProcessingException)
{
    try
    {
        cv::Mat outputMatrix;
        cv::Mat warpedSudoku;
        identifySudoku();
        showSudoku(outputMatrix);
        warpSudoku();
        identifyLines();
        showLines(outputMatrix);
        cvUtils::matToBitmap(env, outputMatrix, output, 0);
    }
    catch(ImageProcessingException &exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "Exception caught while creating preview!");
        throw PreviewException(exception.message);
    }
}

void ImageProcessor::cutDigits(cv::Mat* (&digits)[81]) throw(ImageProcessingException)
{
    try
    {
        calculateIntersections();
        cutCells();
        extractDigits(digits);
    }
    catch(ImageProcessingException &exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "Exception caught while preparing digits!");
        throw;
    }
}

//Private functions
void ImageProcessor::identifySudoku(int kernelSize) throw(ContourNotFoundException)
{
    cv::Mat binaryImage;
    /* Converts the image to binary. */
    auto prepareImage = [&](bool dilating = true, bool eroding = true)
    {
        //Image is convert to grayscale and blurred before conversion to binary image
        cv::cvtColor(m_Input, binaryImage, cv::COLOR_BGR2GRAY);
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

            point.x = (point.x > m_Input.cols) ? m_Input.cols : point.x;
            point.y = (point.y > m_Input.rows) ? m_Input.rows : point.y;
        }

    };

    std::vector<cv::Point> approximation;
    std::vector<cv::Point> paddedApproximation;

    try
    {
        prepareImage();
        getSudokuContour(approximation);
        addPadding(approximation, paddedApproximation);

        m_SudokuContour = approximation;
        cvUtils::intToFloatContour(paddedApproximation, m_PaddedSudokuContour);
    }
    catch(std::exception& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Sudoku contour not found!", exception.what());
        throw ContourNotFoundException("Sudoku contour not found!");
    }
}

void ImageProcessor::showSudoku(cv::Mat& output) throw(ContourNotFoundException)
{
    try {
        output = m_Input.clone();
        //Displaying approximated contour
        cv::drawContours(output, std::vector<std::vector<cv::Point>>{m_SudokuContour},
                         0, cv::Scalar(72, 79, 184), 10);
        //Displaying contour points
        for (cv::Point point: m_SudokuContour) {
            cv::circle(output, point, 20, cv::Scalar(29, 59, 66), -1);
        }
    }
    catch(std::exception& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Sudoku contour not found!", exception.what());
        throw ContourNotFoundException("Sudoku contour not found!");
    }
}

void ImageProcessor::warpSudoku() throw(ImageNotWarpedException)
{
    try {
        std::vector<cv::Point2f> corners = {cv::Point2f(m_Input.cols, 0),
                                            cv::Point2f(0, 0),
                                            cv::Point2f(0, m_Input.rows),
                                            cv::Point2f(m_Input.cols, m_Input.rows)};
        cv::Mat transform = cv::getPerspectiveTransform(m_PaddedSudokuContour, corners);
        cv::warpPerspective(m_Input, m_WarpedSudoku, transform, m_Input.size());
    }
    catch(std::exception& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Image not warped!", exception.what());
        throw ImageNotWarpedException("Image not warped!");
    }
}

void ImageProcessor::identifyLines() throw(LinesNotFoundException)
{
    /* Creates x and y gradient images from input */

    auto createGradientImages = [&](cv::Mat &gradientX, cv::Mat &gradientY, int kernelSize = 25) {
        /* Converts image to grayscale and blurs it */
        auto prepareImage = [&](cv::Mat &output) {
            cv::cvtColor(m_WarpedSudoku, output, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(output, output, cv::Size(kernelSize, kernelSize), 3);
        };

        /* Filters images with sobel kernel to create gradient images */
        auto filterImage = [&](cv::Mat &preparedImage, cv::Mat &gradientX, cv::Mat &gradientY) {
            float kernelValuesX[9] = {(-1.0f / 8.0f), 0, (1.0f / 8.0f),
                                      (-2.0f / 8.0f), 0, (2.0f / 8.0f),
                                      (-1.0f / 8.0f), 0, (1.0f / 8.0f)};
            cv::Mat sobelKernelX(3, 3, CV_32F, kernelValuesX);
            float kernelValuesY[9] = {(1.0f / 8.0f), (2.0f / 8.0f), (1.0f / 8.0f),
                                      0, 0, 0,
                                      (-1.0f / 8.0f), (-2.0f / 8.0f), (-1.0f / 8.0f)};
            cv::Mat sobelKernelY(3, 3, CV_32F, kernelValuesY);
            cv::filter2D(preparedImage, gradientX, CV_8UC1, sobelKernelX);
            cv::filter2D(preparedImage, gradientY, CV_8UC1, sobelKernelY);
        };

        cv::Mat preparedImage;
        prepareImage(preparedImage);

        filterImage(preparedImage, gradientX, gradientY);
    };

    try
    {
        cv::Mat gradientX, gradientY;
        createGradientImages(gradientX, gradientY);

        HoughAccumulator acc(gradientX, gradientY, M_PI / 720.0, 1);
        acc.fill();
        acc.normalize();
        lines = acc.getLines();
        int horizontalLines = 0;
        int verticalLines = 0;
        bool linesTooClose = false;

        for(Pixel* line : lines)
        {
            horizontalLines = (abs(line->theta) > 0.1) ? horizontalLines + 1 : horizontalLines;
            verticalLines = (abs(line->theta - ( M_PI / 2)) > 0.1)  ? verticalLines + 1 : verticalLines;
            if(!linesTooClose) {
                for (Pixel *line2: lines) {
                    if (line2->rho != line->rho && abs(line->theta - line2->theta) < 0.1 &&abs(line->rho - line2->rho) < 70) linesTooClose = true;
                }
            }
        }

        if(lines.size() != 20) throw LinesNotFoundException("Not enough Lines found!");
        if(horizontalLines != 10 || verticalLines != 10) throw LinesNotFoundException("Not enough vertical or horizontal lines identified!");
        if(linesTooClose) throw LinesNotFoundException("Lines too close!");
    }
    catch(...)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Lines not found!");
        throw LinesNotFoundException("Not enough Lines found!");
    }
}

void ImageProcessor::showLines(cv::Mat& output) throw(LinesNotFoundException)
{
    try {
        //Creating line image
        output = m_WarpedSudoku.clone();
        for (Pixel *line: lines) {
            double a = cos((*line).theta);
            double b = sin((*line).theta);

            double x0 = a * (*line).rho;
            double y0 = b * (*line).rho;

            cv::Point2i pt1((int) (y0 + m_WarpedSudoku.size().height * a),
                            (int) (x0 + m_WarpedSudoku.size().width * (-b)));
            cv::Point2i pt2((int) (y0 - m_WarpedSudoku.size().height * a),
                            (int) (x0 - m_WarpedSudoku.size().width * (-b)));

            cv::line(output, pt1, pt2, cv::Scalar(0, 144, 235), 10);
        }
    }
    catch(std::exception& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Lines not found!", exception.what());
        throw LinesNotFoundException("Lines not found!");
    }
}

void ImageProcessor::calculateIntersections() throw(IntersectionsNotFoundException)
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

    try
    {
        //Comparing all lines with each other to find all intersections
        for (Pixel *line1: lines) {
            for (Pixel *line2: lines) {
                cv::Point2i *intersection = getIntersection((*line1), (*line2));

                if (intersection != NULL) {

                    if (!intersectionAlreadyExists(*intersection)) {
                        intersections[numberOfIntersections] = intersection;
                        numberOfIntersections++;
                    }

                }
            }
        }
    }
    catch(...)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Intersections not found!");
        throw IntersectionsNotFoundException("Intersections not found!");
    }
}

void ImageProcessor::showIntersections(cv::Mat &input) throw(IntersectionsNotFoundException)
{
    try
    {
        for (cv::Point2i *intersection: intersections) {
            cv::circle(input, *intersection, 25, cv::Scalar(219, 58, 0), -1);
        }
    }
    catch(std::exception& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Intersections not found!", exception.what());
        throw IntersectionsNotFoundException("Intersections not found!");
    }
}

void ImageProcessor::cutCells() throw(CellsNotCutException)
{
    auto binarizeImage = [&] (cv::Mat& output, int gaussKernelSize = 41)
    {
        cv::cvtColor(m_WarpedSudoku, output, cv::COLOR_BGR2GRAY);
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

    cv::Point2i *sortedIntersections[10][10];
    try
    {
        convertTo2dArray(sortedIntersections);

        //Cut image
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                int width = abs((*sortedIntersections[row + 1][col + 1]).y -
                                (*sortedIntersections[row][col]).y);
                int height = abs((*sortedIntersections[row + 1][col + 1]).x -
                                 (*sortedIntersections[row][col]).x);
                cv::Mat *cell = new cv::Mat(height, width, CV_8UC1);
                cvUtils::cutImage(binaryImage, (*cell), (*sortedIntersections[row][col]),
                                  *sortedIntersections[row + 1][col + 1]);
                cells[row * 9 + col] = cell;
            }
        }
    }
    catch(...)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Cells can not be cut!");
        throw CellsNotCutException("Cells not cut!");
    }
}

void ImageProcessor::showCells(cv::Mat& output, float scale) throw(CellsNotCutException)
{
    output = cv::Mat(m_WarpedSudoku);
    try
    {
        for (int i = 0; i < 81; i++) {
            for (int col = 0; col < (*cells[i]).size().width * scale; col++) {
                for (int row = 0; row < (*cells[i]).size().height * scale; row++) {
                    output.at<uint32_t>(row + (i % 9) * ((*cells[i]).size().height + 1) * scale,
                                        col + (i / 9) * ((*cells[i]).size().width + 1) *
                                              scale) = (*cells[i]).at<uint8_t>(row / scale,
                                                                               col / scale);
                }
            }
            output.at<uint32_t>(29 + (i / 9) * 29 * 5, 29 + (i % 9) * 29 * 5) = 255;
        }
    }
    catch(...)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Cells can not be cut!");
        throw CellsNotCutException("Cells not cut!");
    }
}

void ImageProcessor::extractDigits(cv::Mat* (&digits)[81]) throw(DigitsNotExtractedException)
{
    try {
        for (int i = 0; i < 81; i++) {
            //Resizing cell
            cv::Mat *cell = new cv::Mat(50, 50, CV_8U);
            cv::resize((*cells[i]).clone(), (*cell), cv::Size(50, 50), 0, 0, cv::INTER_CUBIC);
            //Find all contours in cell
            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours((*cell), contours, hierarchy, cv::RETR_TREE,
                             cv::CHAIN_APPROX_SIMPLE);

            cv::Mat digit = cv::Mat::zeros((*cell).size(), CV_8U);
            cv::Mat *digitContour = new cv::Mat();

            //Iterating over all contours to find the digit contour
            for (std::vector<cv::Point> &contour: contours) {
                //Get bounding rectangle of contour to identify the digit contour
                cv::Rect_<int> boundingRectangle = cv::boundingRect(contour);
                cv::Point2i topLeft(boundingRectangle.x, boundingRectangle.y);
                cv::Point2i bottomRight(boundingRectangle.x + boundingRectangle.width,
                                        boundingRectangle.y + boundingRectangle.height);
                if ((boundingRectangle.height < 46.0) && (boundingRectangle.width < 45.0)) {
                    if (((float) cv::contourArea(contour) > 70.0f) &&
                        (430.0f > (float) cv::contourArea(contour))) {
                        if ((cv::arcLength(contour, true) > 59.0f) &&
                            (cv::arcLength(contour, true) < 183.0f)) {
                            double aspectRatio = (double) boundingRectangle.width /
                                                 (double) boundingRectangle.height;
                            if ((aspectRatio > 0.2) && (aspectRatio < 9.0)) {
                                if (boundingRectangle.height > 0 && boundingRectangle.width > 0) {
                                    digitContour = new cv::Mat(cv::Size(boundingRectangle.width,
                                                                        boundingRectangle.height),
                                                               CV_8UC1);
                                    cvUtils::cutImage((*cell), (*digitContour), topLeft,
                                                      bottomRight);
                                } else {
                                    digitContour = new cv::Mat(cv::Size(50, 50), CV_8UC1);
                                    continue;
                                }
                                cv::rectangle((*cell), topLeft, bottomRight,
                                              cv::Scalar(255, 255, 255), 2);
                                break;
                            }
                        }
                    }
                }
            }

            cv::Point2i offset((*cell).size() / 2 - (*digitContour).size() / 2);

            //Copying digitContour into digit
            digit.setTo(cv::Scalar(0));
            if (((*digitContour).size().width < 50) && ((*digitContour).size().width > 5) &&
                (*digitContour).size().height > 0) {
                for (int col = 0; col < (*digitContour).size().width; col++) {
                    for (int row = 0; row < (*digitContour).size().height; row++) {
                        if ((*digitContour).at<uint8_t>(row, col) > 0)
                            digit.at<uint8_t>(row + offset.y,
                                              col + offset.x) = (*digitContour).at<uint8_t>(row,
                                                                                            col);
                    }
                }
            }
            delete digitContour;
            //Resizing digit for neural network
            cv::Mat *resizedDigit = new cv::Mat(28, 28, CV_8UC1);
            cv::resize(digit.clone(), (*resizedDigit), cv::Size(28, 28), 0, 0, cv::INTER_AREA);

            digits[i] = resizedDigit;
        }
    }
    catch(std::exception& exception)
    {
        __android_log_print(ANDROID_LOG_ERROR, "ImageProcessor", "%s: Digits can not be extracted!", exception.what());
        throw CellsNotCutException("Digits not extracted!");
    }
}
