#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <cstdlib>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBToLuminanceImageFilter.h"

//local includes
#include "../util/customprint.h"

template< typename  pixelType = unsigned int >
class ImageReader
{
public:


    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelType>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer  =   typename  rgbImageType::Pointer;
    using rgbReaderType = itk::ImageFileReader<rgbImageType>;

    //gray scale type alias
    using grayImageType = itk::Image< pixelType, 2>;
    using grayImagePointer = typename grayImageType::Pointer;

    ImageReader();
    ~ImageReader(){}

    void readVSI(std::string inFileName, std::string outFileName, float outMagnification);
    void read(std::string fileName);

    //getters
    rgbImagePointer getRGBImage() const;
    grayImagePointer getGrayScaleImage() const;


private:

    rgbImagePointer rgbImage;


};

//template explicit instantiation
template class ImageReader<unsigned int>;
template class ImageReader<unsigned char>;
template class ImageReader<short>;
#endif // IMAGEREADER_H
