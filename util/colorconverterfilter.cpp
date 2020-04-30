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
    constexpr auto  max = math::max<outputPixelT>;
    constexpr auto  min = math::min<outputPixelT>;
    constexpr auto  mod = math::mod<outputPixelCompT>;


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

           //std::cout<<hsvPixel<<std::endl;


        ++inputIt;
        ++outputIt;
    }


    io::printOK("RGB to HSV");

}



template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::hsvToRgb()
{

    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();


    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    double c; //chroma
    double h,x,m;
    inputPixelT hsv;
    inputPixelT rgb;

    while (!inputIt.IsAtEnd() )
    {

        hsv = inputIt.Get();

        //std::cout<<hsv<<std::endl;

        c = hsv[2] * hsv[1];

        h = hsv[0]/60;


        x = c * (1 - std::abs(math::mod<>(h, 2.0) - 1));


        if(h >=0 && h <= 1)
        {
            rgb[0] = c;
            rgb[1] = x;
            rgb[2] = 0;
        }
        else if (h > 1 && h <= 2)
        {
            rgb[0] = x;
            rgb[1] = c;
            rgb[2] = 0;
        }
        else if (h > 2 && h <= 3)
        {
            rgb[0] = 0;
            rgb[1] = c;
            rgb[2] = x;
        }
        else if (h > 3 && h <= 4)
        {
            rgb[0] = 0;
            rgb[1] = x;
            rgb[2] = c;
        }
        else if (h > 4 && h <= 5)
        {
            rgb[0] = x;
            rgb[1] = 0;
            rgb[2] = c;
        }
        else if (h > 5 && h <= 6)
        {
            rgb[0] = c;
            rgb[1] = 0;
            rgb[2] = x;
        }
        else
        {
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;
        }


        m = hsv[2]-c;

        rgb[0] += m;
        rgb[1] += m;
        rgb[2] += m;

        //std::cout<<rgb<<std::endl;
        outputIt.Set(rgb*255);



        ++inputIt;
        ++outputIt;
    }

     io::printOK("HSV to RGB");

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

        outputPixelT rgb;

        //template function alias
        constexpr auto  max = math::max<outputPixelT>;
        constexpr auto  min = math::min<outputPixelT>;
        constexpr auto  mod = math::mod<outputPixelCompT>;


        outputPixelT hslPixel = outputIt.Get();
        while (!inputIt.IsAtEnd() )
        {

            rgb = static_cast<outputPixelT>(inputIt.Get())/255.0;

            outputPixelCompT maxAux = max(rgb);
            outputPixelCompT minAux = min(rgb);
            outputPixelCompT delta =  maxAux - minAux;

            //Hue calculation

            outputPixelCompT h;

            if(delta == 0)
            {
                hslPixel[0] = 0;
            }
            else if (maxAux == rgb.GetRed()) //red
            {
                hslPixel[0] = 60 *  mod( (rgb[1] - rgb[2]) / delta, 6);
            }
            else if (maxAux == rgb.GetGreen()) //green
            {
                hslPixel[0] = 60 * (((rgb[2] - rgb[0])/delta ) + 2);
            }
            else if (maxAux == rgb.GetBlue()) //blue
            {
                hslPixel[0] = 60 * (((rgb[0] - rgb[1])/delta) + 4);
            }

            //Lightness
            hslPixel[2] = (maxAux + minAux) / 2;

            //Saturation
            hslPixel[1] = (delta == 0) ? 0 : delta / (1 - std::abs( 2*hslPixel[2]-1 ));


            outputIt.Set(hslPixel);

            ++inputIt;
            ++outputIt;
        }


        io::printOK("RGB to HSL");
    }
    else
    {
        io::printFail("RGB to HSL", "The output image type must be a float point type");

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

    io::printOK("LAB to XYZ");

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

    io::printOK("XYZ to CIE Lab");


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


        rgb[0] = ( 3.2404542 * XYZ[0]) + (-1.5371385 * XYZ[1]) + (-0.4985314 * XYZ[2]);
        rgb[1] = (-0.9692660 * XYZ[0]) + ( 1.8760108 * XYZ[1]) + ( 0.0415560 * XYZ[2]);
        rgb[2] = ( 0.0556434 * XYZ[0]) + (-0.2040259 * XYZ[1]) + ( 1.0572252 * XYZ[2]);


        rgb = sRGBCompanding(rgb);

        //ouput RGB in [0 - 255]
        outputPixel[0] = static_cast<outputPixelCompT>(std::floor(rgb[0] * 255));
        outputPixel[1] = static_cast<outputPixelCompT>(std::floor(rgb[1] * 255));
        outputPixel[2] = static_cast<outputPixelCompT>(std::floor(rgb[2] * 255));


        outputIt.Set(outputPixel);

        //std::cout<<XYZ<<" -> "<<outputPixel<<std::endl;

        ++inputIt;
        ++outputIt;

    }

    io::printOK("XYZ to RGB");

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

  io::printOK("RGB to XYZ");

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
    rgbPixel must be scaled in [0-1]
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
























