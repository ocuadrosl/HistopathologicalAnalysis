#ifndef CELLBINARIZATIONFILTER_H
#define CELLBINARIZATIONFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkHistogram.h>

template<typename rgbImageT>
class CellBinarizationFilter
{

    using rgbImageP = typename rgbImageT::Pointer;


public:
    CellBinarizationFilter();

    void setImage(rgbImageP inpoutImage);


private:

    void computeHistogram();

    const unsigned numberOfBins = 3;

    rgbImageP inputImage;


};




using uRGB = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellBinarizationFilter<uRGB>;

#endif // CELLBINARIZATIONFILTER_H
