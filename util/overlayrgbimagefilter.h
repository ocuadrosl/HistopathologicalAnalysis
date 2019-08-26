#ifndef OVERLAYRGBIMAGEFILTER_H
#define OVERLAYRGBIMAGEFILTER_H

#include "itkImage.h"
#include "itkRGBPixel.h"
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

template<typename pixelType = unsigned int>
class OverlayRGBImageFilter
{
public:

    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelType>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer  =   typename  rgbImageType::Pointer;

    OverlayRGBImageFilter();

    //setters
    void setBackgroundImage(rgbImagePointer backGroundImage);
    void setForegroundImage(rgbImagePointer foreGroundImage);
    void setAlpha(float alpha);

    //process
    void overlay();
    void softLigh();

    //getters
    rgbImagePointer getOutput() const;



private:

   rgbImagePointer backgroundImage;
   rgbImagePointer foregroundImage;
   rgbImagePointer outputImage;

   float alpha;




};

template class OverlayRGBImageFilter<unsigned int>;
//template class OverlayRGBImageFilter<unsigned char>;

#endif // OVERLAYRGBIMAGEFILTER_H
