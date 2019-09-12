#include "imagereader.h"

template< typename  pixelType>
ImageReader<pixelType>::ImageReader(): rgbImage(nullptr)
{

}

template<typename  pixelType>
void ImageReader<pixelType>::readVSI(std::string inputFileName, std::string outputFileName,  float outputMag)
{


    IO::printWait("Reading and resizing the VSI file", "a few minutes");
    std::string pythonCmd = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py "+
            inputFileName+" "+
            outputFileName+" "+std::to_string(outputMag);


    std::system(pythonCmd.c_str());

    typename rgbReaderType::Pointer reader = rgbReaderType::New();
    reader->SetFileName(outputFileName);
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
