#include "imagereader.h"

template< typename  pixelType>
ImageReader<pixelType>::ImageReader(): rgbImage(nullptr)
{

}

template<typename  pixelType>
void ImageReader<pixelType>::readVSI(std::string inFileName, std::string outFileName,  short outMagnification)
{


    IO::printWait("Reading VSI file", "a few minutes");
    std::string pyCommand = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py "+
            inFileName+" "+
            outFileName+" "+std::to_string(outMagnification);

    std::system(pyCommand.c_str());

    typename rgbReaderType::Pointer reader = rgbReaderType::New();
    reader->SetFileName(outFileName);
    reader->Update();
    rgbImage =  reader->GetOutput();

    IO::printOK("Reading VSI file");


}


template<typename  pixelType>
void ImageReader<pixelType>::read(std::string fileName)
{
    typename rgbReaderType::Pointer reader = rgbReaderType::New();
    reader->SetFileName(fileName);
    reader->Update();
    rgbImage =  reader->GetOutput();
}

template<typename  pixelType>
typename ImageReader<pixelType>::rgbImagePointer
ImageReader<pixelType>::getRGBImage() const
{
    return rgbImage;

}


template<typename  pixelType>
typename ImageReader<pixelType>::grayImagePointer
ImageReader<pixelType>::getGrayScaleImage() const
{

    using FilterType = itk::RGBToLuminanceImageFilter< rgbImageType, grayImageType >;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( rgbImage );
    filter->Update();
    return filter->GetOutput();


}
