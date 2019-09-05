#include "colorconverterfilter.h"


template <typename inputImageT, typename outputImageT>
ColorConverterFilter<inputImageT, outputImageT>::ColorConverterFilter()
{
     white = Illuminant::getWhitePoint(Illuminant::index::d65);

}

template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::setInput(inputImageP inputImage)
{

    this->inputImage = inputImage;

}


template <typename inputImageT, typename outputImageT>
typename ColorConverterFilter<inputImageT, outputImageT>::outputImageP
ColorConverterFilter<inputImageT, outputImageT>::getOutput()
{
    return outputImage;
}


template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::rgbToHsv()
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
    constexpr auto  mod = Math::mod<outputPixelCompT>;


    auto hsvPixel = outputIt.Get();
    while (!inputIt.IsAtEnd() )
    {

            pixelFloat = static_cast<outputPixelT>(inputIt.Get())/255.f;

            outputPixelCompT maxAux = max(pixelFloat);
            outputPixelCompT delta =  maxAux - min(pixelFloat);

            //Hue calculation

            outputPixelCompT h;

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



template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::rgbToHsl()
{

    if constexpr (std::is_floating_point<outputPixelCompT>::value)
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
        constexpr auto  mod = Math::mod<outputPixelCompT>;


        outputPixelT hslPixel = outputIt.Get();
        while (!inputIt.IsAtEnd() )
        {

            pixelFloat = static_cast<outputPixelT>(inputIt.Get())/255.0;

            outputPixelCompT maxAux = max(pixelFloat);
            outputPixelCompT minAux = min(pixelFloat);
            outputPixelCompT delta =  maxAux - minAux;

            //Hue calculation

            outputPixelCompT h;

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
            hslPixel[1] = (delta == 0) ? 0 : delta / (1 - std::abs( 2*hslPixel[2]-1 ));


            outputIt.Set(hslPixel);

            ++inputIt;
            ++outputIt;
        }


        IO::printOK("RGB to HSL");
    }
    else
    {
        IO::printFail("RGB to HSL", "The output image type must be a float point type");

    }


}


template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>:: labToXyz()
{

    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    outputPixelT XYZ;
    outputPixelT xyzR; //r vector
    outputPixelT xyzF; //f vector

    inputPixelT Lab;
    while (!inputIt.IsAtEnd() )
    {
        Lab = inputIt.Get();

        //Do not change the order
        xyzF[1] = (Lab[0] + 16.0)/116;
        xyzF[2] = xyzF[1] - (Lab[2] / 200);
        xyzF[0] = (Lab[1] / 500) + xyzF[1];


        //to avoid double computation
        double xyzXCube = std::pow(xyzF[0], 3.0);
        double xyzZCube = std::pow(xyzF[2], 3.0);


        xyzR[0] = (xyzXCube > e  )? xyzXCube : (116 * xyzF[0] - 16) / k;
        xyzR[1] = (Lab[0]   > k*e)? std::pow((Lab[0]+16) / 116, 3.0) : Lab[0] / k;
        xyzR[2] = (xyzZCube > e  )? xyzZCube : (116 * xyzF[2] - 16) / k;


        XYZ[0] = xyzR[0] * white[0];
        XYZ[1] = xyzR[1] * white[1];
        XYZ[2] = xyzR[2] * white[2];

        outputIt.Set(XYZ);

        ++inputIt;
        ++outputIt;

        //std::cout<<inputPixel<<" -> "<<xyzPixel<<std::endl;
    }

    IO::printOK("LAB to XYZ");




}


/*
using:
model: sRGB
white: D65
Gamma: sRGB

*/
template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::xyzToLab()
{

    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    outputPixelT xyzR; //r vector
    outputPixelT xyzF; //f vector
    outputPixelT Lab;

    inputPixelT XYZ;
    while (!inputIt.IsAtEnd() )
    {
        XYZ = inputIt.Get();

        xyzR[0] =  XYZ[0] / white[0];
        xyzR[1] =  XYZ[1] / white[1];
        xyzR[2] =  XYZ[2] / white[2];


        xyzF[0] = (xyzR[0] > e) ? std::cbrt(xyzR[0]) : (k * xyzR[0] + 16) / 116;
        xyzF[1] = (xyzR[1] > e) ? std::cbrt(xyzR[1]) : (k * xyzR[1] + 16) / 116;
        xyzF[2] = (xyzR[2] > e) ? std::cbrt(xyzR[2]) : (k * xyzR[2] + 16) / 116;


        Lab[0] = (116 *  xyzF[1]) - 16;
        Lab[1] =  500 * (xyzF[0]  - xyzF[1]);
        Lab[2] =  200 * (xyzF[1]  - xyzF[2]);

        outputIt.Set(Lab);

        ++inputIt;
        ++outputIt;

    }

    IO::printOK("XYZ to CIE Lab");


}


