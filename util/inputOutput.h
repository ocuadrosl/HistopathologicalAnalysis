#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H


#include <iostream>
#include <itkImageFileWriter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCastImageFilter.h>

#include "customprint.h"


namespace io
{


template<typename InputImageT>
inline void WriteImage(typename InputImageT::Pointer inputImage, std::string fileName)
{


    using OutputImageT = itk::Image<itk::RGBPixel<unsigned char>, 2>;
    using InputPixelT = typename InputImageT::PixelType;

    if constexpr(std::is_arithmetic<InputPixelT>::value) // is gray-level
    {


        using RescaleType = itk::RescaleIntensityImageFilter<InputImageT, InputImageT>;
        typename RescaleType::Pointer rescale = RescaleType::New();
        rescale->SetInput(inputImage);
        rescale->SetOutputMinimum(itk::NumericTraits<InputPixelT>::Zero);
        rescale->SetOutputMaximum(itk::NumericTraits<InputPixelT>::Zero+255);
        rescale->Update();
        inputImage = rescale->GetOutput();

    }

    using FilterType = itk::CastImageFilter<InputImageT, OutputImageT>;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(inputImage);

    using WriterType = itk::ImageFileWriter<OutputImageT>;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(fileName);
    writer->SetInput(filter->GetOutput());

    try
    {
        writer->Update();
        printOK("Writing Image");
    }
    catch (itk::ExceptionObject & e)
    {
        std::cerr << "Error: Writing image "+fileName << e << std::endl;
        return;
    }





}


}



#endif // INPUTOUTPUT_H
