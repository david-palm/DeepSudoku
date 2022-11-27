#include "HoughAccumulator.h"

#include <android/log.h>

HoughAccumulator::HoughAccumulator(cv::Mat& gradientsX, cv::Mat& gradientsY, double thetaResolution, double rhoResolution)
    : m_GradientsX(gradientsX), m_GradientsY(gradientsY), m_ThetaResolution(thetaResolution), m_RhoResolution(rhoResolution)
{
    int x = (int) (M_PI * 2.0 / thetaResolution) + 1;
    int y = (int) (sqrt(pow(gradientsX.size().width, 2) + pow(gradientsX.size().height, 2)) / rhoResolution) + 1;
    m_Accumulator = cv::Mat::zeros(y, x, CV_16UC1);
    m_BinaryAccumulator = cv::Mat::zeros(y, x, CV_16UC1);
    m_MarkedAccumulator = cv::Mat::zeros(y, x, CV_16UC1);
    m_Max = 0;

    __android_log_print(ANDROID_LOG_ERROR, "HoughAccumulator", "x: %d, y: %d", x, y);
    uchar kernelData[25] = { 0, 0, 1, 0, 0,
                            0, 0, 1, 0, 0,
                            0, 0, 1, 0, 0,
                            0, 0, 1, 0, 0,
                            0, 0, 1, 0, 0 };

    lineKernel5 = cv::Mat::ones(1, 5, CV_16UC1);
    squareKernel5 = cv::Mat::ones(5, 5, CV_16UC1);
    squareKernel15 = cv::Mat::ones(15, 15, CV_16UC1);
    squareKernel17 = cv::Mat::ones(17, 17, CV_16UC1);

}

HoughAccumulator::~HoughAccumulator(){

}

void HoughAccumulator::fill(double minMagnitude)
{
    for(int x = 0; x < m_GradientsX.size().width; x++)
    {
        for(int y = 0; y < m_GradientsX.size().height; y++)
        {
            //__android_log_print(ANDROID_LOG_ERROR, "Coordinates", "x: %d, y: %d", x, y);
            double theta, rho, magnitude;
            std::tie (theta, rho, magnitude) = getThetaRhoMagnitude(x, y);


            if(magnitude >= minMagnitude )
            {
                if(!isnan(theta))
                {
                    int houghX = (int) (theta / m_ThetaResolution);
                    int houghY = (int) (rho / m_RhoResolution);

                    m_Accumulator.at<uint16_t>(houghY, houghX) += 1;
                    m_Max = (m_Accumulator.at<ushort>(houghY, houghX) > m_Max)
                            ? m_Accumulator.at<ushort>(houghY, houghX) : m_Max;
                }
            }

        }
    }
}

/* Normalizes all values in the accumulator to 0-255. All cells with less than the minimum number
 * of votes are set to zero. */
void HoughAccumulator::normalize(uint16_t minVotes)
{
    for(int x = 0; x < m_Accumulator.size().width; x++)
    {
        for(int y = 0; y < m_Accumulator.size().height; y++)
        {

            if(m_Accumulator.at<uint16_t>(y, x) < minVotes)
            {
                m_Accumulator.at<uint16_t>(y, x) = 0;
            }
            m_Accumulator.at<uint16_t>(y, x) = (uint16_t)(m_Accumulator.at<uint16_t>(y, x) / (float) m_Max * 255.0f);
        }
    }
}

std::vector<Pixel*> HoughAccumulator::getLines()
{
    cv::threshold(m_Accumulator, m_BinaryAccumulator, 1, 255, CV_16UC1);


    //Dilating and eroding binary accumulator to connect nearby lines
    /*
    cv::erode(m_BinaryAccumulator, m_BinaryAccumulator, lineKernel5, cv::Point(-1, -1),  3);
    cv::erode(m_BinaryAccumulator, m_BinaryAccumulator, lineKernel5, cv::Point(-1, -1),  3);
    cv::dilate(m_BinaryAccumulator, m_BinaryAccumulator, squareKernel5, cv::Point(-1, -1), 2);
    cv::erode(m_BinaryAccumulator, m_BinaryAccumulator, squareKernel5, cv::Point(-1, -1), 2);
    cv::dilate(m_BinaryAccumulator, m_BinaryAccumulator, squareKernel17, cv::Point(-1, -1), 2);
    cv::erode(m_BinaryAccumulator, m_BinaryAccumulator, squareKernel15, cv::Point(-1, -1), 2);
     */


    //Identify lines in the accumulator using flood fill algorithm
    std::vector<std::vector<Pixel*>> lines;
    for(int x = 0; x < m_BinaryAccumulator.size().width; x++)
    {
        for(int y = 0; y < m_BinaryAccumulator.size().height; y++)
        {
            //Create new line which contains a list of pixels
            std::vector<Pixel*>* line = new std::vector<Pixel*>();

            floodFill(x, y, *line);

            if((*line).size() > 0)
            {
                lines.push_back(*line);
            }
        }
    }

    //Calculating the center of all the pixels per line
    std::vector<Pixel*> centeredLines;
    for(std::vector<Pixel*> line : lines)
    {
        double centeredTheta = 0;
        double centeredRho = 0;
        int votes = 0;

        for(Pixel* pixel : line)
        {
            centeredTheta += (*pixel).theta * (*pixel).votes;
            centeredRho += (*pixel).rho * (*pixel).votes;
            votes += (*pixel).votes;
        }
        centeredTheta /= votes;
        centeredRho /= votes;

        centeredLines.push_back(new Pixel(centeredTheta, centeredRho, votes));
    }

    //Helper function that sorts lines by pixel count in descending order
    auto sortLines = [](Pixel* line1, Pixel* line2) -> bool
    {
        return (*line1).votes > (*line2).votes;
    };

    //If more than 20 lines are found, the lines with the least amount of pixels are removed
    if(centeredLines.size() > 20){
        std::sort(centeredLines.begin(), centeredLines.end(), sortLines);
        centeredLines.erase(centeredLines.begin() + 20, centeredLines.end());
    }

    return centeredLines;
}

std::tuple<double, double, double> HoughAccumulator::getThetaRhoMagnitude(int x, int y)
{
    double theta = atan((float)m_GradientsX.at<uchar>(y, x) / (float)m_GradientsY.at<uchar>(y, x));
    double rho = abs(y * cos(theta) + x * sin(theta));
    double magnitude = sqrt(pow((uchar)m_GradientsX.at<uint32_t>(y, x), 2) + pow((uchar)m_GradientsY.at<uint32_t>(y, x), 2));

    return { theta, rho, magnitude };
}

void HoughAccumulator::floodFill(int x, int y, std::vector<Pixel*>& line){
    //If coordinate is not valid, it cannot be filled
    if(x < 0 || y < 0 || x >= m_Accumulator.size().width || y >= m_Accumulator.size().height){ return; }

    //If coordinate is already filled or has less votes than the threshold, it is not going to be filled
    if(m_BinaryAccumulator.at<uint16_t>(y, x) == 0 || m_MarkedAccumulator.at<int16_t>(y, x) > 0){ return; }

    //If pixel is valid, coordinate is marked in the marker and added to the line
    m_MarkedAccumulator.at<uint16_t>(y, x) = 255;
    line.push_back(new Pixel(x * m_ThetaResolution, y * m_RhoResolution,  (uint16_t) m_Accumulator.at<uint16_t>(y, x)));

    //Nearby pixels are also potentially added to line
    floodFill(x + 1, y, line);
    floodFill(x - 1, y, line);
    floodFill(x, y + 1, line);
    floodFill(x, y - 1, line);
}