/*
USING:
sRGB model
D65  white
sRGB Gamma
*/
template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>:: xyzToRgb()
{

    //inputPixelT must be a float point type

    //TODO bug here -> orange color!!!
    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage  , inputImage ->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    inputPixelT rgb; //rgb in [0 - 1]
    inputPixelT XYZ;

    outputPixelT outputPixel;

    while (!inputIt.IsAtEnd() )
    {

        XYZ = inputIt.Get();


        rgb[0] = ( 3.2404542 * XYZ[0]) + (-1.5371385 * XYZ[1]) + (-0.4985314 * XYZ[2]);
        rgb[1] = (-0.9692660 * XYZ[0]) + ( 1.8760108 * XYZ[1]) + ( 0.0415560 * XYZ[2]);
        rgb[2] = ( 0.0556434 * XYZ[0]) + (-0.2040259 * XYZ[1]) + ( 1.0572252 * XYZ[2]);




        rgb = sRGBCompanding(rgb);

        //ouput RGB in [0 -255]
        outputPixel[0] = static_cast<outputPixelCompT>(std::round(rgb[0] * 255));
        outputPixel[1] = static_cast<outputPixelCompT>(std::round(rgb[1] * 255));
        outputPixel[2] = static_cast<outputPixelCompT>(std::round(rgb[2] * 255));



        outputIt.Set(outputPixel);


        ++inputIt;
        ++outputIt;

    }


    IO::printOK("XYZ to RGB");

}



/*
USING:
sRGB model
D65  white

*/
template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::rgbToXyz()
{

    //outputPixelT must be a float point type

    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage  ,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());


    outputPixelT rgb; //in [0-1]
    outputPixelT XYZ;


    while (!inputIt.IsAtEnd() )
    {

        rgb = sRGBInverseCompanding(static_cast<outputPixelT>(inputIt.Get())/255.0);

        XYZ[0] = (0.4124564 * rgb[0]) + (0.3575761 * rgb[1]) + (0.1804375 * rgb[2]);
        XYZ[1] = (0.2126729 * rgb[0]) + (0.7151522 * rgb[1]) + (0.0721750 * rgb[2]);
        XYZ[2] = (0.0193339 * rgb[0]) + (0.1191920 * rgb[1]) + (0.9503041 * rgb[2]);


        outputIt.Set(XYZ);

        ++inputIt;
        ++outputIt;

    }

  IO::printOK("RGB to XYZ");

}



template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::outputPixelT
ColorConverterFilter<inputImageT, outputImageT>::lInverseCompanding(const outputPixelT& rgbPixel)
{

    outputPixelT v; //output pixel
    for (unsigned i=0 ; i < 3 ; ++i)
    {
      v[i] = ( rgbPixel[i] < 0.08) ? 100 * rgbPixel[i] / k : std::cbrt((rgbPixel[i] + 0.16) / 1.16);
    }

    return v;

}




template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::inputPixelT
ColorConverterFilter<inputImageT, outputImageT>:: lCompanding   (const inputPixelT& rgbPixel)
{

    inputPixelT V; //output pixel
    for (unsigned i=0 ; i < 3 ; ++i)
    {
        V[i] = (  rgbPixel[i] <= e) ? (rgbPixel[i]*k)/100 : 1.16 * std::cbrt(rgbPixel[i]) - 0.16;
    }

    return V;


}




template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::inputPixelT
ColorConverterFilter<inputImageT, outputImageT>::sRGBCompanding(const inputPixelT& rgbPixel)
{
    //v in {r, g, b} = rgbPixel

    //V in {R, G,B}
    //inputPixelT must be a float point type
    inputPixelT V; //output pixel
    for (unsigned i=0 ; i < 3 ; ++i)
    {
        V[i] = (  rgbPixel[i] <= 0.0031308) ? 12.92*rgbPixel[i] : 1.055*std::pow(rgbPixel[i], 1.0/2.4) - 0.055;
    }

    return V;

}

/*
    rgbPixel must be scaled in the range [0-1]
    //rgbPixel = 'V' in {R, G,B}

*/
template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::outputPixelT
ColorConverterFilter<inputImageT, outputImageT>::sRGBInverseCompanding(const outputPixelT& rgbPixel)
{

    //v in {r, g, b}
    //V in {R, G,B}
    outputPixelT v; //output pixel
    for (unsigned i=0 ; i < 3 ; ++i)
    {

        v[i] = (rgbPixel[i] <= 0.04045)? rgbPixel[i]/12.92 : std::pow((rgbPixel[i] + 0.055)/1.055, 2.4);

    }

    return v;

}

template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::outputPixelT
ColorConverterFilter<inputImageT, outputImageT>::gammaInverseCompanding(const outputPixelT& rgbPixel)
{

    outputPixelT v; //output pixel
    for (unsigned i=0 ; i < 3 ; ++i)
    {
        v[i] = std::pow(rgbPixel[i], g);
    }
    return v;

}


template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::inputPixelT
ColorConverterFilter<inputImageT, outputImageT>:: gammaCompanding(const inputPixelT& rgbPixel)
{

    //v in {r, g, b} = rgbPixel

    //V in {R, G,B}
    //inputPixelT must be a float point type
    inputPixelT V; //output pixel
    for (unsigned i=0 ; i < 3 ; ++i)
    {
        V[i] = std::pow(rgbPixel[i], 1.0/g);
    }

    return V;


}


template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::setWhite(unsigned illuminantIndex)
{
    white = Illuminant::getWhitePoint(illuminantIndex);
}
























