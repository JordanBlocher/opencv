#include <iostream>
#include <sstream>

#include "Image.hpp"
#include "IntensityTransform.hpp"
#include "AffineTransform.hpp"

using namespace std;
using namespace cv;

template< class T >
int testSpecification(Image<T>&, Image<T>&, const char*);

template< class T >
int testEqualization(Image<T>&, const char*);

template< class T >
int testAffine(Image<T>&, const char*, int, int, double, double);

template< class T >
int testInterpolate(Image<T>&, const char*, int, double, double);


int project1(int argc, char* argv[])
{
    if( argc > 8 || argc < 3) 
    {
        cout <<" Usage: process_image <image> <outfile> (option)\n"
            "Options: 1. <image> <outfile> <1> <num> for quantization where num is an exponent power of 2"
            "\t 2. <image> <outfile> <2> for equalization.\n"
            "\t 3. <image> <outfile> <3> <specifyingImage> for histogram specification.\n"
            "\t 4. <image> <outfile> <4> (interpolation option) (transform option) for affine transform\n"
            "\t Interpolation Options: 1. <1> for nearest neighbor\n"
            "\t\t 2. <2> for averaging\n"
            "\t\t 3. <3> for bilinear interpolation\n"
            "\t\t\t Transform Options: 1. <1> <angle> for rotate\n"
            "\t\t\t 2. <2> <x> <y> for translate\n"
            "\t\t\t 3. <3> <x> <y> for scale\n"
            "\t\t\t 4. <4> <x> for shear x\n"
            "\t\t\t 5. <5> <y> for shear y\n" 
            "Example: ./process_image boat.png boat 4 1 3 2 2 (scale x and y by 2 using nearest neighbor)"<<endl; 
        return -1;
    }

    Image<uchar> image(argv[1], GRAY);   
    if(! image.source.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    int channel = 0;
  
    if(atoi(argv[3]) == 1)
    {
        if( atoi(argv[4]) < 8 )
        {
            IntensityTransform::Quantize<uchar>(image.source, atoi(argv[4]), channel);

            ostringstream sout;
            sout << "img/quantize/" << argv[2] << argv[4] << ".png";
            
            cout << "Writing image to " << sout.str() << endl;
            imwrite(sout.str().c_str(), image.source);

            imshow("Quantized",image.source);
       }
       else { cout << "Input value must give a value < 256"<<endl; }
    }


    if(atoi(argv[3]) == 2)
        testEqualization(image, argv[2]);       
    
    if(atoi(argv[3]) == 3)
    {
        Image<uchar> s_image(argv[4], GRAY);
        if(! s_image.source.data )                              
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        testSpecification(image, s_image, argv[2]);
    }

    if(atoi(argv[3]) == 4)
    {
        testInterpolate( image, argv[2], atoi(argv[4]), atoi(argv[5]), atof(argv[6]));
    }
 
    waitKey(0);
    return 0;
}

template< class T >
int testInterpolate(Image<T> &image, const char* outfile, int option, double sx, double sy)
{
   // Affine Transform
    int channel = 0;
    int size = 256;
    int xsize = image.source.cols;
    int ysize = image.source.rows;
    Mat trans;
    Image<T> orig = image;
    string msg = "scale";
    
    trans = AffineTransform::Scale(1.0/sx, 1.0/sy);
    xsize = image.source.cols * (1.0/sx);
    ysize = image.source.rows * (1.0/sy);
    AffineTransform::Transform<T>(image.source, trans, xsize, ysize, (InterpolateType)1);
    trans = AffineTransform::Scale(sx, sy);
    xsize = image.source.cols * sx;
    ysize = image.source.rows * sy;
    AffineTransform::Transform<T>(image.source, trans, xsize, ysize, (InterpolateType)option);
    double err = Util::ComputeSquareError<uchar>(image.source, orig.source, image.squareError, size, channel);
    cout<< "Err from " << outfile << option<< "= "<< err <<endl;

    ostringstream sout;
    sout << "img/interpolate/" << outfile << msg << option << ".png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");

    imshow("Interpolate", image.source);
    
    Image<uchar> imgErr(image.squareError, 400, 400, 256, ERROR);
    sout << "img/interpolate/" << outfile << "err" << option << ".png";


    imshow("Error", imgErr.source);
            
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), imgErr.source);

    return 0;
}

