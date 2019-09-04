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

class HEStainFilter
{
public:


    using inputPixelComponentT=unsigned int;

    // RGB type  alias
    using rgbInputPixelT = itk::RGBPixel<inputPixelComponentT>;
    using rgbInputImageT = itk::Image< rgbInputPixelT, 2 >;
    using rgbInputImageP = typename  rgbInputImageT::Pointer;

    using rgbOutputPixelT = itk::RGBPixel<double>;
    using rgbOutputImageT = itk::Image< rgbOutputPixelT, 2 >;
    using rgbOutputImageP = typename  rgbOutputImageT::Pointer;



    //gray scale type alias
    using grayImageT = itk::Image< inputPixelComponentT, 2>;
    using grayImageP = typename grayImageT::Pointer;

    //setters
    void setImage(rgbInputImageP inputImage);

    //getters

    rgbInputImageP getOutput() const;

    void denoiseHSL(bool showResult = false);
    void denoiseLAB(bool showResult = false);

    void colorCorrection(bool showResult = false);


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

#endif // HESTAINFILTER_H
