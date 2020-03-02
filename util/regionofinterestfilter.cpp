#include "regionofinterestfilter.h"



template<typename InputImageT, typename OutputImageT>
RegionOfInterestFilter<InputImageT,  OutputImageT>::RegionOfInterestFilter()
{

}

template<typename InputImageT, typename OutputImageT>
void RegionOfInterestFilter<InputImageT,  OutputImageT>::SetInputImage(InputImageP inputImage)
{
    this->inputImage = inputImage;
}


template<typename InputImageT, typename OutputImageT>
typename RegionOfInterestFilter<InputImageT,  OutputImageT>::OutputImageP
RegionOfInterestFilter<InputImageT,  OutputImageT>::GetRegionOfInterest()
{

    return  outputImage;

}


template<typename InputImageT, typename OutputImageT>
void RegionOfInterestFilter<InputImageT,  OutputImageT>::SetRegionOfInterest(RegionT roi)
{

    this->roi = roi;

}


template<typename InputImageT, typename OutputImageT>
void RegionOfInterestFilter<InputImageT,  OutputImageT>::Compute()
{




    typename OutputImageT::RegionType region;
    region.SetSize(roi.GetSize());

    outputImage = OutputImageT::New();
    outputImage->SetRegions(region);
    outputImage->Allocate();

    itk::ImageRegionConstIterator<InputImageT> iIt(inputImage, roi);
    itk::ImageRegionIterator<OutputImageT> oIt(outputImage, outputImage->GetRequestedRegion());


    for(oIt.GoToBegin(); !oIt.IsAtEnd(); ++iIt, ++oIt)
    {

        oIt.Set(iIt.Get());

    }



}
