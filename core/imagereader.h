#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <cstdlib>
#include "itkImage.h"
#include "itkImageFileReader.h"

template<typename grayImageType=itk::Image<unsigned int,2>>
class ImageReader
{
public:
    // gray scale itk image
    //using pixelType = unsigned int;
    //using imageType = itk::Image<pixelType, 2>;
    using readerType = itk::ImageFileReader<grayImageType>;
    using grayImagePointer = typename grayImageType::Pointer;

    ImageReader();
    ~ImageReader(){}

    grayImagePointer readVSI(std::string inFileName, std::string outFileName, short outMagnification) const;
    typename grayImageType::Pointer read(std::string fileName) const;



};

//Explicit instantiation
template class ImageReader<itk::Image<unsigned char, 2>>;
template class ImageReader<itk::Image<unsigned int, 2>>;

#endif // IMAGEREADER_H
