#ifndef FFT_H
#define FFT_H

#include "Util.hpp"

#include <opencv2/opencv.hpp>
#include <math.h>
#include <iomanip>

#define c 5.0
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

/* (C) Copr. 1986-92 Numerical Recipes Software 0#Y". */
namespace FFT
{

    void FFT1D(double data[], unsigned long nn, int isign)
    {
        unsigned long n,mmax,m,j,istep,i;
        double wtemp,wr,wpr,wpi,wi,theta;
        double tempr,tempi;

        n=nn << 1;
        j=1;
        for (i=1;i<n;i+=2) {
            if (j > i) {
                SWAP(data[j],data[i]);
                SWAP(data[j+1],data[i+1]);
            }
            m=n  >> 1;
            while (m >= 2 && j > m) {
                j -= m;
                m >>= 1;
            }
            j += m;
        }
        mmax=2;
        while (n > mmax) {
            istep=mmax << 1;
            theta=isign*(6.28318530717959/mmax);
            wtemp=sin(0.5*theta);
            wpr = -2.0*wtemp*wtemp;
            wpi=sin(theta);
            wr=1.0;
            wi=0.0;
            for (m=1;m<mmax;m+=2) {
                for (i=m;i<=n;i+=istep) {
                    j=i+mmax;
                    tempr=wr*data[j]-wi*data[j+1];
                    tempi=wr*data[j+1]+wi*data[j];
                    data[j]=data[i]-tempr;
                    data[j+1]=data[i+1]-tempi;
                    data[i] += tempr;
                    data[i+1] += tempi;
                }
                wr=(wtemp=wr)*wpr-wi*wpi+wr;
                wi=wi*wpr+wtemp*wpi+wi;
            }
            mmax=istep;
        }
    }


    template< typename T>
    cv::Mat FFT2D(cv::Mat source, int isign, bool shift = true)
    {
        // Require power of 2
        int height = std::pow(2, std::ceil(log(source.rows)/log(2)));
        int width = std::pow(2, std::ceil(log(source.cols)/log(2)));
        std::cout << "Rows : " << source.rows << std::endl;
        std::cout << "Cols  : " << source.cols << std::endl;
        std::cout << "Height : " << height << std::endl;
        std::cout << "Width  : " << width << std::endl;
        std::cout << "Channels : " << source.channels() << std::endl;
        cv::Mat dest(height, width, CV_64FC2);
        if ( height != source.rows || width != source.cols )
            Util::PadImage<T>(dest, source, width, height);
        
        for ( int i = 0; i < source.rows; ++i )
            for ( int j = 0; j < source.cols; ++j )
                dest.at<Vec2d>(i,j) = Vec2d(static_cast<double>(source.at<T>(i,j)[0]), static_cast<double>(source.at<T>(i,j)[1]));

        //std::cout<<"Performing FFT on: "<<dest<<std::endl<<std::endl;
       
        if ( isign < 0  && shift)
        {
            for ( int i =0; i < dest.rows; ++i )
                for ( int j = 0; j < dest.cols; ++j )
                    if ( (i+j)%2 != 0 )
                    {
                        dest.at<Vec2d>(i,j)[0] *= -1;
                        dest.at<Vec2d>(i,j)[1] *= -1;
                    }
        }
        
        // Construct arrays of image rows
        double* data = new double[std::max(width,height)*2];
        for ( int i = 0; i < dest.rows; i++ )
        {
            for ( int j = 0; j < dest.cols; j++ )
            {
                data[2*j] = dest.at<Vec2d>(i,j)[0];
                data[2*j+1] =dest.at<Vec2d>(i,j)[1];
            }

            // 1D FFT
            FFT::FFT1D( data - 1, dest.cols, isign );

            for ( int k = 0; k < dest.cols; k++ )
            {
                dest.at<Vec2d>(i,k) = Vec2d(data[2*k], data[2*k + 1]);
            }
        }    
        if ( isign < 0 )
            dest = dest * 1.0/(dest.rows*dest.cols);
 
        // Construct arrays of image cols
        for ( int j = 0; j < dest.cols; j++ )
        {
            for ( int i = 0; i < dest.rows; i++ )
            {
                data[2*i] = dest.at<Vec2d>(i, j)[0];

                data[2*i+1] =dest.at<Vec2d>(i, j)[1];
            }

            // 1D FFT
            FFT::FFT1D( data - 1, dest.rows, isign );

            for ( int k = 0; k < dest.rows; k++ )
            {
                dest.at<Vec2d>(k, j) = Vec2d(data[2*k], data[2*k + 1]);
            }
            
        }

    
        if ( isign > 0 && shift)
        {
            for ( int i =0; i < dest.rows; ++i )
                for ( int j = 0; j < dest.cols; ++j )
                    if ( (i+j)%2 != 0 )
                    {
                        dest.at<Vec2d>(i,j)[0] *= -1;
                        dest.at<Vec2d>(i,j)[1] *= -1;
                    }
        }

        delete [] data;
        return dest;
    }
      
  }

#undef SWAP

#endif
