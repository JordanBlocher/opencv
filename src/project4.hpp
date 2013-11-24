#include <iostream>
#include <sstream>

#include "Image.hpp"
#include "AffineTransform.hpp"
#include "Filter.hpp"
#include "Noise.hpp"
#include "FFT.hpp"

using namespace std;
using namespace cv;

template< class T >
int experiment1(Image<T>&, const char*);

template< class T >
int experiment2(Image<T>&, const char*);

template< class T >
int experiment3(Image<T>&, const char*);

inline double h(double a, double b, double t, int i, int j) 
{
    return (t / (M_PI*(i*a + j*b)))*sin(M_PI*(i*a + j*b))*exp(-j*M_PI*(i*a + j*b));
}

int project4(int argc, char* argv[])
{
    if( argc > 8 || argc < 2) 
    {
        cout <<" Usage: process_image <experiment> <num> (options)\n\n"
            "Options: 1. <1> Noise Removal (experiment 1)\n\n "
            "\t 2. <2> Edge Detection (experiment 2)\n"
            "\t 2. <3> Phase / Magnitude (experiment 3)\n";
        return -1;
    }
    
    if(atoi(argv[1]) == 1)
    {
        Image<uchar> boy("./bin/assets/boy_noisy.pgm", GRAY);   
        if(! boy.source.data )                              
        {
            cout <<  "Could not open or find ./bin/assets/boy_noisy.pgm" << std::endl ;
            return -1;
        }
 
        Image<Vec2b> image;   
        vector<cv::Mat> channels(2);
        cv::Mat zeros(boy.source.rows, boy.source.cols, CV_8UC1, cv::Scalar(0.0));

        channels[0] = boy.source;
        channels[1] = zeros.clone();
        cv::merge(channels, image.source);
        experiment1(image, string("boynoisy").c_str());
    }
    if(atoi(argv[1]) == 2 || atoi(argv[1]) == 3)
    {
        Image<uchar> lenna("./bin/assets/lenna.pgm", GRAY);   
        if(! lenna.source.data )                              
        {
            cout <<  "Could not open or find ./bin/assets/lenna.pgm" << std::endl ;
            return -1;
        }
 
        Image<Vec2b> image;   
        vector<cv::Mat> channels(2);
        cv::Mat zeros(lenna.source.rows, lenna.source.cols, CV_8UC1, cv::Scalar(0.0));

        channels[0] = lenna.source;
        channels[1] = zeros.clone();
        cv::merge(channels, image.source);
        if(atoi(argv[1]) == 2)
            experiment2(image, string("lenna").c_str());
        if(atoi(argv[1]) == 3)
            experiment3(image, string("lenna").c_str());

    }
   
    waitKey(0);
    return 0;
}


template< class T >
int experiment1(Image<T> &image, const char* outfile)
{
    ostringstream sout;
    cv::Point max;
    cv::Mat mag, logMag;
    vector<cv::Mat> channels(2);

    cv::Mat fft = FFT::FFT2D<T>(image.source, -1);
    logMag = Util::Magnitude<double>(fft, 200.0, true);

    cv::Mat roiQ1 = logMag(cv::Rect(0, 0, logMag.rows/2, logMag.cols/2));

    sout << "img/fft2/fftboy_noisy.png";
    cout << "Writing image to " << sout.str() << endl;
    cv::split(fft, channels);
    cv::Mat img = channels[0];
    Util::Normalize<double>(img, Scalar(255.0), 0);
    img.convertTo(img, CV_8UC1);
    imshow("FFT", img);
    imwrite(sout.str().c_str(), img);
    sout.str("");

    cv::minMaxLoc(roiQ1, NULL, NULL, NULL, &max);
    double x = fft.cols/2.0 - max.x;
    double y = fft.rows/2.0 - max.y;
    double dist = sqrt(x*x + y*y);

    Mat fftTemp = fft.clone();
    cv::Mat filter = Filter::Band(0, dist/2, fft.cols, fft.rows, true);
    fftTemp = fft.mul(filter);
    
    mag = Util::Magnitude<double>(fftTemp, 20.0, false);

    for ( int i = 0; i < 4; ++i )
    {
        cv::minMaxLoc(mag, NULL, NULL, NULL, &max);
        mag.at<double>(max) = 0.0;
        fft.at<Vec2d>(max) = Interpolate::Average<Vec2d>(fft, cv::Mat(max));
    }

    logMag = Util::Magnitude<double>(fft, 20.0, true);
    sout << "img/fft2/fftboy.png";
    cout << "Writing image to " << sout.str() << endl;
    cv::split(fft, channels);
    img = channels[0];
    img.convertTo(img, CV_8UC1);
    imwrite(sout.str().c_str(), img);
    imshow("FFT Clean", img);
    sout.str("");

    cv::Mat invfft = FFT::FFT2D<Vec2d>(fft, 1);

    cv::split(invfft, channels);
    img = channels[0];
    img.convertTo(img, CV_8UC1);
    imshow("InvFFT", img);
    sout << "img/fft2/boy.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), img);
    sout.str("");
 
    return 0;
}

