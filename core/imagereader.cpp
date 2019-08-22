#include "imagereader.h"



template<typename grayImageType>
ImageReader<grayImageType>::ImageReader()
{

}

template<typename grayImageType>
typename ImageReader<grayImageType>::grayImagePointer
ImageReader<grayImageType>::readVSI(std::string inFileName, std::string outFileName,  short outMagnification) const
{
    std::string pyCommand = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py "+inFileName+" "+outFileName+" "+std::to_string(outMagnification);

    std::system(pyCommand.c_str());

    typename readerType::Pointer reader = readerType::New();
    reader->SetFileName("tmpImage.tiff");
    reader->Update();
    return reader->GetOutput();
}
template<typename grayImageType>
typename grayImageType::Pointer
ImageReader<grayImageType>::read(std::string fileName) const
{
    using ReaderType = itk::ImageFileReader< grayImageType >;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(fileName);
    reader->Update();
    return reader->GetOutput();
}


