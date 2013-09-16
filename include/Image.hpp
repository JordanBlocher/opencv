#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

#include "Transform.hpp"
#include "Interpolate.hpp"

// CV_<bit-depth>{U|S|F}C(<number_of_channels>)

#define GRAYSCALE uchar
#define RGB cv::Vec<uchar, 3>

enum MatType {GRAY = 0, COLOR = 1};

template< class T >
class Image {
 public:
    Image(const char*, MatType);
    Image(const Image<T>&);
    Image(const cv::Mat&);
    Image(const cv::Mat_<double>&, int, int, int);
    ~Image();

    void AffineTransform(const cv::Mat&);
    cv::Mat_<double>& CreateNormalizedHistogram(int, int, const cv::Vec<int, 2>&);
    cv::Mat_<double>& CreateHistogram(int, int, const cv::Vec<int, 2>&);
    void Equalize(const cv::Mat_<double>&);
    
    Image<T>& operator=(const Image<T>&);

    bool isPoint(const cv::Mat&);
    bool isEdge(const cv::Mat&);

    cv::Mat source;
    cv::Mat_<double> histogram;

};

template< class T >
Image<T>::Image(const char* filename, MatType type)
{
    this->source = cv::imread(filename, type);
}

template< class T >
Image<T>::Image(const Image& image)
{
    this->source = image.source.clone();
}

template<class T >
Image<T>::Image(const cv::Mat &src)
{
    this->source = src.clone();
}

template<class T >
Image<T>::Image(const cv::Mat_<double> &hist, int width = 400, int height = 400, int size = 255)
{
    this->histogram = hist.clone();
    this->source = cv::Mat(height, width, this->source.type(), cv::Scalar(0.0));
    
    width = round( (double) width/size );
    
    for(int i=1; i<size; i++ )
    {
        std::cout<<"pt y "<<hist.at<double>(i-1)<<std::endl;
        cv::line( this->source, cv::Point( width*(i-1), round((double)height - hist.at<double>(i-1)) ) ,
        cv::Point( width*(i), round((double)height - hist.at<double>(i)) ),
        cv::Scalar( 255 ), 2, 8, 0  );
    }
}

template< class T>
Image<T>& Image<T>::operator=(const Image<T> &image)
{
    if( !this = image)
    {
        this->source = image.clone();
    }
    return *this;
}

template< class T >
Image<T>::~Image()
{
}

template< class T >
inline bool Image<T>::isPoint(const cv::Mat &v)
{ 
    double v0 = v.at<double>(0,0);
    double v1 = v.at<double>(0,1);
    return (( (floor(v0) - ceil(v0) == 0) ) && ( (floor(v1) - ceil(v1) == 0) )); 
}

// -Compute inverse transform Ti = inverse(T)
// -iterate through each location (xdest,ydest) in the destination image
//         and create a column vector (cv::Mat) D = [xdest;ydest;1]
//     -compute source location vector which is used to determine where
//             to sample from in the source image S = Ti * D
//     -normalize the souce vector S = S * 1/S(3)
//     -sample src image at location xsrc = S(1), ysrc = S(2) and put
//             value in dest location

template< class T >
void Image<T>::AffineTransform(const cv::Mat &transform)
{
    cv::Mat dest(this->source.rows, this->source.cols, this->source.type());

    for( int i=0; i<dest.rows; i++ )
        for( int j=0; j<dest.cols; j++ )
        {
            cv::Vec<double, 3> v(j, i, 1.0);
            cv::Mat tv(v);
            std::cout<<(int)source.at<T>(i,j)<<std::endl;

            tv = transform.inv() * tv;   
            tv = tv * (1.0 / tv.at<double>(2,0));                        
            if( !isPoint(tv) )
            {
                dest.at<T>(i, j) = Interpolate::Bilinear<T>(this->source, tv);
            }
            else
            {
                dest.at<T>(i, j) = source.at<T>((int)tv.at<double>(1, 0), (int)tv.at<double>(0,0));
            }
        }
    
    this->source = dest.clone();
}

template< class T >
cv::Mat_<double>& Image<T>::CreateHistogram(int channel, int size, const cv::Vec<int, 2> &range)
{
    this->histogram = cv::Mat_<double>(1, size, this->source.type());

    for( int i=0; i<this->source.rows; i++ )
        for( int j=0; j<this->source.cols; j++ )
        {
            this->histogram(0,(int)source.at<T>(i,j))++;
        }
    //std::cout<<this->histogram<<std::endl;
    this->histogram.row(0) *= ( 1.0 / (this->source.rows * this->source.cols));

   
    return this->histogram;
}

template< class T >
void Image<T>::Equalize(const cv::Mat_<double> &hist)
{
    cv::Scalar s = 0;
    for( int i=0; i<this->source.rows; i++ )
        for( int j=0; j<this->source.cols; j++ )
        {
            cv::Mat m(hist);
            cv::Mat range = m(cv::Range::all(), cv::Range(0, (int)round(source.at<T>(i, j))) );
            s = cv::sum(range);
            source.at<T>(i, j) = (T)(s[0] * 255);
        }
     
}


template< class T >
cv::Mat_<double>& Image<T>::CreateNormalizedHistogram(int channel, int size, const cv::Vec<int, 2> &range)
{
    double min, max;

    this->histogram = cv::Mat_<double>(1, size, this->source.type());

    for( int i=0; i<this->source.rows; i++ )
        for( int j=0; j<this->source.cols; j++ )
        {
            this->histogram(0,(int)source.at<T>(i,j))++;
        }

    cv::Mat m(this->histogram);
    cv::minMaxLoc(m, &min, &max);
    std::cout<<"MINMAX "<<min<< " "<<max<<std::endl<<std::endl;
    this->histogram.row(0) *= ( size / (max - min) );
    //this->histogram.row(0) *= ( 1.0 / (this->source.rows * this->source.cols));

    std::cout<<this->histogram<<std::endl;
    return this->histogram;
}




#endif



