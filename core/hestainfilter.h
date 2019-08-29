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
 * -Noise filter
 * -Color normalization
 *
*/

//Only RGB images supported
template<typename inputPixeComponentT=unsigned int, typename outputPixeComponentT=unsigned int >
class HEStainFilter
{
public:


    // RGB type  alias
    using rgbInputPixelT = itk::RGBPixel<inputPixeComponentT>;
    using rgbInputImageT = itk::Image< rgbInputPixelT, 2 >;
    using rgbInputImageP = typename  rgbInputImageT::Pointer;


    //gray scale type alias
    using grayImageT = itk::Image< inputPixeComponentT, 2>;
    using grayImageP = typename grayImageT::Pointer;

    //setters
    void setImage(rgbInputImageP inputImage);

    void denoise(bool showResult = false);


    HEStainFilter();

private:

    rgbInputImageP inputImage;
    void separateStainChannels();



};

template  class HEStainFilter<unsigned int, unsigned int>;

#endif // HESTAINFILTER_H
