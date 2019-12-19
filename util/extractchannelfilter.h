#ifndef EXTRACTCHANNELFILTER_H
#define EXTRACTCHANNELFILTER_H


#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>


template <typename inputImageT, typename outputImageT>
class ExtractChannelFilter
{
    using inputImageP  = typename inputImageT::Pointer;
    using outputImageP = typename outputImageT::Pointer;



public:
    ExtractChannelFilter();

    void setImputImage(inputImageP inputImage);

    outputImageP getOutputImage();

    void extractChannel(unsigned channelIndex);


private:

    inputImageP inputImage;
    outputImageP outputImage;


};

using uImage = itk::Image<itk::RGBPixel<unsigned>, 2>;
using floatImage = itk::Image<itk::RGBPixel<float>, 2>;

template class ExtractChannelFilter<uImage, itk::Image<unsigned,2>>;
template class ExtractChannelFilter<floatImage, itk::Image<float,2>>;





#endif // EXTRACTCHANNELFILTER_H
