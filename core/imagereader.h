#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <cstdlib>
#include "itkImage.h"
#include "itkImageFileReader.h"

class ImageReader
{

public:

    ImageReader();
    ~ImageReader(){}
    void read(std::string fileName);

    void readVSI(std::string inFileName, std::string outFileName, short outMagnification) const;

private:
    using pixelType = unsigned char;
    using imageType = itk::Image<pixelType, 2>;


};

#endif // IMAGEREADER_H
