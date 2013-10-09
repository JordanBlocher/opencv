#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

#include "Util.hpp"
#include "Interpolate.hpp"

#include <opencv2/opencv.hpp>
#include <cmath>

enum InterpolateType {BILINEAR = 3, AVERAGE = 2, NEIGHBOR = 1};

namespace AffineTransform
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

    // -Compute inverse transform Ti = inverse(T)
    // -iterate through each location (xdest,ydest) in the destination image
    //         and create a column vector (cv::Mat) D = [xdest;ydest;1]
    //     -compute source location vector which is used to determine where
    //             to sample from in the source image S = Ti * D
    //     -normalize the souce vector S = S * 1/S(3)
    //     -sample src image at location xsrc = S(1), ysrc = S(2) and put
    //             value in dest location
    template< typename T >
    void Transform(cv::Mat &source, const cv::Mat &transform, int xsize, int ysize, InterpolateType type)
    {
        cv::Mat dest(xsize, ysize, source.type());

        for( int i=0; i<xsize; i++ )
            for( int j=0; j<ysize; j++ )
            {
                cv::Vec<double, 3> v(j, i, 1.0);
                cv::Mat tv(v);

                tv = transform.inv() * tv;   
                tv = tv * (1.0 / tv.at<double>(2,0));                        
                if( !Util::isPoint<T>(tv) )
                {
                    switch(type)
                    {
                        case(BILINEAR):
                            dest.at<T>(i, j) = Interpolate::Bilinear<T>(source, tv);
                            break;
                        case(AVERAGE):
                            dest.at<T>(i, j) = Interpolate::Average<T>(source, tv);
                            break;
                        case(NEIGHBOR):
                            dest.at<T>(i, j) = Interpolate::NearestNeighbor<T>(source, tv);
                            break;
                    }
                }
                else
                {
                    dest.at<T>(i, j) = source.at<T>((int)tv.at<double>(1, 0), (int)tv.at<double>(0,0));
                }
            }
        
        source = dest.clone();
    }


}

#endif
