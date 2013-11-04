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
int experiment3(Image<T>&);

inline double f(double x) { return cos(x) + sin(x); };


int project3(int argc, char* argv[])
{
    if( argc > 8 || argc < 2) 
    {
        cout <<" Usage: process_image <experiment> <num> (options)\n\n"
            "Options: 1. <1> <a|b|c|d|e> (rect.dat) to test FFT1D (experiment 1)\n\n "
            "\t 2. <2> <a|b|c> to test FFT2D (experiment 2)\n"
            "\t 2. <3> Phase / Magnitude (experiment 3)\n";
        return -1;
    }

    if(atoi(argv[1]) == 1)
    {
        if(string(argv[2]) == "a")
        {
            Image<Vec2b> image;
            vector<uchar> v = {(uchar)1, (uchar)0, (uchar)2, (uchar)0, (uchar)4, (uchar)0, (uchar)4, (uchar)0};
            image.source = Util::CreateImage(v).clone();
            cout<<"SEQUENCE\n"<<image.source<<endl;
            experiment1(image, string("1D").c_str());
        }
        else if(string(argv[2]) == "b")
        {
            Image<Vec2d> image;
            vector<double> v;
            image.source = Util::CreateImage(cos, 128, 8, v).clone();
            experiment1(image, string("1DCosine").c_str());
        }
        else if(string(argv[2]) == "c")
        {
            Image<Vec2d> image;
            vector<double> v;
            image.source = Util::CreateImage(sin, 128, 8, v).clone();
            std::cout<< image.source<<endl;
            experiment1(image, string("1DSine").c_str());
        }
        else if(string(argv[2]) == "d")
        {
            Image<Vec2d> image;
            vector<double> v;
            image.source = Util::CreateImage(f, 128, 8, v).clone();
            std::cout<< image.source<<endl;
            experiment1(image, string("1DSum").c_str());
        }
        else if(string(argv[2]) == "e")
        {
            Image<Vec2b> image;
            if(!argv[3])
            {
                cout<<"Input data not found"<<endl;
                return 0;
            }
            vector<uchar> v;
            image.source = Util::CreateImage(argv[3], v).clone();
            experiment1(image, string("1DRect").c_str());
        }
    }
    
    if(atoi(argv[1]) == 2)
    {
        Image<Vec2b> image;
        if(string(argv[2]) == "a")
        {
            image.source = Util::CreateImage(256, 32).clone();
            cv::Mat disp = image.source.reshape(1);
            imshow("BW", disp);
            experiment2(image, string("32").c_str());
        }
        else if(string(argv[2]) == "b")
        {
            image.source = Util::CreateImage(256, 64).clone();
            cv::Mat disp = image.source.reshape(1);
            imshow("BW", disp);
            experiment2(image, string("64").c_str());
        }
        else if(string(argv[2]) == "c")
        {
            image.source = Util::CreateImage(256, 128).clone();
            cv::Mat disp = image.source.reshape(1);
            imshow("BW", disp);
            experiment2(image, string("128").c_str());
        }
    }
    
    if(atoi(argv[1]) == 3)
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
        experiment3(image);
    }

   
    waitKey(0);
    return 0;
}

template< class T >
int experiment1(Image<T> &image, const char* outfile)
{
    cv::Mat fft = FFT::FFT2D<T>(image.source, -1);

    cout<<"FFT\n" <<fft<<endl;

    ostringstream sout;
    Image<uchar> re(fft, 400, 400, 128, RE);
    imshow("RE", re.source);
    
    sout << "img/fft/" << outfile <<"RE.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), re.source);
    sout.str("");

    Image<uchar> im(fft, 400, 400, 128, IM);
    imshow("IM", im.source);
    
    sout << "img/fft/" << outfile <<"IM.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), im.source);
    sout.str("");

    Image<uchar> mag(fft, 400, 400, 128, MAG);
    imshow("MAG", mag.source);
    
    sout << "img/fft/" << outfile <<"MAG.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), mag.source);
    sout.str("");

    cv::Mat invfft = FFT::FFT2D<Vec2d>(fft, 1);
 
    cout<<"INVFFT\n"<<invfft<<endl;

    return 0;
}



