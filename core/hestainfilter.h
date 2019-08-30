#ifndef HESTAINFILTER_H
#define HESTAINFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

//for testing
#include "../util/vtkviewer.h"
#include "../util/colorconverterfilter.h"

/*
 * **Haematoxylin and Eosin stain H&E filter**
 * -Noise filter: using the HSL color model
 * -Color normalization
 *
*/

//Only RGB images supported
template<typename inputPixelComponentT=unsigned int>
class HEStainFilter
{
public:


    // RGB type  alias
    using rgbInputPixelT = itk::RGBPixel<inputPixelComponentT>;
    using rgbInputImageT = itk::Image< rgbInputPixelT, 2 >;
    using rgbInputImageP = typename  rgbInputImageT::Pointer;


    //gray scale type alias
    using grayImageT = itk::Image< inputPixelComponentT, 2>;
    using grayImageP = typename grayImageT::Pointer;

    //setters
    void setImage(rgbInputImageP inputImage);

    //getters

    rgbInputImageP getOutput() const;

    void denoise(bool showResult = false);


    HEStainFilter();

private:

    rgbInputImageP inputImage;
    rgbInputImageP outputImage;
    void separateStainChannels();


    double hueThresholdRed;
    double hueThresholdBlue;
    double saturationThreshold;
    double lightnessThreshold;




};

template  class HEStainFilter<unsigned int>;

#endif // HESTAINFILTER_H