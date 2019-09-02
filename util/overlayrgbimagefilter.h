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

    using rgbPixelT     = typename imageT::PixelType;
    using rgbImageP  = typename  imageT::Pointer;

    OverlayRGBImageFilter();

    //setters
    void setBackgroundImage(rgbImageP backGroundImage);
    void setForegroundImage(rgbImageP foreGroundImage);
    void setAlpha(double alpha);

    //process
    void overlay();
    void softLigh();

    //getters
    rgbImageP getOutput() const;



private:

   rgbImageP backgroundImage;
   rgbImageP foregroundImage;
   rgbImageP outputImage;

   double alpha;




};

template class OverlayRGBImageFilter< itk::Image< itk::RGBPixel<double>       , 2> >;
template class OverlayRGBImageFilter< itk::Image< itk::RGBPixel<unsigned int> , 2> >;
template class OverlayRGBImageFilter< itk::Image< itk::RGBPixel<unsigned char>, 2> >;

#endif // OVERLAYRGBIMAGEFILTER_H