template< class T >
int experiment2(Image<T> &image, const char* outfile)
{
    cv::Mat fft = FFT::FFT2D<T>(image.source, -1, false);
    ostringstream sout;
    vector<cv::Mat> channels(2);
    cv::split(fft, channels);

    cv::Mat mag, reSqr, imSqr;
    cv::multiply(channels[0], channels[0], reSqr);
    cv::multiply(channels[1], channels[1], imSqr);
    cv::sqrt(reSqr + imSqr, mag);
    
    cv::Mat logMag;
    cv::log(cv::abs(mag + 1), logMag);
    Util::Normalize<double>(logMag, Scalar(1500.0), 0);
    imshow("FFT Unshifted (DBL)", logMag);
    logMag.convertTo(logMag, CV_8UC1);
    Util::Normalize<uchar>(logMag, Scalar(255.0), 0);
    imshow("FFT Unshifted", logMag);
    sout << "img/fft/" << outfile <<"unshifted.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), logMag);
    sout.str("");

    fft = FFT::FFT2D<T>(image.source, -1);

    cv::split(fft, channels);
    cv::multiply(channels[0], channels[0], reSqr);
    cv::multiply(channels[1], channels[1], imSqr);
    cv::sqrt(reSqr + imSqr, mag);
    
    cv::log(cv::abs(mag + 1), logMag);
    Util::Normalize<double>(logMag, Scalar(1500.0), 0);
    imshow("FFT Shifted (DBL)", logMag);
    logMag.convertTo(logMag, CV_8UC1);
    Util::Normalize<uchar>(logMag, Scalar(255.0), 0);
    imshow("FFT Shifted", logMag);
    sout << "img/fft/" << outfile <<"shifted.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), logMag);
    sout.str("");

    cv::Mat invfft = FFT::FFT2D<Vec2d>(fft, 1);
 
    return 0;
}


template< class T >
int experiment3(Image<T> &image)
{
    cv::Mat fft = FFT::FFT2D<T>(image.source, -1);
    ostringstream sout;
    vector<cv::Mat> channels(2);
    cv::split(fft, channels);

    cv::Mat zeros(fft.rows, fft.cols, CV_64FC1, Scalar(0.0));
    cv::Mat mag, reSqr, imSqr;
    cv::multiply(channels[0], channels[0], reSqr);
    cv::multiply(channels[1], channels[1], imSqr);
    cv::sqrt(reSqr + imSqr, mag);
    channels[0] = mag.clone();
    channels[1] = zeros.clone();
    cv::merge(channels, fft);
    
    cv::Mat logMag;
    cv::log(cv::abs(mag + 1), logMag);
    Util::Normalize<double>(logMag, Scalar(1.0), 0);

    imshow("FFT", logMag);

    cv::Mat invfft = FFT::FFT2D<Vec2d>(fft, 1);
    cv::split(invfft, channels);

    channels[0].convertTo(channels[0], CV_8UC1);
    imshow("INVFFT Zero Phase", channels[0]);
    sout << "img/fft/zerophase.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), channels[0]);
    sout.str("");

    fft = FFT::FFT2D<T>(image.source, -1);

    cv::split(fft, channels);

    for( int i=0; i<fft.rows; i++ )
        for( int j=0; j<fft.cols; j++ )
        {
            double theta = atan2(channels[1].at<double>(i, j), channels[0].at<double>(i, j));
            fft.at<Vec2d>(i,j)[0] = cos(theta);
            fft.at<Vec2d>(i,j)[1] = sin(theta);
        }

    invfft = FFT::FFT2D<Vec2d>(fft, 1);
    Util::Normalize<double>(invfft, Scalar(1.0), 0);
    cv::split(invfft, channels);
    
    channels[0].convertTo(channels[0], CV_8UC1);
    Util::Normalize<uchar>(channels[0], Scalar(255.0), 0);
    imshow("INVFFT Magnitude 1", channels[0]);
    sout << "img/fft/magone.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), channels[0]);
    sout.str("");

 
    return 0;
}


