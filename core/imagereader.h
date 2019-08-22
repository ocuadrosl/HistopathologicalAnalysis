#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <cstdlib>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRGBToLuminanceImageFilter.h"


class ImageReader
{
public:


    using pixelType=unsigned char;
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

    void readVSI(std::string inFileName, std::string outFileName, short outMagnification);
    void read(std::string fileName);

    //getters
    rgbImagePointer getRGBImage() const;
    grayImagePointer getGrayScaleImage() const;


private:

    rgbImagePointer rgbImage;


};

#endif // IMAGEREADER_H
