#ifndef ROIEXTRACTOR_H
#define ROIEXTRACTOR_H

#include "itkImage.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkColormapFunction.h"

//testing include
#include "QuickView.h"



class ROIExtractor
{
public:

    using pixelType=unsigned char;
    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelType>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer  =   typename  rgbImageType::Pointer;

    //gray scale type alias
    using grayImageType = itk::Image< pixelType, 2>;
    using grayImagePointer = typename grayImageType::Pointer;

    ROIExtractor();

    void setImage(grayImagePointer inputImage);
    void setMaskSize(short maskSize);
    auto getColorMap() const;
    void extract();

private:

    //grayscale images
    grayImagePointer inputImage;
    grayImagePointer densityImage;

    //rgb image
    rgbImagePointer colorMap;

    short maskSize;

    grayImagePointer otsuThreshold();

    void applyColorMap();

};


#endif // ROIEXTRACTOR_H
