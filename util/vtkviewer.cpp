#include "vtkviewer.h"

template<typename imageT>
VTKViewer<imageT>::VTKViewer(){}

template<typename imageT>
void VTKViewer<imageT>::visualize(imageP image, std::string description)
{

    if constexpr(std::is_unsigned<pixelT>::value) // is gray-level
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
    else  if constexpr (std::is_unsigned<typename pixelT::ComponentType>::value)
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



template<typename imageT>
void VTKViewer<imageT>::visualizeVectorImage(imageP vectorImage)
{

    if constexpr(!std::is_unsigned<pixelT>::value) // is rgb or vector
    {
        if constexpr(std::is_floating_point<typename pixelT::ComponentType>::value)
        {
            std::cout<<"vector type"<<std::endl;

            //todo visualize here
            //use this
            //https://itk.org/Doxygen/html/SphinxExamples_2src_2Filtering_2ImageGradient_2ComputeAndDisplayGradient_2Code_8cxx-example.html#_a3

        }

    }


}
