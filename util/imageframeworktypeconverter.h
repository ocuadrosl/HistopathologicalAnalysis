#ifndef IMAGEFRAMEWORKTYPECONVERTER_H
#define IMAGEFRAMEWORKTYPECONVERTER_H

#include <itkImage.h>
#include <itkImageRegionConstIteratorWithIndex.h>

#include <dlib/array2d.h>
#include <dlib/gui_widgets.h>
#include <dlib/pixel.h>


#include "inputOutput.h"


namespace ifc //image framework converter
{

template <typename DlibPixelT, typename ITKPixelT>
inline  void DlibToITK(const typename dlib::array2d<DlibPixelT>& dlibImage, typename itk::Image<ITKPixelT>::Pointer& itkImage)
{

    using itkImageT  = itk::Image<ITKPixelT,2>;


    unsigned cols = dlibImage.nc();
    unsigned rows  = dlibImage.nr();

    itkImage = itkImageT::New();
    typename itkImageT::RegionType region;
    region.SetSize({ cols, rows });
    itkImage->SetRegions(region);
    itkImage->Allocate();


    if constexpr(std::is_arithmetic<ITKPixelT>::value &&  std::is_arithmetic<DlibPixelT>::value) // is gray-level os scalar type
    {

        for(unsigned row = 0; row < dlibImage.nr(); ++row)
        {
            for(unsigned col = 0; col < dlibImage.nc(); ++col)
            {

                itkImage->SetPixel({col,row}, static_cast<ITKPixelT>(dlibImage[row][col]));

            }

        }
    }
    else
    {

        for(unsigned row = 0; row < dlibImage.nr(); ++row)
        {
            for(unsigned col = 0; col < dlibImage.nc(); ++col)
            {

                 const auto & rgbPixel = dlibImage[row][col];

                 const auto red   = static_cast<typename ITKPixelT::ComponentType>(rgbPixel.red  );
                 const auto green = static_cast<typename ITKPixelT::ComponentType>(rgbPixel.green);
                 const auto blue  = static_cast<typename ITKPixelT::ComponentType>(rgbPixel.blue );

                 ITKPixelT pixel;
                 pixel.Set(red, green, blue);
                 itkImage->SetPixel({col,row}, pixel);

            }

        }

    }

    io::printOK("Dlib image to ITK image");


}





template <typename ITKPixelT, typename DlibPixelT>
inline  void ITKToDlib(const typename itk::Image<ITKPixelT>::Pointer itkImage, typename dlib::array2d<DlibPixelT>& dlibImage)
{

    using itkImageT  = itk::Image<ITKPixelT,2>;
    using dlibImageT = dlib::array2d<DlibPixelT>;

    auto itkImageSize = itkImage->GetRequestedRegion().GetSize(); //cols, rows

    dlibImage.set_size(itkImageSize[1], itkImageSize[0]); //rows, cols

    itk::ImageRegionConstIteratorWithIndex<itkImageT> itkIt(itkImage, itkImage->GetRequestedRegion());


    if constexpr(std::is_arithmetic<ITKPixelT>::value &&  std::is_arithmetic<DlibPixelT>::value) // is gray-level os scalar type
    {
        for(;!itkIt.IsAtEnd(); ++itkIt)
        {
            const auto index = itkIt.GetIndex();
            dlibImage[index[1]][index[0]] = static_cast<DlibPixelT>(itkIt.Get());

        }
    }
    else //WARNING a cast to unsigned char is performed here
    {
        for(;!itkIt.IsAtEnd(); ++itkIt)
        {
            const auto index = itkIt.GetIndex();
            const auto pixel = itkIt.Get();
            dlibImage[index[1]][index[0]].red   = static_cast<unsigned char>(pixel[0]);
            dlibImage[index[1]][index[0]].green = static_cast<unsigned char>(pixel[1]);
            dlibImage[index[1]][index[0]].blue  = static_cast<unsigned char>(pixel[2]);

        }


    }


    io::printOK("ITK image to DLib image");

}


};


#endif // IMAGEFRAMEWORKTYPECONVERTER_H
