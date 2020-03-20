#include "imagereader.h"

template< typename  rgbImageT>
ImageReader<rgbImageT>::ImageReader(): rgbImage(nullptr)
{

}

template<typename  rgbImageT>
void ImageReader<rgbImageT>::readVSI(std::string inputFileName, std::string outputFileName,  float outputMag)
{


    //python reads args within quotes
    IO::printWait("Reading and resizing the VSI file", "a few minutes");
    std::string pythonCmd = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py '"+
            inputFileName+"' '"+
            outputFileName+"' '"+std::to_string(outputMag)+"'";


    std::system(pythonCmd.c_str());

    using rgbReaderT = itk::ImageFileReader<rgbImageT>;
    using rgbReaderP    = typename rgbReaderT::Pointer;

    rgbReaderP reader = rgbReaderT::New();
    reader->SetFileName(outputFileName);
    reader->Update();
    rgbImage =  reader->GetOutput();

    IO::printOK("Reading VSI file");



}

template<typename  rgbImageT>
void ImageReader<rgbImageT>::read(std::string fileName)
{

    using rgbReaderT = itk::ImageFileReader<rgbImageT>;
    using rgbReaderP    = typename rgbReaderT::Pointer;

    rgbReaderP reader = rgbReaderT::New();

    reader->SetFileName(fileName);
    reader->Update();
    rgbImage =  reader->GetOutput();
}

template<typename  rgbImageT>
typename ImageReader<rgbImageT>::rgbImageP
ImageReader<rgbImageT>::getRGBImage() const
{
    return rgbImage;

}


template<typename  rgbImageT>
typename ImageReader<rgbImageT>::grayImageP
ImageReader<rgbImageT>::getGrayScaleImage() const
{
    using FilterType = itk::RGBToLuminanceImageFilter< rgbImageT, grayImageT >;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput( rgbImage );
    filter->Update();
    return filter->GetOutput();
}



template<typename  rgbImageT>
std::string ImageReader<rgbImageT>::regularDirToConsoleDir(std::string dir)
{


    //todo implement this..

    return dir;

}













