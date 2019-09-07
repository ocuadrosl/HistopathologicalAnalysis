#ifndef RGBHISTOGRAMFILTER_H
#define RGBHISTOGRAMFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <vector>
#include <itkImageRegionConstIterator.h>


//local incldues
#include "../util/customprint.h"
#include "../util/minmaxrgbimagecalculator.h"
#include "../util/math.h"

template< typename rgbImageT >
class RGBHistogramFilter
{
public:

    using pixelT     = typename rgbImageT::PixelType;
    using pixelCompT = typename pixelT   ::ComponentType;
    using imageP     = typename rgbImageT::Pointer;

    using histogramT = std::vector<double>;
    using rgbHistogramT = std::vector<histogramT>;


    //setters
    void setImage(imageP inputImage);
    void setNumberOfBins(unsigned red, unsigned green, unsigned blue);
    void setMaxPossibleValues(const pixelT& max);
    void setMinPossibleValues(const pixelT& min);
    void setMaxPossibleValues(pixelCompT r, pixelCompT g, pixelCompT b);
    void setMinPossibleValues(pixelCompT r, pixelCompT g, pixelCompT b);

    //getters
    rgbHistogramT getHistogram();
    rgbHistogramT getCumulativeDistribution();

    void computeHistogram(bool normalized = false);
    void computeComulativeDistribution();

    void normalizeHistogram();

    RGBHistogramFilter();

private:

    imageP  inputImage;

    rgbHistogramT rgbHistogram;
    rgbHistogramT rgbCumulativeDistribution;


    unsigned nBinsRed   = 0;
    unsigned nBinsGreen = 0;
    unsigned nBinsBlue  = 0;

    pixelT minPossibleValues;
    pixelT maxPossibleValues;









};
using imageDouble   = itk::Image<itk::RGBPixel<double  >, 2>;
using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;

template class RGBHistogramFilter<imageDouble>;
template class RGBHistogramFilter<imageUnsigned>;


#endif // RGBHISTOGRAMFILTER_H
