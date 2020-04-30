#include "hestainfilter.h"



HEStainFilter::HEStainFilter():
    hueThresholdYellow(60), hueThresholdBlue(180), saturationThreshold(0.7), lightnessThreshold(0.6)
{

}


typename HEStainFilter::rgbImageUP
HEStainFilter::getOutput() const
{

    return outputImage;

}


void HEStainFilter::denoiseHSV(bool showResult)
{

    //To HSL
    using colorConverterFilterT = ColorConverterFilter<rgbImageU, rgbImageD>;
    std::unique_ptr< colorConverterFilterT> colorConverterFilter(new colorConverterFilterT());
    colorConverterFilter->setInput(inputImage);
    colorConverterFilter->rgbToHsv();
    auto hsvImage = colorConverterFilter->getOutput();


    outputImage = rgbImageU::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();


    itk::ImageRegionConstIterator<rgbImageU> inputit(inputImage  , inputImage ->GetRequestedRegion());
    itk::ImageRegionConstIterator<rgbImageD> hsvIt  (hsvImage    , hsvImage   ->GetRequestedRegion());
    itk::ImageRegionIterator     <rgbImageU> outputit(outputImage, outputImage->GetRequestedRegion());


    auto white = itk::NumericTraits<rgbPixelU>::Zero+255;

    auto pixel = inputit.Get();
    auto hsvPixel = hsvIt.Get();

    while(!inputit.IsAtEnd())
    {

        pixel = inputit.Get();
        hsvPixel = hsvIt.Get();

        if(hsvPixel[0] > hueThresholdYellow  && hsvPixel[0] < hueThresholdBlue ) // not in H&E color space
        {
            outputit.Set(white);
        }

        else
        {
            outputit.Set(pixel);
        }


        ++inputit;
        ++hsvIt;
        ++outputit;

    }

    if(showResult)
    {

        VTKViewer::visualize<rgbImageU>(outputImage, "Denoised image");
    }

}


void HEStainFilter::denoiseLAB(bool showResult)
{


    using rgbToXyzFilterT = ColorConverterFilter<rgbImageU, rgbImageD>;
    std::unique_ptr< rgbToXyzFilterT> rgbToXyzFilter(new rgbToXyzFilterT());
    rgbToXyzFilter->setInput(inputImage);
    rgbToXyzFilter->rgbToXyz();


    using xyzToLabFilterT = ColorConverterFilter<rgbImageD, rgbImageD>;
    std::unique_ptr< xyzToLabFilterT> xyzToLabFilter(new xyzToLabFilterT());
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();





    auto labImage = xyzToLabFilter->getOutput();

    //Lab typedefs
    using pixelDouble = itk::RGBPixel<double>;
    using imageDouble = itk::Image< pixelDouble, 2 >;


    outputImage = rgbImageU::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();



    itk::ImageRegionConstIterator< rgbImageU > inputit(inputImage  , inputImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< imageDouble    > labIt  (labImage    , labImage->GetRequestedRegion());
    itk::ImageRegionIterator     < rgbImageU > outputit(outputImage, outputImage->GetRequestedRegion());


    auto rgbWhite = itk::NumericTraits<rgbPixelU>::Zero+255;
    auto labPixel = labIt.Get();

    while(!inputit.IsAtEnd())
    {

        labPixel = labIt.Get();


        if(labPixel[1] < 0 ) // not in H&E color space -> Eosinophilic
        {
            outputit.Set(rgbWhite);

        }
        else if(labPixel[2] > 0) // not in H&E color space -> Basophilic
        {
            outputit.Set(rgbWhite);

        }
        else if(labPixel[0] < 5) //too white
        {
            outputit.Set(rgbWhite);
        }
        else if(labPixel[0] > 95) //too black
        {
            outputit.Set(rgbWhite);
        }
        else
        {
            outputit.Set(inputit.Get());

            // std::cout<<labPixel[1]<<std::endl;
        }


        ++inputit;
        ++labIt;
        ++outputit;

    }

    if(showResult)
    {

        VTKViewer::visualize<rgbImageU>(outputImage, "Denoised image");
    }

}

void HEStainFilter::setGamma(double gamma)
{

    this->gamma = gamma;
}



void HEStainFilter:: colorEnhancement(bool showResult)
{


    using rgbToHsvFilterT = ColorConverterFilter<rgbImageU, rgbImageD>;
    std::unique_ptr< rgbToHsvFilterT> rgbToHsvFilter(new rgbToHsvFilterT());
    rgbToHsvFilter->setInput(inputImage);
    rgbToHsvFilter->rgbToHsv();
    auto hsvImage = rgbToHsvFilter->getOutput();

    using minMaxRGBImageCalculatorT = MinMaxRGBImageCalculator<rgbImageD>;
    std::unique_ptr<minMaxRGBImageCalculatorT> minMaxImageCal(new minMaxRGBImageCalculatorT());
    minMaxImageCal->setImage(hsvImage);
    minMaxImageCal->compute();
    const auto minHsv = minMaxImageCal->getMinValues();
    const auto maxHsv = minMaxImageCal->getMaxValues();

    itk::ImageRegionIterator<rgbImageD> hsvIt  (hsvImage, hsvImage->GetRequestedRegion());


    rgbPixelD hsvPixel;

    math::MinMax<> minMaxS(minHsv[1], maxHsv[1], 0, 1.0);
    math::MinMax<> minMaxV(minHsv[2], maxHsv[2], 0, 1.0);

    while(!hsvIt.IsAtEnd())
    {

        hsvPixel = hsvIt.Get();

        //std::cout<<hsvPixel<<" -> ";

        hsvPixel[1] = minMaxS(hsvPixel[1]);
        hsvPixel[2] = minMaxV(hsvPixel[2]);
        //std::cout<<hsvPixel<<std::endl;

        hsvIt.Set( hsvPixel );

        ++hsvIt;

    }


    using hsvToRgbFilterT = ColorConverterFilter<rgbImageD, rgbImageU>;
    std::unique_ptr< hsvToRgbFilterT> hsvToRgbFilter(new hsvToRgbFilterT());
    hsvToRgbFilter->setInput(hsvImage);
    hsvToRgbFilter->hsvToRgb();
    outputImage = hsvToRgbFilter->getOutput();


    io::printOK("Color Enhancement");


    if(showResult)
    {
        VTKViewer::visualize<rgbImageU>(outputImage, "Color Enhancement");
    }



}


void HEStainFilter::setImage(rgbImageUP inputImage)
{
    this->inputImage = inputImage;
}
