#include "minmaxrgbimagecalculator.h"

template<typename pixelType>
MinMaxRGBImageCalculator<pixelType>::MinMaxRGBImageCalculator()
{

}


template<typename pixelType>
void MinMaxRGBImageCalculator<pixelType>::setInput(const rgbInputImagePointer inputImage)
{
    this->inputImage = inputImage;

}


template<typename pixelType>
void MinMaxRGBImageCalculator<pixelType>::calculate()
{

    itk::ImageRegionConstIterator< rgbInputImageType > it(inputImage, inputImage->GetRequestedRegion());

    rgbInputPixelType value;


    //assign possible min and max values
    minOutputValue = it.Get();
    maxOutputValue.Fill(static_cast<pixelType>(0));

    while (!it.IsAtEnd() )
    {
         value = it.Get();
         //min
         minOutputValue[0] = ( minOutputValue[0] > value[0]) ? value[0] : minOutputValue[0];
         minOutputValue[1] = ( minOutputValue[1] > value[1]) ? value[1] : minOutputValue[1];
         minOutputValue[2] = ( minOutputValue[2] > value[2]) ? value[2] : minOutputValue[2];

         //max
         maxOutputValue[0] = ( maxOutputValue[0] < value[0]) ? value[0] : maxOutputValue[0];
         maxOutputValue[1] = ( maxOutputValue[1] < value[1]) ? value[1] : maxOutputValue[1];
         maxOutputValue[2] = ( maxOutputValue[2] < value[2]) ? value[2] : maxOutputValue[2];

        ++it;
    }


}

template<typename pixelType>
typename MinMaxRGBImageCalculator<pixelType>::rgbInputPixelType
MinMaxRGBImageCalculator<pixelType>::getMinValue() const
{
    return minOutputValue;
}
template<typename pixelType>
typename MinMaxRGBImageCalculator<pixelType>::rgbInputPixelType
MinMaxRGBImageCalculator<pixelType>::getMaxValue() const
{
    return maxOutputValue;
}



