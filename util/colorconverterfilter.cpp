#include "colorconverterfilter.h"


template <typename inputImageT, typename outputImageT>
ColorConverterFilter<inputImageT, outputImageT>::ColorConverterFilter()
{
     white = Illuminant::getWhitePoint(Illuminant::index::d50);

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



template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::rgbToHsl()
{

    if constexpr (std::is_floating_point<outputPixelComponentT>::value)
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


        outputPixelT hslPixel = outputIt.Get();
        while (!inputIt.IsAtEnd() )
        {

            pixelFloat = static_cast<outputPixelT>(inputIt.Get())/255.0;

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

    outputPixelT xyzPixel;
    outputPixelT xyzR; //r vector
    outputPixelT xyzF; //f vector

    inputPixelT labPixel;
    while (!inputIt.IsAtEnd() )
    {
        labPixel = inputIt.Get();

        //Do not change the order
        xyzF[1] = (labPixel[0] + 16.0)/116.0;
        xyzF[0] = (labPixel[1]/500.0) + xyzF[1];
        xyzF[2] = xyzF[1] - (labPixel[2]/200.0);

        //to avoid double computation
        double xyzXCube = std::pow(xyzF[0], 3.0);
        double xyzZCube = std::pow(xyzF[2], 3.0);
        double xyzYCube = std::pow( (labPixel[0]+16.0) / 116.0, 3.0);

        xyzR[0] = (xyzXCube > e)? xyzXCube : (116.0 * xyzF[0] - 16.0) / k;
        xyzR[1] = (labPixel[0] > k*e)? xyzYCube: labPixel[0] / k;
        xyzR[2] = ( xyzZCube > e)? xyzZCube : ( 116.0*xyzF[2]-16.0) / k;



        xyzPixel[0] = xyzR[0]*white[0];
        xyzPixel[1] = xyzR[1]*white[1];
        xyzPixel[2] = xyzR[2]*white[2];

        outputIt.Set(xyzPixel);

        ++inputIt;
        ++outputIt;

        //std::cout<<inputPixel<<" -> "<<xyzPixel<<std::endl;
    }

    IO::printOK("LAB to XYZ");




}


/*
using:
model: sRGB
white: D50
Gamma: sRGB
Adaptation: Bradford

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
    outputPixelT labPixel;

    inputPixelT pixelAux;
    while (!inputIt.IsAtEnd() )
    {
        pixelAux = inputIt.Get();

        xyzR[0] =  pixelAux[0] / white[0];
        xyzR[1] =  pixelAux[1] / white[1];
        xyzR[2] =  pixelAux[2] / white[2];

        xyzF[0] = (xyzR[0] > e) ? std::cbrt(xyzR[0]) : (k * xyzR[0] + 16) / 116.0;
        xyzF[1] = (xyzR[1] > e) ? std::cbrt(xyzR[1]) : (k * xyzR[1] + 16) / 116.0;
        xyzF[2] = (xyzR[2] > e) ? std::cbrt(xyzR[2]) : (k * xyzR[2] + 16) / 116.0;

        labPixel[0] = 116 *  xyzF[0] - 16;
        labPixel[1] = 500 * (xyzF[0] - xyzF[1]);
        labPixel[2] = 200 * (xyzF[1] - xyzF[2]);

        outputIt.Set(labPixel);

        ++inputIt;
        ++outputIt;

    }

    IO::printOK("XYZ to LAB");


}


/*
USING:
sRGB model
D50  white
sRGB Gamma
*/
template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>:: xyzToRgb()
{


    //TODO bug here -> orange color!!!
    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage  , inputImage ->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());

    inputPixelT pixelRGB; //rgb in v
    inputPixelT pixelXYZ;

    outputPixelT outputPixel;

    while (!inputIt.IsAtEnd() )
    {

        pixelXYZ = inputIt.Get();

        pixelRGB[0] = ( 3.1338561 * pixelXYZ[0]) + (-1.6168667 * pixelXYZ[1]) + (-0.4906146 * pixelXYZ[2]);
        pixelRGB[1] = (-0.9787684 * pixelXYZ[0]) + ( 1.9161415 * pixelXYZ[1]) + ( 0.0334540 * pixelXYZ[2]);
        pixelRGB[2] = ( 0.0719453 * pixelXYZ[0]) + (-0.2289914 * pixelXYZ[1]) + ( 1.4052427 * pixelXYZ[2]);


        //std::cout<<pixelXYZ<<" -> "<<sRGBCompanding(pixelRGB)<<std::endl;

        pixelRGB = sRGBCompanding(pixelRGB);
        outputPixel[0] = static_cast<outputPixelComponentT>(std::round(pixelRGB[0]*255.0));
        outputPixel[1] = static_cast<outputPixelComponentT>(std::round(pixelRGB[1]*255.0));
        outputPixel[2] = static_cast<outputPixelComponentT>(std::round(pixelRGB[2]*255.0));

        std::cout<<pixelXYZ<<" -> "<<outputPixel<<std::endl;

        outputIt.Set(outputPixel);


        ++inputIt;
        ++outputIt;

    }


    IO::printOK("XYZ to RGB");

}



/*
USING:
sRGB model
D50  white
sRGB Gamma
*/
template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::rgbToXyz()
{

    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< inputImageT > inputIt(inputImage ,  inputImage->GetRequestedRegion());
    itk::ImageRegionIterator     < outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());


    outputPixelT pixelAux;
    outputPixelT pixelXYZ;


    while (!inputIt.IsAtEnd() )
    {

        pixelAux = sRGBInverseCompanding(static_cast<outputPixelT>(inputIt.Get())/255.0);

        pixelXYZ[0] = (0.4360747 * pixelAux[0]) + (0.3850649 * pixelAux[1]) + (0.1430804 * pixelAux[2]);
        pixelXYZ[1] = (0.2225045 * pixelAux[0]) + (0.7168786 * pixelAux[1]) + (0.0606169 * pixelAux[2]);
        pixelXYZ[2] = (0.0139322 * pixelAux[0]) + (0.0971045 * pixelAux[1]) + (0.7141733 * pixelAux[2]);

        outputIt.Set(pixelXYZ);

        ++inputIt;
        ++outputIt;

    }

  IO::printOK("RGB to XYZ");

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

        v[i] = (rgbPixel[i] <= 0.04045)? rgbPixel[i]/12.92 : std::pow((rgbPixel[i] +0.055)/1.055, 2.4);

    }

    return v;

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


template <typename inputImageT, typename outputImageT>
void ColorConverterFilter<inputImageT, outputImageT>::setWhite(unsigned illuminantIndex)
{
    white = Illuminant::getWhitePoint(illuminantIndex);
}
























