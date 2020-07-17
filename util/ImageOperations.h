#ifndef IMAGEOPERATIONS_H
#define IMAGEOPERATIONS_H

#include <vector>

//Dlib includes
#include <dlib/array2d.h>
#include <dlib/image_transforms.h>
#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkLabelMapToRGBImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkImageDuplicator.h>

#include "vtkviewer.h"

namespace util
{





template <typename LabelMapT,  typename GrayImageT>
inline typename GrayImageT::Pointer LabelMapToBinaryImage(const typename LabelMapT::Pointer& labelMap,  unsigned insideValue=255, unsigned outsideValue=0, bool show=false)
{


    //Label-map to RGB image
    using  rgbImageT =  itk::Image<itk::RGBPixel<unsigned char>, 2>;
    typedef itk::LabelMapToRGBImageFilter<LabelMapT, rgbImageT> RGBFilterType;
    typename RGBFilterType::Pointer labelMapToRGBFilter = RGBFilterType::New();
    labelMapToRGBFilter->SetInput(labelMap);
    labelMapToRGBFilter->Update();


    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter<rgbImageT, GrayImageT>;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(labelMapToRGBFilter->GetOutput());
    rgbToGrayFilter->Update();


    using FilterType = itk::BinaryThresholdImageFilter<GrayImageT, GrayImageT>;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(rgbToGrayFilter->GetOutput());
    filter->SetLowerThreshold(0);
    filter->SetUpperThreshold(1); //all values greater than black->0
    filter->SetOutsideValue(outsideValue);
    filter->SetInsideValue(insideValue);
    filter->Update();

    if(show)
    {

        using RescaleType = itk::RescaleIntensityImageFilter<GrayImageT, GrayImageT>;
        typename RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(filter->GetOutput());
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(255);
        rescaler->Update();

        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput(), "Label Map to Gray Image");
    }

    return filter->GetOutput();

}



template <typename ImageT>
inline  void PaintRegion(typename ImageT::Pointer& image,
                         typename ImageT::RegionType& region,
                         const typename ImageT::PixelType& color)
{


    itk::ImageRegionIterator<ImageT>  it(image, region); //output it


    for(it; !it.IsAtEnd(); ++it)
    {

        it.Set(color);

    }



}

template <typename ImageT>
inline unsigned ExtractNeighborhoodITK(const typename ImageT::Pointer& inputImage,
                                       const typename ImageT::IndexType& centerIndex,
                                       const unsigned& neighborhoodSize,
                                       typename ImageT::Pointer& neighbohood)
{


    if(neighborhoodSize % 2 == 0)
    {
        std::cerr<<"ERROR: neighborhood must be odd"<<std::endl;
        return 0;
    }

    const unsigned& colum = centerIndex[0];
    const unsigned& row   = centerIndex[1];

    const auto imageSize = inputImage->GetRequestedRegion().GetSize();

    const auto imageRowSize = imageSize[1];
    const auto imageColSize = imageSize[0];


    unsigned  neighSizeCenter = neighborhoodSize/2;

    //Define begin index
    unsigned  imageRowBegin = (neighSizeCenter > row  ) ? 0 : row   - neighSizeCenter;
    unsigned  imageColBegin = (neighSizeCenter > colum) ? 0 : colum - neighSizeCenter;



    //Define actual neighbprhood size

    unsigned imageRowEnd = (row   + neighSizeCenter >= imageRowSize) ? imageRowSize :  row   + neighSizeCenter+1;
    unsigned imageColEnd = (colum + neighSizeCenter >= imageColSize) ? imageColSize :  colum + neighSizeCenter+1;



    //std::cout<<imageColEnd<<" -> "<<imageRowEnd<<std::endl;


    neighbohood =  ImageT::New();
    typename ImageT::RegionType region;
    region.SetSize({imageColEnd - imageColBegin, imageRowEnd - imageRowBegin});
    neighbohood->SetRegions(region);
    neighbohood->Allocate();


    //this is a slow version reimplement it using iterators
    unsigned roiR=0;
    for(unsigned r = imageRowBegin; r < imageRowEnd ; ++r)
    {
        unsigned roiC=0;
        for(unsigned c = imageColBegin; c < imageColEnd; ++c)
        {
            neighbohood->SetPixel({roiC++, roiR}, inputImage->GetPixel( {c, r} ));

        }
        ++roiR;
    }

    return 1;



}


/*
ImageT is a Dlib image type
index = col, row
*/
template <typename ImageT, typename IndexT = std::vector<unsigned long>>
inline unsigned ExtractNeighborhood(const ImageT& inputImage, const IndexT& centerIndex , const unsigned& neighborhoodSize, ImageT& roi)
{

    if(neighborhoodSize % 2 == 0)
    {
        std::cerr<<"ERROR: neighborhood must be odd"<<std::endl;
        return 0;
    }

    const unsigned& colum = centerIndex[0];
    const unsigned& row   = centerIndex[1];

    const auto imageRowSize = inputImage.nr();
    const auto imageColSize = inputImage.nc();

    unsigned  neighSizeCenter = neighborhoodSize/2;

    //Define begin index
    unsigned  imageRowBegin = (neighSizeCenter > row  ) ? 0 : row   - neighSizeCenter;
    unsigned  imageColBegin = (neighSizeCenter > colum) ? 0 : colum - neighSizeCenter;


    //Define actual neighbprhood size

    unsigned imageRowEnd = (row   + neighSizeCenter >= imageRowSize) ? imageRowSize :  row   + neighSizeCenter+1;
    unsigned imageColEnd = (colum + neighSizeCenter >= imageColSize) ? imageColSize :  colum + neighSizeCenter+1;


    /* std::cout<<index<<std::endl;
    std::cout<<imageColBegin<<" "<<imageRowBegin<<std::endl;
    std::cout<<imageColEnd - imageColBegin<<" "<<imageRowEnd - imageRowBegin<<std::endl;
    std::cout<<std::endl;
    */

    roi.set_size(imageRowEnd - imageRowBegin, imageColEnd - imageColBegin);
    unsigned roiR=0;
    for(unsigned r = imageRowBegin; r < imageRowEnd ; ++r)
    {
        unsigned roiC=0;
        for(unsigned c=imageColBegin; c < imageColEnd; ++c)
        {

            roi[roiR][roiC++] = inputImage[r][c];
        }
        ++roiR;
    }

    return 1;

}


}



#endif // IMAGEOPERATIONS_H
