#ifndef VTKVIEWER_H
#define VTKVIEWER_H


#include <itkImage.h>
#include <itkCastImageFilter.h>
#include <QuickView.h>
#include <itkCovariantVector.h>



template<typename imageT = itk::Image<itk::RGBPixel<unsigned>, 2>>
class VTKViewer
{
public:

    using pixelT = typename imageT::PixelType;
    using imageP = typename imageT::Pointer;




    VTKViewer();
    static void visualize(imageP image, std::string description="");

    static void visualizeVectorImage();




};


template class VTKViewer< itk::Image<itk::RGBPixel<unsigned>, 2> >;
template class VTKViewer< itk::Image<unsigned, 2> >;


#endif // VTKVIEWER_H

