#ifndef RESCALERGBIMAGEFILTER_H
#define RESCALERGBIMAGEFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include "minmaxrgbimagecalculator.h"
#include "math.h"


/*
rescale rgb values

*/

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
    void setMinInputValue(const rgbInputPixelType& minValue);
    void setMaxInputValue(const rgbInputPixelType& maxValue);

    void setMinOutputValue(const rgbOutputPixelType& minValue);
    void setMaxOutputValue(const rgbOutputPixelType& maxValue);


    rgbOutputImagePointer getOutput() const;

    //no max min
    void rescaleAuto();
    void rescale();


private:


    rgbInputImagePointer inputImage;
    rgbOutputImagePointer outputImage;

    rgbInputPixelType minInputValue;
    rgbInputPixelType maxInputValue;

    rgbOutputPixelType minOutputValue;
    rgbOutputPixelType maxOutputValue;



};

template class RescaleRGBImageFilter<unsigned int, unsigned int>;
template class RescaleRGBImageFilter<unsigned char, unsigned char>;
template class RescaleRGBImageFilter<float, unsigned int>;
template class RescaleRGBImageFilter<unsigned int, float>;
template class RescaleRGBImageFilter<float, float>;



#endif // RESCALERGBIMAGEFILTER_H
