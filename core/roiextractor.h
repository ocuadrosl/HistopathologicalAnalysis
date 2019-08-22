#ifndef ROIEXTRACTOR_H
#define ROIEXTRACTOR_H

#include "itkImage.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"

//testing include
#include "QuickView.h"


template<typename grayImageType=itk::Image<unsigned int,2>>
class ROIExtractor
{
public:


    //rgb image type, used to create the color map
    using pixelType = typename grayImageType::PixelType;
    using rgbPixelType = itk::RGBPixel<pixelType>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;

    //pointers
    using rgbImagePointer  = typename  rgbImageType::Pointer;
    using grayImagePointer = typename grayImageType::Pointer;

    ROIExtractor();

    void setImage(grayImagePointer inputImage);
    void setMaskSize(short maskSize);
    auto getColorMap() const;
    void process();

private:
    grayImagePointer inputImage;
    grayImagePointer densityImage;
    rgbImagePointer colorMap;

    short maskSize;

    grayImagePointer otsuThreshold();

    void applyColorMap();

};

//Explicit instantiation
template class ROIExtractor<itk::Image<unsigned int, 2>>;
template class ROIExtractor<itk::Image<unsigned char,2>>;

#endif // ROIEXTRACTOR_H
