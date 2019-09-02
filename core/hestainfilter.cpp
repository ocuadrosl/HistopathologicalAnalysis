#include "hestainfilter.h"



HEStainFilter::HEStainFilter():
    hueThresholdRed(60), hueThresholdBlue(180), saturationThreshold(0.7), lightnessThreshold(0.6)
{

}


typename HEStainFilter::rgbInputImageP
HEStainFilter::getOutput() const
{

    return outputImage;

}


void HEStainFilter::denoise(bool showResult)
{


    //To HSV
    using colorConverterFilterT = ColorConverterFilter<rgbInputImageT, rgbOutputImageT>;
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

        VTKViewer<inputPixelComponentT>::visualizeRGB(outputImage, "Denoised image");
    }

}




void HEStainFilter::setImage(rgbInputImageP inputImage)
{
    this->inputImage = inputImage;
}
