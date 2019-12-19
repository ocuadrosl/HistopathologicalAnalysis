#ifndef REPLACEIMAGECHANNELFILTER_H
#define REPLACEIMAGECHANNELFILTER_H


#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

template<typename inputImageT>
class ReplaceImageChannelFilter
{

    using inputImageP = typename inputImageT::Pointer;
    using pixelT      = typename inputImageT::PixelType;
    using componentT  = typename pixelT::ComponentType;

    using channelImageT = itk::Image<componentT ,2>;
    using channelImageP = typename channelImageT::Pointer;



public:

    ReplaceImageChannelFilter();
    void replaceChannel(unsigned channelIndex);
    void setChannelImage(channelImageP channelImage);
    void setInputImage(inputImageP inputImage);
    inputImageP getOutput();

private:

    channelImageP channelImage;
    inputImageP inputImage;
    inputImageP outputImage;



};


using uImage = itk::Image<itk::RGBPixel<unsigned>, 2>;
using floatImage = itk::Image<itk::RGBPixel<float>, 2>;

template class ReplaceImageChannelFilter<uImage>;
template class ReplaceImageChannelFilter<floatImage>;


#endif // REPLACEIMAGECHANNELFILTER_H
