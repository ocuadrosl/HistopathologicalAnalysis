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


void HEStainFilter::denoiseHSL(bool showResult)
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

        VTKViewer<rgbInputImageT>::visualize(outputImage, "Denoised image");
    }

}


void HEStainFilter::denoiseLAB(bool showResult)
{


    using rgbToXyzFilterT = ColorConverterFilter<rgbInputImageT, rgbOutputImageT>;
    std::unique_ptr< rgbToXyzFilterT> rgbToXyzFilter(new rgbToXyzFilterT());
    rgbToXyzFilter->setInput(inputImage);
    rgbToXyzFilter->rgbToXyz();


    using xyzToLabFilterT = ColorConverterFilter<rgbOutputImageT, rgbOutputImageT>;
    std::unique_ptr< xyzToLabFilterT> xyzToLabFilter(new xyzToLabFilterT());
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();





    auto labImage = xyzToLabFilter->getOutput();

    //Lab typedefs
    using pixelDouble = itk::RGBPixel<double>;
    using imageDouble = itk::Image< pixelDouble, 2 >;


    outputImage = rgbInputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();



    itk::ImageRegionConstIterator< rgbInputImageT > inputit(inputImage  , inputImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< imageDouble    > labIt  (labImage    , labImage->GetRequestedRegion());
    itk::ImageRegionIterator     < rgbInputImageT > outputit(outputImage, outputImage->GetRequestedRegion());


    auto rgbWhite = itk::NumericTraits<rgbInputPixelT>::Zero+255;
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

        VTKViewer<rgbInputImageT>::visualize(outputImage, "Denoised image");
    }

}



void HEStainFilter:: colorCorrection(bool showResult)
{


    if(showResult)
    {

        VTKViewer<rgbInputImageT>::visualize(inputImage, "Input image");
    }

    //RGB to XYZ
    using rgbToXyzFilterT = ColorConverterFilter<rgbInputImageT, rgbOutputImageT>;
    std::unique_ptr< rgbToXyzFilterT> rgbToXyzFilter(new rgbToXyzFilterT());
    rgbToXyzFilter->setInput(inputImage);
    rgbToXyzFilter->rgbToXyz();





     //XYZ to LAB
    using xyzToLabFilterT = ColorConverterFilter<rgbOutputImageT, rgbOutputImageT>;
    std::unique_ptr< xyzToLabFilterT> xyzToLabFilter(new xyzToLabFilterT());
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();


    rgbOutputImageP labImage = xyzToLabFilter->getOutput();

/*
    //histogram
    using RGBHistogramFilterT  = RGBHistogramFilter<rgbOutputImageT>;
    std::unique_ptr<RGBHistogramFilterT> rgbHistogramFilter(new RGBHistogramFilterT());
    rgbHistogramFilter->setImage(labImage);
    rgbHistogramFilter->setMinPossibleValues(0  , -500, -200);
    rgbHistogramFilter->setMaxPossibleValues(100,  500,  200);

    rgbHistogramFilter->computeHistogram(true); //normalized


    return;

    rgbHistogramFilter->computeComulativeDistribution();
    auto histogram = rgbHistogramFilter->getHistogram();
    auto cumulativeDistro = rgbHistogramFilter->getCumulativeDistribution();
*/


    //histogram equalization

   // auto imageSize = labImage->GetRequestedRegion().GetSize();
   // unsigned long noPixels = imageSize[0] * imageSize[1];


    rgbOutputImageP equalizedImage = rgbOutputImageT::New();
    equalizedImage->SetRegions(labImage->GetRequestedRegion());
    equalizedImage->Allocate();

    itk::ImageRegionIterator<rgbOutputImageT> eqIt  (equalizedImage, equalizedImage->GetRequestedRegion());
    itk::ImageRegionConstIterator<rgbOutputImageT> labIt  (labImage, labImage->GetRequestedRegion());

    //unsigned indexAux;
   // double cdf;
  //  double cdfMinL = cumulativeDistro[0][0] * 100;

    rgbOutputPixelT pixelAux;

    while(!labIt.IsAtEnd())
    {

        pixelAux = labIt.Get();

        pixelAux[0] +=15;

        //indexAux = static_cast<unsigned>(pixelAux[0]);

        //cdf = 100 * cumulativeDistro[0][indexAux];

        //pixelAux[0] = std::floor(cdf);

        eqIt.Set( pixelAux );

        //pixelAux[0] = std::floor( 100 * cumulativeDistro[0][static_cast<unsigned>(pixelAux[0])]);

        //indexAux =  static_cast<unsigned>(std::floor(pixelAux[1])+500);
        //pixelAux[1] = std::floor( 500 * cumulativeDistro[1][indexAux]);

        //indexAux =  static_cast<unsigned>(std::floor(pixelAux[2])+200);
        //pixelAux[2] = std::floor( 200 * cumulativeDistro[2][indexAux]);

        //labIt.Set(pixelAux);

        ++labIt;
        ++eqIt;
    }

    /*

    //Lightness
    auto labImage = xyzToLabFilter->getOutput();
    itk::ImageRegionIterator<rgbOutputImageT> labIt  (labImage, labImage->GetRequestedRegion());
    while(!labIt.IsAtEnd())
    {

        labIt.Get().SetRed(100);
        ++labIt;

    }
*/




    using labToXyzFilterT = ColorConverterFilter<rgbOutputImageT, rgbOutputImageT>;
    std::unique_ptr<labToXyzFilterT> labToXyzFilter(new labToXyzFilterT());
    labToXyzFilter->setInput(equalizedImage);
    labToXyzFilter->labToXyz();


    using xyzToRgbFilterT = ColorConverterFilter<rgbOutputImageT, rgbInputImageT>;
    std::unique_ptr<xyzToRgbFilterT> xyzToRgbFilter(new xyzToRgbFilterT());
    xyzToRgbFilter->setInput(labToXyzFilter->getOutput());
    xyzToRgbFilter->xyzToRgb();



    outputImage = xyzToRgbFilter->getOutput();

    if(showResult)
    {

        VTKViewer<rgbInputImageT>::visualize(outputImage, "XYZ to RGB");
    }


}


void HEStainFilter::setImage(rgbInputImageP inputImage)
{
    this->inputImage = inputImage;
}
