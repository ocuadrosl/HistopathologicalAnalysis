#include "imagereader.h"

ImageReader::ImageReader()
{

}


void ImageReader::read(std::string fileName)
{


}

void ImageReader::readVSI(std::string inFileName, std::string outFileName,  short outMagnification) const
{

    std::string pyCommand = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py "+inFileName+" "+outFileName+" "+std::to_string(outMagnification);
    std::system(pyCommand.c_str());




}
