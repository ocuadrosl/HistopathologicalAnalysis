#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <cstdlib>
#include <itkImage.h>
#include "itkImageFileReader.h"
#include "itkRGBToLuminanceImageFilter.h"
#include <regex>

//local includes
#include "../util/customprint.h"

template< typename  rgbImageT>
class ImageReader
{
public:


    // RGB type  alias
    using rgbPixelT     = typename rgbImageT::PixelType;
    using rgbPixelCompT = typename rgbPixelT::ComponentType;
    using rgbImageP     = typename rgbImageT::Pointer;


    //gray scale type alias
    using grayImageT = itk::Image< rgbPixelCompT, 2>;
    using grayImageP = typename grayImageT::Pointer;

    ImageReader();
    ~ImageReader(){}

    void readVSI(std::string inputFileName, std::string outputFileName, float outMagnification);
    void read(std::string fileName);

    //getters
    rgbImageP  getRGBImage() const;
    grayImageP getGrayScaleImage() const;


private:

    rgbImageP rgbImage;

    std::string regularDirToConsoleDir(std::string dir);


};

//template explicit instantiation
using rgbUIntPixelT   = itk::RGBPixel<unsigned int>;
using rgbDoublePixelT = itk::RGBPixel<double>;
using rgbUCharPixelT  = itk::RGBPixel<unsigned char>;

template class ImageReader<itk::Image<rgbDoublePixelT,2>>;
template class ImageReader<itk::Image<rgbUIntPixelT  ,2>>;
template class ImageReader<itk::Image<rgbUCharPixelT ,2>>;


#endif // IMAGEREADER_H
