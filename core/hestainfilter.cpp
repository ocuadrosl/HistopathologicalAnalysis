#include "hestainfilter.h"


template<typename inputPixeComponentT >
HEStainFilter<inputPixeComponentT>::HEStainFilter():
    hueThresholdRed(60), hueThresholdBlue(180), saturationThreshold(0.7), lightnessThreshold(0.6)
{

}

template<typename inputPixeComponentT >
typename HEStainFilter<inputPixeComponentT>::rgbInputImageP
HEStainFilter<inputPixeComponentT>::getOutput() const
{

    return outputImage;

}

template<typename inputPixeComponentT >
void HEStainFilter<inputPixeComponentT>::denoise(bool showResult)
{


    //To HSV
    using colorConverterFilterT = ColorConverterFilter<inputPixeComponentT, double>;
    std::unique_ptr< colorConverterFilterT> colorConverterFilter(new colorConverterFilterT());
    colorConverterFilter->setInput(inputImage);
    colorConverterFilter->rgbToHsl();
    auto hsvImage = colorConverterFilter->getOutput();

    //HSV typedefs
    using hsvPixelDouble = itk::RGBPixel<double>;
    using hsvImageDouble = itk::Image< hsvPixelDouble, 2 >;


    outputImage = rgbInputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();



    itk::ImageRegionConstIterator< rgbInputImageT > inputit(inputImage  , inputImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< hsvImageDouble > hslIt   (hsvImage   , hsvImage->GetRequestedRegion());
    itk::ImageRegionIterator     < rgbInputImageT > outputit(outputImage, outputImage->GetRequestedRegion());


    auto white = itk::NumericTraits<rgbInputPixelT>::Zero+255;

    auto pixel = inputit.Get();
    auto hslPixel = hslIt.Get();

    while(!inputit.IsAtEnd())
    {

        pixel = inputit.Get();
        hslPixel = hslIt.Get();

        if(hslPixel[0] < hueThresholdBlue ) // not in H&E color space
        //if(hslPixel[0] > hueThresholdRed &&  hslPixel[0] < hueThresholdBlue ) // not in H&E color space
        {
            outputit.Set(white);
            //std::cout<< pixel<<"->"<<hslPixel<<std::endl;
        }
        else if(hslPixel[2] < lightnessThreshold &&  hslPixel[1] < saturationThreshold)
        {
            outputit.Set(white);

        }
        else
        {
            outputit.Set(pixel);
        }


        ++inputit;
        ++hslIt;
        ++outputit;

    }

    if(showResult)
    {

        VTKViewer<inputPixeComponentT>::visualizeRGB(outputImage, "Denoised image");
    }

}



template<typename inputPixeComponentT >
void HEStainFilter<inputPixeComponentT>::setImage(rgbInputImageP inputImage)
{
    this->inputImage = inputImage;
}
