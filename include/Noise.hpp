#ifndef NOISE_H
#define NOISE_H

#include <opencv2/opencv.hpp>
#include <cmath>

namespace Noise
{
    template< typename T >
    void SaltandPepper(cv::Mat& source, int percentage)
    {
        if( percentage == 0 )
            return;

        for(int i=0; i<source.rows; i++)
            for(int j=0; j<source.cols; j++)
            {
                if(rand() % 100 < percentage)
                    for(int k=0; k<source.channels(); k++)
                        source.at<T>(i, j, k) = (uchar)(rand() % 2) * 255;
            }
    }

}

#endif
