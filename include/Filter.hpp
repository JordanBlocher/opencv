#ifndef FILTER_H
#define FILTER_H

#include "Interpolate.hpp"
#include "Util.hpp"

#include <opencv2/opencv.hpp>
#include <cmath>

enum MasqueType {GAUSSIAN15 = 15, GAUSSIAN7 = 7, SOBEL = 0, PREWITT = 1, LAPLACIAN = 2};

namespace Filter
{
    template< typename T >
    cv::Mat Correlation(cv::Mat& source, const cv::Mat& filter, bool normalize, bool apply)
    {
        cv::Mat padded;
        cv::Mat dest(source.size(), CV_64F);
        Util::PadImage<T>(padded, source, filter.cols, filter.rows);
        for(int i=filter.rows/2; i<source.rows + filter.rows/2; i++)
            for(int j=filter.cols/2; j<source.cols + filter.cols/2; j++)
            {

                cv::Mat roi = padded(cv::Rect(j - filter.cols/2, i - filter.rows/2, filter.cols, filter.rows)); 
                cv::Mat out;
                cv::multiply(roi, filter, out, 1, CV_64F);
                cv::Scalar sum = cv::sum(out);
                for(int k=0; k<source.channels(); k++)
                {
                    dest.at<double>(i - filter.rows/2, j - filter.cols/2, k) = sum[k];
                }
            }

        cv::Mat ret = dest.clone();
        if(normalize)
        {
            Util::Normalize<double>(dest, cv::Scalar(255.0), 0);
        }
        if(apply)
        {
            dest.convertTo(source, source.type(), 1.0);
        }

        return ret;
    }

    template< typename T >
    cv::Mat Median(cv::Mat& source, int filterSize, bool apply)
    {
        cv::Mat padded;
        cv::Mat dest(source.size(), CV_8U);
        Util::PadImage<T>(padded, source, filterSize, filterSize);
        for(int i=filterSize/2; i<source.rows + filterSize/2; i++)
            for(int j=filterSize/2; j<source.cols + filterSize/2; j++)
            {
                cv::Mat roi = padded(cv::Rect(j - filterSize/2, i - filterSize/2, filterSize, filterSize)).clone();
                std::nth_element(roi.data, roi.data+(filterSize*filterSize)/2, roi.dataend); //only median is sorted
                dest.at<T>(i-filterSize/2, j-filterSize/2) = roi.at<uchar>(filterSize/2,filterSize/2);
            }

        if(apply)
        {
            dest.convertTo(source, source.type(), 1.0);
        }
        return dest;
    }


    inline cv::Mat Gaussian7()
    {
        return (cv::Mat_<double>(7, 7) << 1, 1, 2, 2, 2, 1, 1,
                                        1, 2, 2, 4, 2, 2, 1,
                                        2, 2, 4, 8, 4, 2, 2,
                                        2, 4, 8,16, 8, 4, 2,
                                        2, 2, 4, 8, 4, 2, 2,
                                        1, 2, 2, 4, 2, 2, 1,
                                        1, 1, 2, 2, 2, 1, 1);
    }

    inline cv::Mat Gaussian15()
    {
        return (cv::Mat_<double>(15, 15) << 2 ,2, 3 ,4 ,5 ,5, 6, 6, 6, 5, 5, 4, 3, 2, 2,
                                            2, 3, 4, 5, 7, 7, 8, 8, 8, 7, 7, 5, 4, 3, 2,
                                            3, 4, 6, 7, 9,10,10,11,10,10, 9, 7, 6, 4, 3,
                                            4, 5, 7, 9,10,12,13,13,13,12,10, 9, 7, 5, 4,
                                            5, 7, 9,11,13,14,15,16,15,14,13,11, 9, 7, 5,
                                            5, 7,10,12,14,16,17,18,17,16,14,12,10, 7, 5,
                                            6, 8,10,13,15,17,19,19,19,17,15,13,10, 8, 6,
                                            6, 8,11,13,16,18,19,20,19,18,16,13,11, 8, 6,
                                            6, 8,10,13,15,17,19,19,19,17,15,13,10, 8, 6,
                                            5, 7,10,12,14,16,17,18,17,16,14,12,10, 7, 5,
                                            5, 7, 9,11,13,14,15,16,15,14,13,11, 9, 7, 5,
                                            4, 5, 7, 9,10,12,13,13,13,12,10, 9, 7, 5, 4,
                                            3, 4, 6, 7, 9,10,10,11,10,10, 9, 7, 6, 4, 3,
                                            2, 3, 4, 5, 7, 7, 8, 8, 8, 7, 7, 5, 4, 3, 2,
                                            2 ,2, 3 ,4 ,5 ,5, 6, 6, 6, 5, 5, 4, 3, 2, 2);
    }

    inline cv::Mat Prewitt()
    {
        return (cv::Mat_<double>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
    }
    
    inline cv::Mat Sobel()
    {
        return (cv::Mat_<double>(3,3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
    }

    inline cv::Mat Laplacian()
    {
        return (cv::Mat_<double>(3,3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);
    }
}

#endif
