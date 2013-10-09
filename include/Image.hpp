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

template< typename T >
class Image {
 public:
    Image(const char*, MatType);
    Image(const Image<T>&);
    Image(const cv::Mat& m = cv::Mat());
    Image(const cv::Mat&, int width, int height = 400, int size = 256, enum HType = HPDF);
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
