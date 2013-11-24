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
                if((rand() % 100) < percentage)
                    source.at<T>(i, j) = (uchar)(rand() % 2) * 255;
            }
    }

    template< typename T>
    void Create(cv::Mat& source, double(*h)(double, double, double, int, int), double a, double b, double t)
    {
        for(int i=0; i<source.rows; i++)
            for(int j=0; j<source.cols; j++)
                source.at<T>(i, j) *= (*h)(a, b, t, i, j);
    }

    extern double ranf();         /* ranf() is uniform in 0..1 */
    //(c) Copyright 1994, Everett F. Carter Jr.
    double BoxMuller(double m, double s)	/* normal random variate generator */
    {				        /* mean m, standard deviation s */
	    double x1, x2, w, y1;
	    static double y2;
	    static int use_last = 0;

	    if (use_last)		        /* use value from previous call */
	    {
		    y1 = y2;
		    use_last = 0;
	    }
	    else
	    {
		    do {
			    x1 = 2.0 * ranf() - 1.0;
			    x2 = 2.0 * ranf() - 1.0;
			    w = x1 * x1 + x2 * x2;
		    } while ( w >= 1.0 );

		    w = sqrt( (-2.0 * log( w ) ) / w );
		    y1 = x1 * w;
		    y2 = x2 * w;
		    use_last = 1;
	    }

	    return( m + y1 * s );
    }


    template< typename T>
    void Gaussian(cv::Mat& source, double mu, double sigma)
    {
        for(int i=0; i<source.rows; i++)
            for(int j=0; j<source.cols; j++)
                source.at<T>(i, j) += mu + sigma*Noise::BoxMuller(mu, sigma);
    }
    



}

#endif
