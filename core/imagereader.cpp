#include "imagereader.h"


ImageReader::ImageReader(): rgbImage(nullptr)
{
    //std::cout<<typeid(pixelType).name()<<std::endl;
}


void ImageReader::readVSI(std::string inFileName, std::string outFileName,  short outMagnification)
{
    std::string pyCommand = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py "+inFileName+" "+outFileName+" "+std::to_string(outMagnification);

    std::system(pyCommand.c_str());

    typename rgbReaderType::Pointer reader = rgbReaderType::New();
    reader->SetFileName("tmpImage.tiff");
    reader->Update();
    rgbImage =  reader->GetOutput();
}



void ImageReader::read(std::string fileName)
{
    typename rgbReaderType::Pointer reader = rgbReaderType::New();
    reader->SetFileName(fileName);
    reader->Update();
    rgbImage =  reader->GetOutput();
}


ImageReader::rgbImagePointer ImageReader::getRGBImage() const
{
    return rgbImage;

}



typename ImageReader::grayImagePointer ImageReader::getGrayScaleImage() const
{

    using FilterType = itk::RGBToLuminanceImageFilter< rgbImageType, grayImageType >;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( rgbImage );
    filter->Update();
    return filter->GetOutput();


}
