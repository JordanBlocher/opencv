#ifndef IMAGE_H
#define IMAGE_H

#include "IntensityTransform.hpp"
#include "AffineTransform.hpp"
#include "Interpolate.hpp"
#include "Util.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <assert.h>


// CV_<bit-depth>{U|S|F}C(<number_of_channels>)

enum MatType {GRAY = 0, COLOR = 1};
enum HType {HPDF = 0, HCDF = 1, ERROR = 2};
enum FFTPlot {RE = 0, IM = 1, MAG = 2, PHZ = 3};


// Image class
// -holds data for image operations
template< typename T >
class Image {
 public:
    Image(const char*, MatType);
    Image(const Image<T>&);
    Image(const cv::Mat& m = cv::Mat());
    Image(const cv::Mat&, int width, int height = 400, int size = 256, enum HType = HPDF);
    Image(const cv::Mat&, int width, int height = 400, int size = 256, enum FFTPlot = MAG);
    ~Image();

    Image<T>& operator=(const Image<T>&);

    cv::Mat source;
    cv::Mat histogram;
    cv::Mat squareError;

};

template< typename T >
Image<T>::Image(const char* filename, MatType type)
{
    this->source = cv::imread(filename, type);
}

template< typename T >
Image<T>::Image(const Image& image)
{
    this->source = image.source.clone();
    this->histogram = image.histogram.clone();
}

template<typename T >
Image<T>::Image(const cv::Mat &src)
{
    this->source = src.clone();
}

// -create image of histogram
template<typename T >
Image<T>::Image(const cv::Mat &hist, int width, int height, int size, HType type)
{
    this->histogram = hist.clone();
    this->source = cv::Mat(height, width, this->source.type(), cv::Scalar(0.0));

    double min, max;
    cv::minMaxLoc(histogram.row(type == HPDF || type == ERROR ? 0 : 1), &min, &max);
    
    double scale = 1.0/max;
    double w = (double) (width-10)/size;
    
    for(int i=0; i<size; i++ )
    {
        if ( type == HPDF )
        {
            cv::line( this->source,
                cv::Point( floor(5+w*(i)), height ) ,
                cv::Point( floor(5+w*(i)), height - round( scale * height * hist.at<double>(type, i) )),
                cv::Scalar( 255 ), 2, 8, 0 );
        }
        else if( type == ERROR )
        {
                cv::line( this->source,
                cv::Point( floor(5+w*(i-1)), height - 0.1 * round(10000 * hist.at<double>(0,i-1) )) ,
                cv::Point( floor(5+w*(i)), height - 0.1 * round(10000 * hist.at<double>(0, i) )),
                cv::Scalar( 255 ), 2, 8, 0  );

        }
        else if (i != 0)
        {
            cv::line( this->source,
                cv::Point( floor(5+w*(i-1)), height - round( scale * height * hist.at<double>(type,i-1) )) ,
                cv::Point( floor(5+w*(i)), height - round( scale * height * hist.at<double>(type, i) )),
                cv::Scalar( 255 ), 2, 8, 0  );
        }
    }
}

// -plot fft data
template<typename T >
Image<T>::Image(const cv::Mat &fft, int width, int height, int size, FFTPlot type)
{
    this->source = cv::Mat(height, width, CV_8U, cv::Scalar(0.0));
    double minVal, maxVal;

    std::vector<cv::Mat> channels(2);

    cv::split(fft, channels);
    cv::Mat mag;
    cv::Mat phz;

    if ( type == RE )
        cv::minMaxLoc(channels[0], &minVal, &maxVal);
    else if ( type == IM )
        cv::minMaxLoc(channels[1], &minVal, &maxVal);
    else if ( type == MAG )
    {
        cv::Mat reSqr;
        cv::Mat imSqr;
        cv::multiply(channels[0], channels[0], reSqr);
        cv::multiply(channels[1], channels[1], imSqr);
        cv::sqrt(reSqr + imSqr, mag);
        cv::minMaxLoc(mag, &minVal, &maxVal);
    }
    else if ( type == PHZ )
    {
        phz = cv::Mat(channels[0].rows, channels[0].cols, CV_64FC1);
        for ( int i = 0; i < channels[0].rows; ++i )
            for ( int j = 0; j < channels[1].cols; ++j )
                phz.at<double>(i,j) = atan2(channels[1].at<double>(i,j), channels[0].at<double>(i,j));
        cv::minMaxLoc(phz, &minVal, &maxVal);
    }
    else
    {
        minVal = 0.0;
        maxVal = 255.0;
    }
   
    std::cout << "Min : " << minVal << "   Max : " << maxVal << std::endl;

    minVal = std::min(0.0,minVal);

    double scale = height/(maxVal-minVal);
    double w = (double) (width-10)/size;

    cv::Mat img;
    if( type == RE )
        img = channels[0];
    else if( type == IM )
        img = channels[1];
    else if ( type == MAG )
        img = mag;
    else if ( type == PHZ )
        img = phz;
    else
        img = cv::Mat(fft.rows, fft.cols, CV_64FC1, cv::Scalar(0.0));

    // draw horizontal axis
    cv::line(this->source,
             cv::Point(0,height - scale * fabs(minVal)),
             cv::Point(width, height - scale * fabs(minVal)),
             cv::Scalar( 255 ), 1, 8, 0);

    // draw bars
    for(int i=0; i<size; i++ )
    {
        cv::line( this->source,
            cv::Point( floor(w/2.0+w*(i)), height - scale * fabs(minVal) ),
            cv::Point( floor(w/2.0+w*(i)), height - round( scale * (img.at<double>(i, 0) + fabs(minVal)) )),
            cv::Scalar( 255 ), 2, 8, 0 );
    }
}

template< typename T>
Image<T>& Image<T>::operator=(const Image<T> &image)
{
    if( !this == image)
    {
        this->source = image.clone();
    }
    return *this;
}

template< typename T >
Image<T>::~Image()
{
}

#endif
