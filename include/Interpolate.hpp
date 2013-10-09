#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <opencv2/opencv.hpp>
#include <math.h>

namespace Interpolate
{
    template< typename T>
    T Bilinear(cv::Mat &source, const cv::Mat &v)
    {
        double x = v.at<double>(0,0);
        double y = v.at<double>(1,0); 
        int x0 = floor( x ); 
        int x1 = ceil( x );
        int y0 = floor( y );
        int y1 = ceil( y );

        if ( x0 == x1 ) x1++;
        if ( y0 == y1 ) y1++;

        T v00 = (x0 < 0 || x0 >= source.cols || y0 < 0 || y0 >= source.rows) ? 0 : source.at<T>( cv::Point(x0, y0) );
        T v01 = (x0 < 0 || x0 >= source.cols || y1 < 0 || y1 >= source.rows) ? 0 : source.at<T>( cv::Point(x0, y1) );
        T v10 = (x1 < 0 || x1 >= source.cols || y0 < 0 || y0 >= source.rows) ? 0 : source.at<T>( cv::Point(x1, y0) );
        T v11 = (x1 < 0 || x1 >= source.cols || y1 < 0 || y1 >= source.rows) ? 0 : source.at<T>( cv::Point(x1, y1) );

        double p00 = v00*( x1 - v.at<double>(0,0) )*( y1 - v.at<double>(1,0) );
        double p01 = v01*( x1 - v.at<double>(0,0) )*( v.at<double>(1,0) - y0 );
        double p10 = v10*( v.at<double>(0,0) - x0 )*( y1 - v.at<double>(1,0) );
        double p11 = v11*( v.at<double>(0,0) - x0 )*( v.at<double>(1,0) - y0 );
        
        return ( p00 + p01 + p10 + p11 ) / (( x1 - x0 )*( y1 - y0 ));
    }

    template< typename T >
    T Average(cv::Mat &source, const cv::Mat &v)
    {
        double x = v.at<double>(0,0);
        double y = v.at<double>(1,0); 
        int x0 = floor( x ); 
        int x1 = ceil( x );
        int y0 = floor( y );
        int y1 = ceil( y );

        int v00 = (x0 < 0 || x0 >= source.cols || y0 < 0 || y0 >= source.rows) ? 0 : source.at<T>( cv::Point(x0, y0) );
        int v01 = (x0 < 0 || x0 >= source.cols || y1 < 0 || y1 >= source.rows) ? 0 : source.at<T>( cv::Point(x0, y1) );
        int v10 = (x1 < 0 || x1 >= source.cols || y0 < 0 || y0 >= source.rows) ? 0 : source.at<T>( cv::Point(x1, y0) );
        int v11 = (x1 < 0 || x1 >= source.cols || y1 < 0 || y1 >= source.rows) ? 0 : source.at<T>( cv::Point(x1, y1) );

        return (uchar)((v00 + v01 + v10 + v11) /4.0);
    }

    template< typename T >
    T NearestNeighbor(cv::Mat &source, const cv::Mat &v)
    {
        double x = v.at<double>(0,0);
        double y = v.at<double>(1,0); 
        int ny = round( y ); 
        int nx = round( x );

        nx = nx < 0 ? 0 : (nx >= source.cols ? source.cols - 1 : nx);
        ny = ny < 0 ? 0 : (ny >= source.rows ? source.rows - 1 : ny);
        return source.at<T>( cv::Point(nx, ny) );
    }

}

#endif
