#include <iostream>

#include "Image.hpp"

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    if( argc != 2) 
    {
        cout <<" Usage: process_image <image>" << endl;
        return -1;
    }

    Image<GRAYSCALE> image(argv[1], GRAY);   

    if(! image.source.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    // Affine Transform
    //cv::Mat trans = Transform::Translate(image.source.cols/2, image.source.rows/2) *
    //                Transform::Rotate(M_PI/4) *
    //                Transform::Translate(image.source.cols/-2, image.source.rows/-2);
   

    //image.AffineTransform(trans);
    
    // Create Histogram
    cv::Vec<int,2> range(0, 255);
    int channel = 0;
    int size = 255;

    //Image<GRAYSCALE> histograph(image.CreateHistogram(channel, size, range));
    cv::Mat_<double> hist(image.CreateHistogram(channel, size, range));
    //Image<GRAYSCALE> histograph(hist);
    image.Equalize(hist);

    namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
    imshow( "Display window", image.source );                   

    waitKey(0);                                         
    return 0;
}
