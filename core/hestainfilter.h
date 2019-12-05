#ifndef HESTAINFILTER_H
#define HESTAINFILTER_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include <itkAdaptiveHistogramEqualizationImageFilter.h>


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
    using rgbPixelU = itk::RGBPixel<inputPixelComponentT>;
    using rgbImageU = itk::Image< rgbPixelU, 2 >;
    using rgbImageUP = typename  rgbImageU::Pointer;

    using rgbPixelD = itk::RGBPixel<double>;
    using rgbImageD = itk::Image< rgbPixelD, 2 >;
    using rgbImageDP = typename  rgbImageD::Pointer;



    //gray scale type alias
    using grayImageT = itk::Image< inputPixelComponentT, 2>;
    using grayImageP = typename grayImageT::Pointer;

    //setters
    void setImage(rgbImageUP inputImage);
    void setGamma(double gamma);

    //getters

    rgbImageUP getOutput() const;

    void denoiseHSV(bool showResult = false);
    void denoiseLAB(bool showResult = false);



    void colorEnhancement(bool showResult = false);


    HEStainFilter();

private:

    rgbImageUP inputImage;
    rgbImageUP outputImage;
    void separateStainChannels();

    double gamma=1; //degree of lightness


    double hueThresholdYellow;
    double hueThresholdBlue;
    double saturationThreshold;
    double lightnessThreshold;

};

#endif // HESTAINFILTER_H
