#include "vtkviewer.h"

template<typename pixelType>
VTKViewer<pixelType>::VTKViewer()
{

}

template<typename pixelType>
void VTKViewer<pixelType>::visualizeRGB(rgbImagePointer image, std::string description)
{
    using rgbPixelType = itk::RGBPixel<unsigned char>;
    using rgbImageChar = itk::Image< rgbPixelType, 2 >;

    using castFilterCastType = itk::CastImageFilter< rgbImageType, rgbImageChar >;
    typename castFilterCastType::Pointer castFilter = castFilterCastType::New();
    castFilter->SetInput( image );
    //castFilter->Update();

    //visualizing
    QuickView viewer;
    viewer.AddRGBImage(castFilter->GetOutput(), true, description);
    viewer.Visualize();

}

template<typename pixelType>
void VTKViewer<pixelType>::visualizeGray(grayImagePointer image, std::string description)
{


    using imageChar = itk::Image< unsigned char, 2 >;

    using castFilterCastType = itk::CastImageFilter< grayImageType, imageChar >;
    typename castFilterCastType::Pointer castFilter = castFilterCastType::New();
    castFilter->SetInput( image );
    //castFilter->Update();

    //visualizing
    QuickView viewer;
    viewer.AddImage(castFilter->GetOutput(), true, description);
    viewer.Visualize();

}
