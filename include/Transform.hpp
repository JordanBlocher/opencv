#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <opencv2/opencv.hpp>
#include <math.h>

namespace Transform
{
    inline cv::Mat Scale(double sx = 1.0, double sy = 1.0)
    {
        return (cv::Mat_<double>(3,3) << sx, 0, 0, 0, sy, 0, 0, 0, 1);
    }

    inline cv::Mat Rotate(double rad = 0.0)
    {
        return (cv::Mat_<double>(3,3) << cos(rad), -sin(rad), 0, sin(rad), cos(rad), 0, 0, 0, 1);
    }

    inline cv::Mat Translate(double tx = 0.0, double ty = 0.0)
    {
        return (cv::Mat_<double>(3,3) << 1, 0, tx, 0, 1, ty, 0, 0, 1);
    }

    inline cv::Mat ShearX(double hx = 0.0)
    {
        return (cv::Mat_<double>(3,3) << 1, 0, 0, hx, 1, 0, 0, 0, 1);
    }
    
    inline cv::Mat ShearY(double hy = 0.0)
    {
        return (cv::Mat_<double>(3,3) << 1, hy, 0, 0, 1, 0, 0, 0, 1);
    }
}

#endif
