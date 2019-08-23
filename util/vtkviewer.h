#ifndef VTKVIEWER_H
#define VTKVIEWER_H


#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "QuickView.h"



template<typename pixelType= unsigned int>
class VTKViewer
{
public:

    using rgbPixelType = itk::RGBPixel<pixelType>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImagePointer = typename  rgbImageType::Pointer;

    using grayImageType = itk::Image< pixelType, 2 >;
    using grayImagePointer = typename  grayImageType::Pointer;

    VTKViewer();

    static void visualizeRGB(rgbImagePointer image, std::string description="");
    static void visualizeGray(grayImagePointer image, std::string description="");

};


template class VTKViewer<unsigned int>;
template class VTKViewer<unsigned char>;


#endif // VTKVIEWER_H

