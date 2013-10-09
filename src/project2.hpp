#include <iostream>
#include <sstream>

#include "Image.hpp"
#include "AffineTransform.hpp"
#include "Filter.hpp"
#include "Noise.hpp"

using namespace std;
using namespace cv;

template< class T >
int testCorrelation(Image<T>&, Image<T>&, const char*);

template< class T >
int testSmoothing(Image<T>&, MasqueType, const char*);

template< class T >
int testMedian(Image<T>&, int, const char*, int);

template< class T >
int testAveraging(Image<T>&, int, const char*, int);

template< class T >
int testSharpening(Image<T>&, MasqueType, const char*);

template< class T >
int testUnsharpening(Image<T>&, Image<T>&, const char*, double);


int project2(int argc, char* argv[])
{
    if( argc > 8 || argc < 3) 
    {
        cout <<" Usage: process_image <image> <outfile> (option)\n\n"
            "Options: 1. <image> <outfile> <1> <imageMasque> for Correlation\n\n "
            "\t 2. <image> <outfile> <2> (options) for Smoothing\n"
            "\t Smoothing Options: 1. <15> for Gaussian 15x15 Filter\n"
            "\t\t\t    2. <7> for Gaussian 7x7 Filter\n\n"
            "\t 3. <image> <outfile> <3> (options) for Median\n"
            "\t Median Options: 1. <filterSize> <salt&pepperPercentage>\n\n"
            "\t 4. <image> <outfile> <4> (options) for Averaging\n"
            "\t Averaging Options: 1. <filterSize> <salt&pepperPercentage>\n\n"
            "\t 5. <image> <outfile> <5> (options) for Sharpening\n"
            "\t Sharpening Options: 1. <0> for Sobel Filter\n"
            "\t\t\t     2. <1> for Prewitt Filter\n"
            "\t\t\t     3. <2> for Laplacian Filter\n\n"
            "\t 6. <image> <outfile> <6> <smoothImage> <A> for Unsharpening (High Boost/Low Pass)\n"
            "\t (A = 1 for High Pass Filter)\n\n"
            "\tExample: ./process_image boat.png boatSmooth15x15 2 15 (smooth boat using Gaussian 15x15)"<<endl; 
        return -1;
    }

    Image<uchar> image(argv[1], GRAY);   
    if(! image.source.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    if(atoi(argv[3]) == 1)
    {
        Image<uchar> masque(argv[4], GRAY);
        if(! masque.source.data )                              
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        testCorrelation(image, masque, argv[2]);
    }
    
    if(atoi(argv[3]) == 2)
        testSmoothing(image, (MasqueType)(atoi(argv[4])), argv[2]);
    
    if(atoi(argv[3]) == 3)
        testMedian(image, (int)(atoi(argv[4])), argv[2], (int)atoi(argv[3]));

    if(atoi(argv[3]) == 4)
        testAveraging(image, (int)(atoi(argv[4])), argv[2], (int)atoi(argv[3]));

    if(atoi(argv[3]) == 5)
        testSharpening(image, (MasqueType)(atoi(argv[4])), argv[2]);
    
    if(atoi(argv[3]) == 6)
    {
        Image<uchar> smooth(argv[4], GRAY);
        if(! smooth.source.data )                              
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        testUnsharpening(image, smooth, argv[2], (double)atof(argv[3]));
    }
 
    waitKey(0);
    return 0;
}

template< class T >
int testCorrelation(Image<T> &image, Image<T> &masque, const char* outfile)
{
    Filter::Correlation<T>(image.source, masque.source, true, true);

    ostringstream sout;
    sout << "img/filter/" << outfile << ".png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    
    imshow("Correlation", image.source);

    return 0;
}

template< class T >
int testSmoothing(Image<T> &image, MasqueType type, const char* outfile)
{
    int channel = 0;

    Mat masque;
    if(type == GAUSSIAN15)
        masque = Filter::Gaussian15();
    else if(type == GAUSSIAN7)
        masque = Filter::Gaussian7();

    Scalar sum = cv::sum(masque);
    masque /= sum[channel];

    cv::Mat smooth = Filter::Correlation<T>(image.source, masque, false, true);

    ostringstream sout;
    sout << "img/filter/" << outfile <<".png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);

    sout.str("");
    sout << "img/filter/" << outfile <<".pgm";
    imwrite(sout.str().c_str(), smooth);
    
    imshow("Smoothing", image.source);

    return 0;
}


template< class T >
int testMedian(Image<T> &image, int filterSize, const char* outfile, int percentage)
{
    Noise::SaltandPepper<T>(image.source, percentage);
    imshow("SaltandPepper", image.source);

    Filter::Median<T>(image.source, filterSize, true);

    ostringstream sout;
    sout << "img/filter/" << outfile << ".png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    
    imshow("Median", image.source);

    return 0;
}

template< class T >
int testAveraging(Image<T> &image, int filterSize, const char* outfile, int percentage)
{
    Noise::SaltandPepper<T>(image.source, percentage);
    imshow("SaltandPepper", image.source);

    cv::Mat filter(filterSize, filterSize, CV_64F, Scalar(1.0));
    Filter::Correlation<T>(image.source, filter, true, true);

    ostringstream sout;
    sout << "img/filter/" << outfile << ".png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    
    imshow("Averaging", image.source);

    return 0;
}

template< class T >
int testSharpening(Image<T> &image, MasqueType type, const char* outfile)
{
    int channel = 0;
    cv::Mat masque;
    string msg = "";
    switch(type)
    {
        case SOBEL: masque = Filter::Sobel();
                    break;
        case PREWITT: masque = Filter::Prewitt();
                      break;
        case LAPLACIAN: masque = Filter::Laplacian();
                       break;
        default: cerr<<"Masque does not exist;";
                 return -1;
    }


    cv::Mat xshow = image.source.clone();
    cv::Mat xderiv = Filter::Correlation<T>(xshow, masque, true, true);
    if(type == SOBEL || type == PREWITT)
        msg = "_xderiv";

    ostringstream sout;
    sout << "img/filter/" << outfile << msg <<"x.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    
    imshow("Sharpening x", xshow);

    if(type == SOBEL || type == PREWITT)
    {
        cv::Mat yshow = image.source.clone();
        masque = masque.t();
        msg = "_yderiv";

        cv::Mat yderiv = Filter::Correlation<T>(yshow, masque, true, true);

        sout << "img/filter/" << outfile << msg <<"y.png";
        cout << "Writing image to " << sout.str() << endl;
        imwrite(sout.str().c_str(), yshow);
        sout.str("");
        
        imshow("Sharpening y", yshow);

        cv::Mat gradient = xderiv.mul(xderiv) + yderiv.mul(yderiv);
        cv::sqrt(gradient, gradient);
        msg = "_gradient";

        Util::Normalize<T>(gradient, Scalar(255.0), channel);
        gradient.convertTo(image.source, image.source.type(), 1.0);
        
        sout << "img/filter/" << outfile << msg <<"grad.png";
        cout << "Writing image to " << sout.str() << endl;
        imwrite(sout.str().c_str(), image.source);
        sout.str("");

        imshow("Sharpening Gradient", image.source);
     
    }

    return 0;
}


template< class T >
int testUnsharpening(Image<T> &image, Image<T> &smooth, const char* outfile, double A)
{
    int channel = 0;
    cv::Mat unsharp = A*image.source - smooth.source;
    Util::Normalize<T>(unsharp, Scalar(255.0), channel);

    ostringstream sout;
    sout << "img/filter/" << outfile << ".png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    
    imshow("Unsharpening", unsharp);

    return 0;
}


