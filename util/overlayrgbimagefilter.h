#ifndef OVERLAYRGBIMAGEFILTER_H
#define OVERLAYRGBIMAGEFILTER_H

#include "itkImage.h"
#include "itkRGBPixel.h"
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include "rescalergbimagefilter.h"

template<typename imageT>
class OverlayRGBImageFilter
{
public:

    // RGB type  alias

    using rgbPixelT  = typename imageT::PixelType;
    using rgbImageP  = typename  imageT::Pointer;

    OverlayRGBImageFilter();

    //setters
    void setBackgroundImage(rgbImageP backGroundImage);
    void setForegroundImage(rgbImageP foreGroundImage);
    void setForegroundAlpha(float foregroundAlpha);

    //process
    void overlay();
    void softLigh();
    void alphaBlending();

    //getters
    rgbImageP getOutput() const;



private:

   rgbImageP backgroundImage;
   rgbImageP foregroundImage;
   rgbImageP outputImage;

   float backgroundAlpha = 1.0f;
   float foregroundAlpha = 0.5f;


   inline float blendMode(float background, float foreground,std::string mode);





};

template class OverlayRGBImageFilter< itk::Image< itk::RGBPixel<double>       , 2> >;
template class OverlayRGBImageFilter< itk::Image< itk::RGBPixel<unsigned int> , 2> >;
template class OverlayRGBImageFilter< itk::Image< itk::RGBPixel<unsigned char>, 2> >;

#endif // OVERLAYRGBIMAGEFILTER_H
