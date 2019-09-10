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

    //aux variables
    outputPixelCompT tx, ty, tz;

    while (!inputIt.IsAtEnd() )
    {
        Lab = inputIt.Get();

        tx = ((Lab[0] + 16) / 116) + (Lab[1]/500);
        ty = ( Lab[0] + 16) / 116;
        tz = ((Lab[0] + 16) / 116) - (Lab[2]/200);

        XYZ[0] = cieLabToXYZFunc(tx) * white[0];
        XYZ[1] = cieLabToXYZFunc(ty) * white[1];
        XYZ[2] = cieLabToXYZFunc(tz) * white[2];

        outputIt.Set(XYZ/100);

        //std::cout<<Lab<<" -> "<<XYZ/100<<std::endl;

        ++inputIt;
        ++outputIt;

    }

    IO::printOK("LAB to XYZ");

}


/*
Wikipedia version
*/
template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::outputPixelCompT
ColorConverterFilter<inputImageT, outputImageT>::cieLabToXYZFunc(outputPixelCompT t)
{
    //0.137931034 = 4/29
    return ( t > g ) ? std::pow(t,3) : (3*std::pow(g, 2)) * (t - 0.137931034);
}



/*
Wikipedia version
*/
template <typename inputImageT, typename outputImageT>
inline typename ColorConverterFilter<inputImageT, outputImageT>::outputPixelCompT
ColorConverterFilter<inputImageT, outputImageT>::xyzToCieLabFunc(outputPixelCompT t)
{
    //0.137931034 = 4/29
    return ( t > std::pow(g,3)) ? std::cbrt(t) :  (t / (3*std::pow(g,2))) + 0.137931034;
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

    //aux variables
    outputPixelCompT xw, yw, zw; //input XYZ/white

    while (!inputIt.IsAtEnd() )
    {
        //normalizing XYZ white [0 - 1] to  [0 - 100]
        XYZ = inputIt.Get()*100;

        xw = XYZ[0] / white[0];
        yw = XYZ[1] / white[1];
        zw = XYZ[2] / white[2];

        Lab[0] = (116 * xyzToCieLabFunc(yw)) - 16;
        Lab[1] = 500 * (xyzToCieLabFunc(xw) -  xyzToCieLabFunc(yw));
        Lab[2] = 200 * (xyzToCieLabFunc(yw) - xyzToCieLabFunc(zw));


        //std::cout<<inputIt.Get()<<" -> "<<Lab<<std::endl;

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


    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage  , inputImage ->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    inputPixelT rgb; //rgb in [0 - 1]
    inputPixelT XYZ; //XYZ in [0 - 1]

    outputPixelT outputPixel;

    while (!inputIt.IsAtEnd() )
    {

        XYZ = inputIt.Get();

        //TODO esto es solo para verificar que los colores no se salgan hay un paper
        //https://link.springer.com/article/10.1007/s10043-019-00499-2
        XYZ[0] = (XYZ[0]>white[0])? white[0]: XYZ[0];
        XYZ[1] = (XYZ[1]>white[1])? white[1]: XYZ[1];
        XYZ[2] = (XYZ[2]>white[2])? white[2]: XYZ[2];


       rgb[0] = ( 3.2404542 * XYZ[0]) + (-1.5371385 * XYZ[1]) + (-0.4985314 * XYZ[2]);
       rgb[1] = (-0.9692660 * XYZ[0]) + ( 1.8760108 * XYZ[1]) + ( 0.0415560 * XYZ[2]);
       rgb[2] = ( 0.0556434 * XYZ[0]) + (-0.2040259 * XYZ[1]) + ( 1.0572252 * XYZ[2]);


        rgb = sRGBCompanding(rgb);

        //ouput RGB in [0 - 255]
        outputPixel[0] = static_cast<outputPixelCompT>(std::floor(rgb[0] * 255));
        outputPixel[1] = static_cast<outputPixelCompT>(std::floor(rgb[1] * 255));
        outputPixel[2] = static_cast<outputPixelCompT>(std::floor(rgb[2] * 255));


        outputPixel[0] = (outputPixel[0]>255)? 255 : outputPixel[0];
        outputPixel[1] = (outputPixel[1]>255)? 255 : outputPixel[1];
        outputPixel[2] = (outputPixel[2]>255)? 255 : outputPixel[2];

        outputIt.Set(outputPixel);

        if(outputPixel[0]>255 || outputPixel[1]>255||outputPixel[2]>255 )

            std::cout<<XYZ<<" -> "<<outputPixel<<std::endl;


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


    outputPixelT rgb; //Linear RGB in [0-1]
    outputPixelT XYZ;


    while (!inputIt.IsAtEnd() )
    {

        rgb = sRGBInverseCompanding( static_cast<outputPixelT>(inputIt.Get()) / 255.0 );

        //std::cout<<inputIt.Get()<<" -> "<<rgb<<std::endl;

        XYZ[0] = (0.4124564 * rgb[0]) + (0.3575761 * rgb[1]) + (0.1804375 * rgb[2]);
        XYZ[1] = (0.2126729 * rgb[0]) + (0.7151522 * rgb[1]) + (0.0721750 * rgb[2]);
        XYZ[2] = (0.0193339 * rgb[0]) + (0.1191920 * rgb[1]) + (0.9503041 * rgb[2]);


        //XYZ in [0-1]
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
        V[i] = (rgbPixel[i] <= 0.0031308) ? 12.92*rgbPixel[i] : 1.055*std::pow(rgbPixel[i], 1.0/2.4) - 0.055;
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

        v[i] = (rgbPixel[i] <= 0.04045) ? rgbPixel[i] / 12.92 : std::pow( (rgbPixel[i] + 0.055) / 1.055, 2.4);

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
























