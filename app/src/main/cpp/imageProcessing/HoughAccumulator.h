#pragma once

#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

struct Pixel
{
    double theta;
    double rho;
    int votes;
    Pixel(double theta, double rho, int votes) : theta(theta), rho(rho), votes(votes) {}
};

class HoughAccumulator {
public:
    HoughAccumulator(cv::Mat& gradientsX, cv::Mat& gradientsY, double thetaResolution, double rhoResolution);
    void fill(double minMagnitude = 0);
    void normalize(uint16_t minVotes = 200);
    std::vector<Pixel*> getLines();
    ~HoughAccumulator();
private:
    std::tuple<double, double, double> getThetaRhoMagnitude(int x, int y);
    void erode(cv::Mat& input, cv::Mat& output, int kernelSize, int iterations, bool lineKernel = false);
    void dilate(cv::Mat& input, cv::Mat& output, int kernelSize, int iterations, bool lineKernel = false);
    void floodFill(int x, int y, std::vector<Pixel*>& line);


public:
    cv::Mat m_Accumulator;
    cv::Mat m_BinaryAccumulator;
private:
    cv::Mat m_MarkedAccumulator;

    cv::Mat& m_GradientsX;
    cv::Mat& m_GradientsY;
    double m_ThetaResolution;
    double m_RhoResolution;
    int m_Max;

    cv::Mat lineKernel5;
    cv::Mat squareKernel5;
    cv::Mat squareKernel15;
    cv::Mat squareKernel17;
};
