#ifndef MINMAXRGBIMAGECALCULATOR_H
#define MINMAXRGBIMAGECALCULATOR_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>


template<typename pixelType>
class MinMaxRGBImageCalculator
{
public:

    // RGB type global alias
    using rgbInputPixelType = itk::RGBPixel<pixelType>;
    using rgbInputImageType = itk::Image< rgbInputPixelType, 2 >;
    using rgbInputImagePointer = typename  rgbInputImageType::Pointer;

    //getters
    void setInput(const rgbInputImagePointer inputImage);

    //process
    void calculate();

    //setters
    rgbInputPixelType getMinValue() const;
    rgbInputPixelType getMaxValue() const;


    MinMaxRGBImageCalculator();
    ~MinMaxRGBImageCalculator(){}

private:

    rgbInputImagePointer inputImage;
    rgbInputPixelType minOutputValue;
    rgbInputPixelType maxOutputValue;

};

template class MinMaxRGBImageCalculator<float>;
template class MinMaxRGBImageCalculator<unsigned int>;
template class MinMaxRGBImageCalculator<unsigned char>;


#endif // MINMAXRGBIMAGECALCULATOR_H
