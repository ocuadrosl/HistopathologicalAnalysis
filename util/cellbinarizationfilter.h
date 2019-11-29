#ifndef CELLBINARIZATIONFILTER_H
#define CELLBINARIZATIONFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageToHistogramFilter.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

#include <numeric>
#include <algorithm>

#include "../util/vtkviewer.h"
#include "../util/math.h"
#include "../util/findpeaks.h"
#include "../util/persistence1d.hpp"

template<typename rgbImageT>
class CellBinarizationFilter
{

    using rgbImageP = typename rgbImageT::Pointer;


    using grayImageT = itk::Image<unsigned, 2>;
    using grayImageP =  grayImageT::Pointer;

    using longVectorT = std::vector<long int>;
    using uVectorT = std::vector<unsigned>;

    using derivativeVectorT = std::vector<std::pair<long int, unsigned>>;




public:
    CellBinarizationFilter();
    void compute();

    void setImage(rgbImageP inpoutImage);
    grayImageP getBinaryImage();
    grayImageP getBlurMaskImage();
    grayImageP getBlurImage();
    grayImageP getEqualizedImage();


private:

    //stl vector for conveniency
    longVectorT histogram;
    derivativeVectorT derivatives;
    uVectorT localMinimum;

    unsigned threshold = 100;
    float sigma = 5.f;

    const unsigned numberOfBins = 255;

    rgbImageP  inputImage;
    grayImageP grayImage;
    grayImageP blurImage;
    grayImageP eqImage;
    grayImageP binaryImage;
    grayImageP blurMaskImage;


    void computeHistogram();

    void histogramEqualization();

    void binaryThreholding();

    void gaussianBlur();

    void findThreshold();

    void interpolateZeros();
};




using uRGB = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellBinarizationFilter<uRGB>;

#endif // CELLBINARIZATIONFILTER_H
