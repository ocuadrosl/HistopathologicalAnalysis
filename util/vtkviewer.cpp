#include "vtkviewer.h"

template<typename imageT>
VTKViewer<imageT>::VTKViewer(){}

template<typename imageT>
void VTKViewer<imageT>::visualize(imageP image, std::string description)
{

    if constexpr(std::is_scalar<pixelT>::value) // is gray-level
    {

        using imageChar = itk::Image< unsigned char, 2 >;
        using castFilterCastType = itk::CastImageFilter< imageT, imageChar >;
        typename castFilterCastType::Pointer castFilter = castFilterCastType::New();
        castFilter->SetInput( image );
        castFilter->Update();

        //visualizing
        QuickView viewer;
        viewer.AddImage(castFilter->GetOutput(), true, description);
        viewer.Visualize();
    }
    else  //is RGB
    {

        using rgbPixelType = itk::RGBPixel<unsigned char>;
        using rgbImageChar = itk::Image< rgbPixelType, 2 >;
        using castFilterCastType = itk::CastImageFilter< imageT, rgbImageChar >;
        typename castFilterCastType::Pointer castFilter = castFilterCastType::New();
        castFilter->SetInput( image );

        //visualizing
        QuickView viewer;
        viewer.AddRGBImage(castFilter->GetOutput(), true, description);
        viewer.Visualize();

    }
}
