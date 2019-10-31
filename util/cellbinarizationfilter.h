#ifndef CELLBINARIZATIONFILTER_H
#define CELLBINARIZATIONFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageToHistogramFilter.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <numeric>
#include <algorithm>

template<typename rgbImageT>
class CellBinarizationFilter
{

    using rgbImageP = typename rgbImageT::Pointer;


    using grayImageT = itk::Image<unsigned,2>;
    using grayImageP =  grayImageT::Pointer;

    using longVectorT = std::vector<long int>;
    using uVectorT = std::vector<unsigned>;

    using derivativeVectorT = std::vector<std::pair<long int, unsigned>>;




public:
    CellBinarizationFilter();
    void compute();

    void setImage(rgbImageP inpoutImage);


private:

    //stl vector for conveniency
    longVectorT histogram;
    derivativeVectorT derivatives;
    uVectorT localMinimum;

    const unsigned numberOfBins = 255;

    rgbImageP  inputImage;
    grayImageP grayImage;


    void findLocalMinimum(unsigned number=2);
    void computeDerivatives();
    void computeHistogram();
};




using uRGB = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellBinarizationFilter<uRGB>;

#endif // CELLBINARIZATIONFILTER_H
