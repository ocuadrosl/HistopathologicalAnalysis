#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <cstdlib>
#include "itkImage.h"
#include "itkImageFileReader.h"

class ImageReader
{
public:
    using pixelType = unsigned char;
    using imageType = itk::Image<pixelType, 2>;
    using readerType = itk::ImageFileReader<imageType>;

    ImageReader();
    ~ImageReader(){}

    imageType::Pointer readVSI(std::string inFileName, std::string outFileName, short outMagnification) const;
    imageType::Pointer read(std::string fileName) const;

};

#endif // IMAGEREADER_H
