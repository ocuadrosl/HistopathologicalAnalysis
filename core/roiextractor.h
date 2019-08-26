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

#include "../util/math.h"
#include "../util/overlayrgbimagefilter.h"

//for testing
#include "../util/vtkviewer.h"


template<typename pixelType = unsigned int>
class ROIExtractor
{
public:

    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelType>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer  =   typename  rgbImageType::Pointer;

    //gray scale type alias
    using grayImageType = itk::Image< pixelType, 2>;
    using grayImagePointer = typename grayImageType::Pointer;


    //labelmap
    using labelObjectType = itk::LabelObject< pixelType, 2>;
    using labelMapType = itk::LabelMap< labelObjectType >;
    using labelMapPointer = typename labelMapType::Pointer;


    ROIExtractor();

    void setImage(rgbImagePointer inputImage);
    void setKernelSize(short kernelSize);
    auto getColorMap() const;
    void extract();

private:

    //rgb images
    rgbImagePointer inputImage;
    rgbImagePointer colorMapImage;

    grayImagePointer densityImage;


    short kernelSize;
    pixelType densityThreshold;

    grayImagePointer otsuThreshold();

    void densityToColorMap();
    void applyColorMap();

};

template class ROIExtractor<unsigned int>;
//template class ROIExtractor<unsigned char>;


#endif // ROIEXTRACTOR_H
