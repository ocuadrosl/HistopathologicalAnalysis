#ifndef LABELMAPTOMULTIPLERGBIMAGESFILTER_H
#define LABELMAPTOMULTIPLERGBIMAGESFILTER_H


#include <itkImage.h>
#include <itkRGBPixel.h>


/*
   Create a RGB image for each labelObject in the labelMap
   //TODO implement this...
*/
template<typename pixelComponentT = unsigned int>
class LabelMapToMultipleRGBImagesFilter
{
public:

    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelComponentT>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer  =   typename  rgbImageType::Pointer;

    LabelMapToMultipleRGBImagesFilter();
};

template class LabelMapToMultipleRGBImagesFilter<unsigned int>;
template class LabelMapToMultipleRGBImagesFilter<float>;


#endif // LABELMAPTOMULTIPLERGBIMAGESFILTER_H
