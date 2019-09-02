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



//template<typename inputPixelComponentT = unsigned int, typename outputPixelComponentT = double>
template <typename inputImageT, typename outputImageT>
class ColorConverterFilter
{
public:


    // RGB type  alias
    using inputPixelT = typename inputImageT::PixelType;
    using inputImageP = typename inputImageT::Pointer;

    // RGB type  alias
    using outputPixelT = typename outputImageT::PixelType;
    using outputPixelComponentT  = typename outputPixelT::ComponentType;
    using outputImageP = typename outputImageT::Pointer;


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

template  class ColorConverterFilter<itk::Image<itk::RGBPixel<unsigned>, 2>, itk::Image<itk::RGBPixel<double>, 2>>;
template  class ColorConverterFilter<itk::Image<itk::RGBPixel<unsigned>, 2>, itk::Image<itk::RGBPixel<float>, 2>>;

#endif // COLORCONVERTERFILTER_H
