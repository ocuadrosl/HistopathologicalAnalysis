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




    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();


    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    outputPixelT pixelFloat;

    //template function alias
    constexpr auto  max = Math::max<outputPixelT>;
    constexpr auto  min = Math::min<outputPixelT>;
    constexpr auto  mod = Math::mod<outputPixelComponentT>;


    auto hsvPixel = outputIt.Get();
    while (!inputIt.IsAtEnd() )
    {

            pixelFloat = static_cast<outputPixelT>(inputIt.Get())/255.f;

            outputPixelComponentT maxAux = max(pixelFloat);
            outputPixelComponentT delta =  maxAux - min(pixelFloat);

            //Hue calculation

            outputPixelComponentT h;

            if(delta == 0)
            {
                hsvPixel[0] = 0;
            }
            else if (maxAux == pixelFloat.GetRed()) //red
            {
                hsvPixel[0] = 60 *  mod( (pixelFloat[1] - pixelFloat[2]) / delta, 6);
            }
            else if (maxAux == pixelFloat.GetGreen()) //green
            {
                hsvPixel[0] = 60 * (((pixelFloat[2] - pixelFloat[0])/delta ) + 2);
            }
            else if (maxAux == pixelFloat.GetBlue()) //blue
            {
                hsvPixel[0] = 60 * (((pixelFloat[0] - pixelFloat[1])/delta) + 4);
            }

            //Saturation
            hsvPixel[1] = (maxAux == 0) ? 0 : delta/maxAux;

            //Value
            hsvPixel[2] = maxAux;

            outputIt.Set(hsvPixel);


           // std::cout<<inputIt.Get() << " -> "<<h<<" "<<s<<" "<<v<<std::endl;


        ++inputIt;
        ++outputIt;
    }


    IO::printOK("RGB to HSV");

}






template<typename inputPixelComponentT, typename outputPixelComponentT>
void ColorConverterFilter<inputPixelComponentT, outputPixelComponentT>::rgbToHsl()
{




    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();


    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    outputPixelT pixelFloat;

    //template function alias
    constexpr auto  max = Math::max<outputPixelT>;
    constexpr auto  min = Math::min<outputPixelT>;
    constexpr auto  mod = Math::mod<outputPixelComponentT>;


    auto hslPixel = outputIt.Get();
    while (!inputIt.IsAtEnd() )
    {

            pixelFloat = static_cast<outputPixelT>(inputIt.Get())/255.f;

            outputPixelComponentT maxAux = max(pixelFloat);
            outputPixelComponentT minAux = min(pixelFloat);
            outputPixelComponentT delta =  maxAux - minAux;

            //Hue calculation

            outputPixelComponentT h;

            if(delta == 0)
            {
                hslPixel[0] = 0;
            }
            else if (maxAux == pixelFloat.GetRed()) //red
            {
                hslPixel[0] = 60 *  mod( (pixelFloat[1] - pixelFloat[2]) / delta, 6);
            }
            else if (maxAux == pixelFloat.GetGreen()) //green
            {
                hslPixel[0] = 60 * (((pixelFloat[2] - pixelFloat[0])/delta ) + 2);
            }
            else if (maxAux == pixelFloat.GetBlue()) //blue
            {
                hslPixel[0] = 60 * (((pixelFloat[0] - pixelFloat[1])/delta) + 4);
            }

            //Lightness
            hslPixel[2] = (maxAux + minAux) / 2;



            //Saturation
            hslPixel[1] = (delta == 0) ? 0 : delta/ (1 - std::abs(2*hslPixel[2]-1));


            outputIt.Set(hslPixel);

        ++inputIt;
        ++outputIt;
    }


    IO::printOK("RGB to HSV");

}
