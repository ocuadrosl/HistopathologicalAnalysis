#ifndef MINMAXRGBIMAGECALCULATOR_H
#define MINMAXRGBIMAGECALCULATOR_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>



/*


*/
template<typename imageT>
class MinMaxRGBImageCalculator
{
public:

    //Suffixes: T = type, P = pointer, Comp = Component

    using rgbPixelT  = typename imageT::PixelType;
    using pixelCompT = typename rgbPixelT::ComponentType;
    using rgbImageP  = typename imageT::Pointer;

    //getters
    void setInput(const rgbImageP inputImage);

    //process
    void calculate();

    //setters
    rgbPixelT getMinValue() const;
    rgbPixelT getMaxValue() const;


    MinMaxRGBImageCalculator();
    ~MinMaxRGBImageCalculator(){}

private:

    rgbImageP inputImage;
    rgbPixelT minOutputValue;
    rgbPixelT maxOutputValue;

};

template class MinMaxRGBImageCalculator< itk::Image< itk::RGBPixel<double>       , 2> >;
template class MinMaxRGBImageCalculator< itk::Image< itk::RGBPixel<unsigned int> , 2> >;
template class MinMaxRGBImageCalculator< itk::Image< itk::RGBPixel<unsigned char>, 2> >;



#endif // MINMAXRGBIMAGECALCULATOR_H
