#ifndef RESCALERGBIMAGEFILTER_H
#define RESCALERGBIMAGEFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include "minmaxrgbimagecalculator.h"
#include "math.h"

template<typename inputPixelType, typename outputPixelType>
class RescaleRGBImageFilter
{
public:


    // RGB type  alias
    using rgbInputPixelType = itk::RGBPixel<inputPixelType>;
    using rgbInputImageType = itk::Image< rgbInputPixelType, 2 >;
    using rgbInputImagePointer  =   typename  rgbInputImageType::Pointer;

    // RGB type  alias
    using rgbOutputPixelType = itk::RGBPixel<outputPixelType>;
    using rgbOutputImageType = itk::Image< rgbOutputPixelType, 2 >;
    using rgbOutputImagePointer  =   typename  rgbOutputImageType::Pointer;


    RescaleRGBImageFilter();

    //setters
    void setInput(rgbInputImagePointer inputImage);
    void setMinValue(const rgbInputPixelType& minValue);
    void setMaxValue(const rgbInputPixelType& maxValue);


    rgbOutputImagePointer getOutput() const;

    void rescale();

private:


    rgbInputImagePointer inputImage;
    rgbOutputImagePointer outputImage;

    rgbInputPixelType minValue;
    rgbInputPixelType maxValue;



};

template class RescaleRGBImageFilter<unsigned int, unsigned int>;
template class RescaleRGBImageFilter<float, unsigned int>;
template class RescaleRGBImageFilter<unsigned int, float>;
template class RescaleRGBImageFilter<float, float>;



#endif // RESCALERGBIMAGEFILTER_H
