#ifndef COLORCONVERTERFILTER_H
#define COLORCONVERTERFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkNumericTraitsArrayPixel.h>

#include <cmath>

//local includes
#include "../util/math.h"
#include "../util/customprint.h"
#include "../util/illuminant.h"



//template<typename inputPixelComponentT = unsigned int, typename outputPixelComponentT = double>
template <typename inputImageT, typename outputImageT>
class ColorConverterFilter
{
public:

    // RGB type  alias
    using inputPixelT = typename inputImageT::PixelType;
    using inputImageP = typename inputImageT::Pointer;

    // RGB type  alias
    using outputPixelT = typename outputImageT::PixelType;
    using outputPixelCompT  = typename outputPixelT::ComponentType;
    using outputImageP = typename outputImageT::Pointer;

    //double pixel type
    using pixelDouble = itk::RGBPixel<double>;

    //setters
    void setInput(inputImageP inputImage);
    void setWhite(unsigned illuminantIndex);

    //getters
    outputImageP getOutput();

    void rgbToHsv();
    void rgbToHsl();

    void rgbToXyz();
    void xyzToLab(); //CIE standard
    void labToXyz();
    void xyzToRgb();


    ColorConverterFilter();

private:

    inputImageP  inputImage;
    outputImageP outputImage;

    pixelDouble white;

    //LAB constants
    const double e = 0.008856; // epsilon constant CIE standard
    const double k = 903.3;    //kappa constant CIE standard
    const double g = 2.2;

    inline outputPixelT sRGBInverseCompanding (const outputPixelT& rgbPixel);
    inline outputPixelT lInverseCompanding    (const outputPixelT& rgbPixel); //L*
    inline outputPixelT gammaInverseCompanding(const outputPixelT& rgbPixel); //L*

    inline inputPixelT  sRGBCompanding (const inputPixelT& rgbPixel);
    inline inputPixelT  lCompanding    (const inputPixelT& rgbPixel); //L*
    inline inputPixelT  gammaCompanding(const inputPixelT& rgbPixel); //L*



};

using imageDouble   = itk::Image<itk::RGBPixel<double  >, 2>;
using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;
using imageFloat    = itk::Image<itk::RGBPixel<float   >, 2>;

template  class ColorConverterFilter<imageDouble  , imageDouble>;
template  class ColorConverterFilter<imageUnsigned, imageDouble>;
template  class ColorConverterFilter<imageDouble  , imageUnsigned>;
template  class ColorConverterFilter<imageUnsigned, imageFloat >;

#endif // COLORCONVERTERFILTER_H
