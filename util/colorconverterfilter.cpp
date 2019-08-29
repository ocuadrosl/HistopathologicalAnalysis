#include "colorconverterfilter.h"


template<typename inputPixelComponentT, typename outputPixelComponentT>
ColorConverterFilter<inputPixelComponentT, outputPixelComponentT>::ColorConverterFilter()
{

}

template<typename inputPixelComponentT, typename outputPixelComponentT>
void ColorConverterFilter<inputPixelComponentT, outputPixelComponentT>::setInput(inputImageP inputImage)
{

    this->inputImage = inputImage;

}


template<typename inputPixelComponentT, typename outputPixelComponentT>
typename ColorConverterFilter<inputPixelComponentT, outputPixelComponentT>::outputImageP
ColorConverterFilter<inputPixelComponentT, outputPixelComponentT>::getOutput()
{
    return outputImage;
}


template<typename inputPixelComponentT, typename outputPixelComponentT>
void ColorConverterFilter<inputPixelComponentT, outputPixelComponentT>::rgbToHsv()
{


    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage, inputImage->GetRequestedRegion());

    outputPixelT pixelFloat;
    auto pixel =  inputIt.Get();

    while (!inputIt.IsAtEnd() )
    {

            pixel = inputIt.Get();
            pixelFloat.SetRed( static_cast<outputPixelComponentT>(pixel.GetRed())/255.f);
            pixelFloat.SetGreen( static_cast<outputPixelComponentT>(pixel.GetGreen())/255.f);
            pixelFloat.SetBlue( static_cast<outputPixelComponentT>(pixel.GetBlue())/255.f);


            std::cout<<pixelFloat<<std::endl;





        ++inputIt;
    }




}
