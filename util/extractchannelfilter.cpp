#include "extractchannelfilter.h"

template <typename inputImageT, typename outputImageT>
ExtractChannelFilter<inputImageT, outputImageT>::ExtractChannelFilter()
{

}


template <typename inputImageT, typename outputImageT>
void ExtractChannelFilter<inputImageT, outputImageT>::setImputImage(inputImageP inputImage)
{
    this->inputImage = inputImage;
}


template <typename inputImageT, typename outputImageT>
typename ExtractChannelFilter<inputImageT, outputImageT>::outputImageP
ExtractChannelFilter<inputImageT, outputImageT>::getOutputImage()
{
    return outputImage;
}

template <typename inputImageT, typename outputImageT>
void ExtractChannelFilter<inputImageT, outputImageT>::extractChannel(unsigned channelIndex)
{

    outputImage = outputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();


    itk::ImageRegionConstIterator<inputImageT> inputIt(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator<outputImageT> outputIt(outputImage, outputImage->GetRequestedRegion());


    while(!inputIt.IsAtEnd())
    {


        outputIt.Set(inputIt.Get()[channelIndex]);

        ++outputIt;
        ++inputIt;

    }




}
