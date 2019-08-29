#include "hestainfilter.h"


template<typename inputPixeComponentT, typename outputPixeComponentT >
HEStainFilter<inputPixeComponentT, outputPixeComponentT>::HEStainFilter()
{

}


template<typename inputPixeComponentT, typename outputPixeComponentT >
void HEStainFilter<inputPixeComponentT, outputPixeComponentT>::denoise(bool showResult)
{


    //

    std::unique_ptr<ColorConverterFilter<inputPixeComponentT, float>> colorConverterFilter(new ColorConverterFilter<inputPixeComponentT, float>());
    colorConverterFilter->setInput(inputImage);
    colorConverterFilter->rgbToHsv();


     itk::ImageRegionIterator< rgbInputImageT > it(inputImage  , inputImage->GetRequestedRegion());


     auto white = itk::NumericTraits<rgbInputPixelT>::Zero+255;

     auto pixel = it.Get();

     while(!it.IsAtEnd())
     {

        pixel = it.Get();

        if(!( pixel.GetGreen() > 127  && pixel.GetRed() < 255 && pixel.GetRed() < 255))
        {
            it.Set(white);
        }

        ++it;

     }

     if(showResult)
     {

         VTKViewer<inputPixeComponentT>::visualizeRGB(inputImage, "Denoised image");
     }

}



template<typename inputPixeComponentT, typename outputPixeComponentT >
void HEStainFilter<inputPixeComponentT, outputPixeComponentT>::setImage(rgbInputImageP inputImage)
{
    this->inputImage = inputImage;
}
