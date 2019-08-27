#ifndef ROIEXTRACTOR_H
#define ROIEXTRACTOR_H

#include "itkImage.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkColormapFunction.h"
#include "itkRGBToLuminanceImageFilter.h"
#include <itkLabelObject.h>
#include <itkLabelMap.h>
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapOverlayImageFilter.h"
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelToRGBImageFilter.h>
#include <itkLabelImageToLabelMapFilter.h>

//local includes
#include "../util/math.h"
#include "../util/overlayrgbimagefilter.h"
#include "../util/labelmaptomultiplegrayimagesfilter.h"


//for testing
#include "../util/vtkviewer.h"

/*

suffix T = Type

*/
template<typename pixelComponentT = unsigned int>
class ROIExtractor
{
public:

    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelComponentT>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer  =   typename  rgbImageType::Pointer;

    //gray scale type alias
    using grayImageType = itk::Image< pixelComponentT, 2>;
    using grayImagePointer = typename grayImageType::Pointer;

    //const values for otsu
    const pixelComponentT BACKGROUND=255;
    const pixelComponentT FOREGROUND=0;


    ROIExtractor();

    //setters
    void setImage(rgbImagePointer inputImage);
    void setKernelSize(short kernelSize);
    void setDensityThreshold(pixelComponentT threshold);

    auto getColorMap() const;
    void extract();

private:

    //rgb images
    rgbImagePointer inputImage;
    rgbImagePointer colorMapImage;

    grayImagePointer densityImage;

    short kernelSize;
    pixelComponentT densityThreshold;

    //ROIs
    //std::vector<grayImagePointer> highDensitySubImages;

    grayImagePointer otsuThreshold();

    void densityToColorMap();
    void blendColorMap();

    void connectedComponents();

    void divideDensityIntoHighAndLow(grayImagePointer &highDensity, grayImagePointer &lowDensity);


};

template class ROIExtractor<unsigned int>;
//template class ROIExtractor<unsigned char>;


#endif // ROIEXTRACTOR_H
