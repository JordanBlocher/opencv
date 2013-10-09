#ifndef INTENSITYTRANSFORM_H
#define INTENSITYTRANSFORM_H

#include "Util.hpp"

#include <opencv2/opencv.hpp>
#include <cmath>

namespace IntensityTransform
{
    // -find binary number for next power of 2 above level (desired bit restriction 2^level)
    //      and subtract 1 to get mask
    //      -AND all values in image to restrict possible values to 2^level
    template< typename T>
    void Quantize(cv::Mat &source, int level, int channel)
    {
        uchar filter = pow(2, 8 - level) - 1;
        filter ^= 0xff;

        for(int i=0; i<source.rows; i++)
            for(int j=0; j<source.cols; j++)
                source.at<uchar>(i,j,channel) &= filter;
    }

    template< typename T >
    void CreateHistogram(cv::Mat &histogram, const cv::Mat& source, int channel)
    {
        Util::PDF<T>(histogram, source, channel);
        Util::CDF<T>(histogram, channel);
    }

    // -get histograms for two images passed (as member variable and parameter)
    //       -create lookup table for new values [0, size]
    //       -replace each value in the dest image with the corresponding value in
    //       the lookup table
    //       -return new histogram
    template< typename T >
    void SpecifyHistogram(cv::Mat &source, const cv::Mat &hdest, int size, int channel)
    {
        cv::Mat histogram = cv::Mat(2, size, CV_64F, cv::Scalar(0.0));
        CreateHistogram<T>(histogram, source, channel);

        cv::Mat lookup;
        Util::CreateLookup<T>(lookup, histogram, hdest, channel);

         for( int i=0; i<source.rows; i++ )
            for( int j=0; j<source.cols; j++ )
                source.at<uchar>(i, j, channel) = (uchar)lookup.at<uchar>(0, source.at<uchar>(i, j, channel));
     
    }

    // -create histogram
    //      and replace each value in the image with the value referenced 
    //      in the CDF [0,1] and unnormalize (val*255)
    template< typename T >
    void Equalize(cv::Mat &source, int size, int channel)
    {
        cv::Mat histogram = cv::Mat(2, size, CV_64F, cv::Scalar(0.0));
        CreateHistogram<T>(histogram, source, channel);

        for( int i=0; i<source.rows; i++ )
            for( int j=0; j<source.cols; j++ )
                source.at<uchar>(i, j, channel) = (uchar)(histogram.at<double>(1, source.at<uchar>(i, j, channel)) *255);
    }
}


#endif
