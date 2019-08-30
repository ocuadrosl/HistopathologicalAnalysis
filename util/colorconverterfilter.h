#ifndef COLORCONVERTERFILTER_H
#define COLORCONVERTERFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkNumericTraitsArrayPixel.h>

#include <cmath>

//local includes
#include "../util/math.h"
#include "../util/customprint.h"


template<typename inputPixelComponentT  = unsigned int, typename outputPixelComponentT = float>
class ColorConverterFilter
{
public:


    // RGB type  alias
    using inputPixelT = itk::RGBPixel<inputPixelComponentT>;
    using inputImageT = itk::Image< inputPixelT, 2 >;
    using inputImageP =   typename  inputImageT::Pointer;

    // RGB type  alias
    using outputPixelT = itk::RGBPixel<outputPixelComponentT>;
    using outputImageT = itk::Image< outputPixelT, 2 >;
    using outputImageP =   typename  outputImageT::Pointer;


    //setters

    void setInput(inputImageP inputImage);

    //getters

    outputImageP getOutput();

    void rgbToHsv();
     void rgbToHsl();


    ColorConverterFilter();


private:

    inputImageP  inputImage;
    outputImageP outputImage;

};



template  class ColorConverterFilter<unsigned int, float>;
template  class ColorConverterFilter<unsigned int, double>;

#endif // COLORCONVERTERFILTER_H