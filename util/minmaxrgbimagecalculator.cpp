#include "minmaxrgbimagecalculator.h"

template<typename imageT>
MinMaxRGBImageCalculator<imageT>::MinMaxRGBImageCalculator()
{

}


template<typename imageT>
void MinMaxRGBImageCalculator<imageT>::setInput(const rgbImageP inputImage)
{
    this->inputImage = inputImage;

}


template<typename imageT>
void MinMaxRGBImageCalculator<imageT>::calculate()
{

    itk::ImageRegionConstIterator< imageT > it(inputImage, inputImage->GetRequestedRegion());

    rgbPixelT value;


    //assign possible min and max values
    minOutputValue = it.Get();
    maxOutputValue.Fill(static_cast<pixelCompT>(0));

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

template<typename imageT>
typename MinMaxRGBImageCalculator<imageT>::rgbPixelT
MinMaxRGBImageCalculator<imageT>::getMinValue() const
{
    return minOutputValue;
}
template<typename imageT>
typename MinMaxRGBImageCalculator<imageT>::rgbPixelT
MinMaxRGBImageCalculator<imageT>::getMaxValue() const
{
    return maxOutputValue;
}



