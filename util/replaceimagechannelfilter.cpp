#include "replaceimagechannelfilter.h"


template<typename inputImageT>
ReplaceImageChannelFilter<inputImageT>::ReplaceImageChannelFilter()
{

}

template<typename inputImageT>

void ReplaceImageChannelFilter<inputImageT>::setChannelImage(channelImageP channelImage)
{

    this->channelImage = channelImage;
}


template<typename inputImageT>
void ReplaceImageChannelFilter<inputImageT>::setInputImage(inputImageP inputImage)
{

    this->inputImage = inputImage;

}


template<typename inputImageT>
void ReplaceImageChannelFilter<inputImageT>::replaceChannel(unsigned channelIndex)
{


    outputImage = inputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator<channelImageT> channelIt(channelImage, channelImage->GetRequestedRegion());

    itk::ImageRegionConstIterator<inputImageT> inputIt(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator<inputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());



    while(!inputIt.IsAtEnd())
    {

        auto pixel =  inputIt.Get();
        pixel[channelIndex] = channelIt.Get();
        outputIt.Set(pixel);

        ++inputIt;
        ++channelIt;
        ++outputIt;

    }


}

template<typename inputImageT>
typename ReplaceImageChannelFilter<inputImageT>::inputImageP
ReplaceImageChannelFilter<inputImageT>::getOutput()
{

    return outputImage;
}


