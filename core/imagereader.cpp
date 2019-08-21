#include "imagereader.h"

ImageReader::ImageReader()
{

}


ImageReader::imageType::Pointer ImageReader::readVSI(std::string inFileName, std::string outFileName,  short outMagnification) const
{

    std::string pyCommand = "python  /home/oscar/src/HistopathologicalAnalysis/python/vsiReader.py "+inFileName+" "+outFileName+" "+std::to_string(outMagnification);

    std::system(pyCommand.c_str());


    readerType::Pointer reader = readerType::New();
    reader->SetFileName( "tmpImage.tiff" );
    reader->Update();
    return reader->GetOutput();
}

ImageReader::imageType::Pointer ImageReader::read(std::string fileName) const
{
    using ReaderType = itk::ImageFileReader< imageType >;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(fileName);
    reader->Update();
    return reader->GetOutput();
}
