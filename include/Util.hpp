#ifndef UTIL_H
#define UTIL_H

#include "Image.hpp"

#include <opencv2/opencv.hpp>
#include <cmath>
#include <assert.h>
#include <fstream>

#define coeff 2.0

namespace Util
{
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

    template< typename T>
    void Shift(cv::Mat& source)
    {
        for ( int i =0; i < source.rows; ++i )
            for ( int j = 0; j < source.cols; ++j )
                if ( (i+j)%2 != 0 )
                {
                    source.at<T>(i,j)[0] *= -1;
                    source.at<T>(i,j)[1] *= -1;
                }
    }

    template< typename T>
    cv::Mat Magnitude(const cv::Mat& source, double c, bool log) 
    {
        cv::vector<cv::Mat> channels(2);
        cv::Mat mag, reSqr, imSqr;
        cv::Mat logMag;

        cv::split(source, channels);
        cv::multiply(channels[0], channels[0], reSqr);
        cv::multiply(channels[1], channels[1], imSqr);
        cv::sqrt(reSqr + imSqr, mag);
       
        if(log)
        {
            cv::log(c * cv::abs(mag + 1), logMag);
            Util::Normalize<double>(logMag, cv::Scalar(1.0), 0);
            return logMag;
        }
        else
            return mag;
    }

    template< typename T>
    void PadImage(cv::Mat& padded, const cv::Mat &source, int padX, int padY)
    {
        padded = cv::Mat( source.rows + padY, source.cols + padX, source.type(), cv::Scalar(0.0, 0.0, 0.0));
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

    cv::Mat CreateImage(const char* filename, std::vector<uchar> v)
    {
        double in;
        std::ifstream fin(filename);
        if(fin)
        {
            while( !fin.eof() )
            {
                fin >> in;
                v.push_back(static_cast<uchar>(in));
                v.push_back(static_cast<uchar>(0));
            }
            fin.close();
        }
        
        cv::Mat dest(( v.size() - 1)/2,1,CV_8UC2);
        for ( int i = 0; i < dest.rows; ++i )
            dest.at<cv::Vec2b>(i,0) = cv::Vec2b(v[2*i], v[2*i+1]);
        return dest;
    }

    cv::Mat CreateImage(std::vector<uchar> v)
    {
        cv::Mat dest(v.size()/2,1,CV_8UC2);
        for ( int i = 0; i < dest.rows; ++i )
            dest.at<cv::Vec2b>(i,0) = cv::Vec2b(v[2*i], v[2*i+1]);
        return dest;
    }

    cv::Mat CreateImage( double (*f)(double), int N ,int u, std::vector<double> v)
    {
        for(int i=0; i<N; i++)
        {
            v.push_back((*f)(M_PI*i*u/N));
            v.push_back(0.0);
        }
        cv::Mat dest(v.size()/2,1,CV_64FC2);
        for ( int i = 0; i < dest.rows; ++i )
            dest.at<cv::Vec2b>(i,0) = cv::Vec2b(v[2*i], v[2*i+1]);
        return dest;
    }

    cv::Mat CreateImage(int b, int w)
    {
        cv::Mat black(b, b, CV_8UC2, cv::Scalar(0.0, 0.0));
        cv::Mat white(w, w, CV_8UC2, cv::Scalar(255.0, 0.0));
        cv::Mat roi = black(cv::Rect(black.cols/2 - white.cols/2, black.rows/2 - white.rows/2, white.cols, white.rows));
        white.copyTo(roi);
        return black;
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
