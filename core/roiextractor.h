#ifndef ROIEXTRACTOR_H
#define ROIEXTRACTOR_H

#include "itkImage.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "QuickView.h"

class ROIExtractor
{
public:

    using pixelType = unsigned int;
    using grayImageType = itk::Image<pixelType, 2>;


    //rgb image type
    using rgbPixelType = itk::RGBPixel< unsigned char >;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;

    ROIExtractor();

    void setImage(grayImageType::Pointer inputImage);
    rgbImageType::Pointer getColorMapImage() const;
    void process();

private:
    grayImageType::Pointer image;
    rgbImageType::Pointer colorMap;

    grayImageType::Pointer otsuThreshold();

};

#endif // ROIEXTRACTOR_H
