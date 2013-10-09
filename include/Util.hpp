#ifndef UTIL_H
#define UTIL_H

#include "Image.hpp"

#include <opencv2/opencv.hpp>
#include <cmath>
#include <assert.h>

namespace Util
{

    template< typename T>
    void PadImage(cv::Mat& padded, const cv::Mat &source, int padX, int padY)
    {
        padded = cv::Mat( source.rows + padY, source.cols + padX, source.type(), cv::Scalar(0.0));
        cv::Mat roi = padded(cv::Rect(padX/2, padY/2, source.cols, source.rows));
        source.copyTo(roi);
    }

    template< typename T >
    inline bool isPoint(const cv::Mat &v)
    { 
        double v0 = v.at<double>(0,0);
        double v1 = v.at<double>(0,1);
        return (( (floor(v0) - ceil(v0) == 0) ) && ( (floor(v1) - ceil(v1) == 0) )); 
    }

    template< class T >
    double ComputeSquareError(const cv::Mat &source, const cv::Mat &compare, cv::Mat &squareError, int size, int channel)
    {
        squareError = cv::Mat(1, size, CV_64FC1);
        double sum = 0;
        if(source.cols != compare.cols || source.rows != compare.rows)
        {
            std::cout<< "Images must have the same dimension";
            return sum;
        }
        for( int i=0; i<source.rows; i++ )
            for( int j=0; j<source.cols; j++ )
            {
                squareError.at<double>(0, j) = (double)pow(source.at<uchar>(i, j, channel) - compare.at<uchar>(i, j, channel), 2) / pow(size, 2);
                sum += squareError.at<double>(0, j);
            }
        return sum; 
    }


    // -normalize values to [0, size]
    template< class T >
    void Normalize(cv::Mat &mat, const cv::Scalar &size, int channel)
    {
        double min, max;
        cv::minMaxLoc(mat, &min, &max);
        for(int i=0; i<mat.rows; i++)
        {
            for(int j=0; j<mat.cols; j++)
            {
                mat.at<T>(i, j, channel) -= min;
                mat.at<T>(i, j, channel) *= ( size[channel] / (max - min));
            }
        }
    }

    // -define histogram as size = image depth
    // -iterate through each location (xsrc,ysrc) in the source image
    //         and increment histogram at truncated value of pixel
    //     -normalize the histogram H = H * 1/MN
    template< typename T > 
    void PDF(cv::Mat &histogram, const cv::Mat &source, int channel)
    {
        for( int i=0; i<source.rows; i++ )
            for( int j=0; j<source.cols; j++ )
            {
                histogram.at<double>(0,(int)source.at<uchar>(i,j,channel))++;
            }
        histogram.row(0) *= ( 1.0 / (source.rows * source.cols));
    }

    // -iterate through each location in the histogram 
    //         and compute sum of histogram sum(0,col)
    //         for each location in pixel range
    //         assert histogram[size] = 1
    template< typename T >
    void CDF(cv::Mat &histogram, int channel)
    {
        histogram.at<double>(1, 0, channel) = histogram.at<double>(0, 0, channel);
        for( int j=1; j<histogram.cols; j++ )
        {
            histogram.at<double>(1, j, channel) = histogram.at<double>(0, j, channel) + histogram.at<double>(1, j-1, channel);
        }
        assert( histogram.at<double>(1,255,0) - 1.0 < 1e-8 );
         
    }

    // -iterate through each location of specified histogram's CDF
    //         and set new histogram equal to closest pixel value floor(j) 
    template< typename T >
    void CreateLookup(cv::Mat& lookup, const cv::Mat &hsrc, const cv::Mat &hdest, int channel)
    {
        lookup = cv::Mat(1, hsrc.cols, CV_8UC1, cv::Scalar(0.0));
        int j = 0;

        for(int i =0; i < hsrc.cols; i++)
        {
            if( (long double)(hsrc.at<double>(1,i,channel) - hdest.at<double>(1,j,channel)) <= 0 )
                lookup.at<uchar>(0,i,channel) = j; 
            else
            {
                while( (long double)(hsrc.at<double>(1,i,channel) - hdest.at<double>(1,j,channel)) > 0 )
                    j++;
                if( (long double)(hsrc.at<double>(1,i,channel) - hdest.at<double>(1,j,channel)) > (long double)(hsrc.at<double>(1,i,channel) - hdest.at<double>(1,j-1,channel)) )
                    lookup.at<uchar>(0,i,channel) = j--;
                else 
                    lookup.at<uchar>(0,i,channel) = j;
            }
        }
    }

}

#endif