template< class T >
int testAffine(Image<T> &image, const char* outfile, int op1, int op2, double x, double y)
{
   // Affine Transform
    int xsize = image.source.cols;
    int ysize = image.source.rows;
    int offsetx, offsety;
    Mat trans;
    Image<T> orig = image;
    string msg;

    switch(op2)
    {
        case 1:
            xsize = image.source.cols*cos(x) + image.source.rows*sin(x);
            ysize = image.source.cols*sin(x) + image.source.rows*cos(x);
            offsetx = xsize/2 + (xsize/2 - image.source.cols);
            offsety = ysize/2 + (ysize/2 - image.source.rows);
            trans = AffineTransform::Translate(image.source.cols/2, image.source.rows/2) *
                            AffineTransform::Rotate(x) *
                            AffineTransform::Translate(-offsetx, -offsety);
            msg = "rotate";
           break;
        case 2:
            trans = AffineTransform::Translate(x, y);
            msg = "translate";
            break;
        case 3:
            trans = AffineTransform::Scale(x, y);
            xsize = image.source.cols * x;
            ysize = image.source.rows * y;
            msg = "scale";
            break;
        case 4:
            trans = AffineTransform::ShearX(x);
            msg = "shearx";
            break;
        case 5:
            trans = AffineTransform::ShearY(x);
            msg = "sheary";
            break;
    }
    AffineTransform::Transform(image.source, trans, xsize, ysize, (InterpolateType)op1);
   
    ostringstream sout;
    sout << "img/affine/" << outfile << msg << op1 << ".png";

    imshow("Transformed", image.source);
            
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);

    return 0;
}

template< class T >
int testEqualization(Image<T> &image, const char* outfile)
{
    // Create Histogram
    int channel = 0;
    int size = 256;
    
    IntensityTransform::CreateHistogram<T>(image.histogram, image.source, channel);
    Image<uchar> histographPDF(image.histogram, 400, 400, 256, HPDF);
    Image<uchar> histographCDF(image.histogram, 400, 400, 256, HCDF);
    
    IntensityTransform::Equalize<T>(image.source, size, channel);

    IntensityTransform::CreateHistogram<T>(image.histogram, image.source, channel);
    Image<uchar> histographPDF2(image.histogram, 400, 400, 256, HPDF);
    Image<uchar> histographCDF2(image.histogram, 400, 400, 256, HCDF);

    ostringstream sout;
    sout << "img/equalize/" << outfile << "pdf.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographPDF.source);
    sout.str("");
    sout << "img/equalize/" << outfile << "cdf.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographCDF.source);
    sout.str("");
    sout << "img/equalize/" << outfile << "equalized.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    sout << "img/equalize/" << outfile << "pdf_out.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographPDF2.source);
    sout.str("");
    sout << "img/equalize/" << outfile << "cdf_out.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographCDF2.source);

    imshow("Equalized", image.source);
   
    return 0;
}

template< class T >
int testSpecification(Image<T> &image, Image<T> &s_image, const char* outfile)
{
    // Create Histogram
    int channel = 0;
    int size = 256;
    
    IntensityTransform::CreateHistogram<T>(s_image.histogram, s_image.source, channel);
    IntensityTransform::CreateHistogram<T>(image.histogram, image.source, channel);
    Image<uchar> histographPDF(image.histogram, 400, 400, 256, HPDF);
    Image<uchar> histographCDF(image.histogram, 400, 400, 256, HCDF);
    Image<uchar> s_histographPDF(s_image.histogram, 400, 400, 256, HPDF);
    Image<uchar> s_histographCDF(s_image.histogram, 400, 400, 256, HCDF);
    
    IntensityTransform::SpecifyHistogram<T>(image.source, s_image.histogram, size, channel);
    IntensityTransform::CreateHistogram<T>(image.histogram, image.source, channel);
    Image<uchar> histographPDF2(image.histogram, 400, 400, 256, HPDF);
    Image<uchar> histographCDF2(image.histogram, 400, 400, 256, HCDF);

    ostringstream sout;
    sout << "img/specify/" << outfile << "pdf.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographPDF.source);
    sout.str("");
    sout << "img/specify/" << outfile << "cdf.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographCDF.source);
    sout.str("");
    sout << "img/specify/" << outfile << "s_pdf.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), s_histographPDF.source);
    sout.str("");
    sout << "img/specify/" << outfile << "s_cdf.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), s_histographCDF.source);
    sout.str("");
 
    sout << "img/specify/" << outfile << "specified.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), image.source);
    sout.str("");
    sout << "img/specify/" << outfile << "s_pdf_out.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographPDF2.source);
    sout.str("");
    sout << "img/specify/" << outfile << "s_cdf_out.png";
    cout << "Writing image to " << sout.str() << endl;
    imwrite(sout.str().c_str(), histographCDF2.source);
 
    imshow("Specified", image.source);
    imshow("Original CDF", s_histographCDF.source);
    imshow("Specified CDF", histographCDF2.source);

    return 0;
}