template< class T >
int experiment2(Image<T> &image, const char* outfile)
{
    ostringstream sout;
    cv::Point max;
    cv::Mat mag, logMag, sobel;
    vector<cv::Mat> channels(2), sobelchannels(2);
    cv::Mat tmp = Filter::Sobel();
    tmp = tmp.t();
    cv::Mat zeros(tmp.rows, tmp.cols, CV_64FC1, cv::Scalar(0.0));
    channels[0] = tmp.clone();
    channels[1] = zeros.clone();
    cv::merge(channels, sobel);
    cv::Mat srcPadded, sobelPadded;

    Util::PadImage<T>(srcPadded, image.source, 256, 256);
    Util::PadImage<Vec2d>(sobelPadded, sobel, 509, 509);

    cv::Mat fft = FFT::FFT2D<T>(srcPadded, -1);
    cv::Mat sobelfft = FFT::FFT2D<Vec2d>(sobelPadded, -1);
    logMag = Util::Magnitude<double>(fft, 20.0, true);
    imshow("FFT Lenna", logMag);
    logMag = Util::Magnitude<double>(sobelfft, 20.0, true);
    imshow("FFT Sobel", logMag);
    
    Util::Shift<Vec2d>(sobelfft);
    cv::split(sobelfft, sobelchannels);
    cv::split(fft, channels);
    cv::Mat re = channels[0].mul(sobelchannels[0]);
    re = re - channels[1].mul(sobelchannels[1]);
    cv::Mat im = channels[0].mul(sobelchannels[1]);
    im = im + channels[1].mul(sobelchannels[0]);
    channels[0] = re.clone();
    channels[1] = im.clone();
    cv::merge(channels, fft);
    logMag = Util::Magnitude<Vec2d>(fft, 20.0, true);
    cv::split(logMag, channels);
    imshow("test", channels[0]);
    
    sout << "img/fft2/lennaedge.png";
    cout << "Writing image to " << sout.str() << endl;

    cv::Mat invfft = FFT::FFT2D<Vec2d>(fft, 1);

    cv::split(invfft, channels);
    cv::Mat img = channels[0](cv::Rect(128, 128, 256, 256));
    Util::Normalize<double>(img, cv::Scalar(255.0), 0);
    img.convertTo(img, CV_8UC1);
    imshow("InvFFT", img);

    imwrite(sout.str().c_str(), logMag);
    sout.str("");
 
    return 0;
}


template< class T >
int experiment3(Image<T> &image, const char* outfile)
{
    ostringstream sout;
    cv::Mat mag, logMag;
    vector<cv::Mat> channels(2);

    cv::Mat fft = FFT::FFT2D<T>(image.source, -1);

    Noise::Create<Vec2d>(fft, h, 0.1, 0.1, 1);
   
    sout << "img/fft2/lennamotion.png";
    cout << "Writing image to " << sout.str() << endl;

    cv::Mat invfft = FFT::FFT2D<Vec2d>(fft, 1);

    cv::split(invfft, channels);
    cv::Mat img = channels[0];
    Util::Normalize<double>(img, cv::Scalar(255.0), 0);
    img.convertTo(img, CV_8UC1);
    imshow("Lenna MotionBlur", img);

    imwrite(sout.str().c_str(), logMag);
    sout.str("");
 
    return 0;
}





