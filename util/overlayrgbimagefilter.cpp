#include "overlayrgbimagefilter.h"
template<typename pixelType>
OverlayRGBImageFilter<pixelType>::OverlayRGBImageFilter(): alpha(0.5)
{

}

template<typename pixelType>
void OverlayRGBImageFilter<pixelType>::setBackgroundImage(rgbImagePointer backgroundImage)
{
    this->backgroundImage = backgroundImage;
}


template<typename pixelType>
void OverlayRGBImageFilter<pixelType>::setForegroundImage(rgbImagePointer foregroundImage)
{
    this->foregroundImage = foregroundImage;
}


template<typename pixelType>
typename OverlayRGBImageFilter<pixelType>::rgbImagePointer
OverlayRGBImageFilter<pixelType>::getOutput() const
{
    return outputImage;
}
template<typename pixelType>
void OverlayRGBImageFilter<pixelType>::setAlpha(float alpha)
{

    this->alpha = alpha;

}

template<typename pixelType>
void OverlayRGBImageFilter<pixelType>::softLigh()
{

    //allocate output image
    if(outputImage.IsNull())
    {
        outputImage  = rgbImageType::New();
        outputImage->SetRegions(backgroundImage->GetRequestedRegion());
        outputImage->Allocate();
    }

    //create iterators
    itk::ImageRegionConstIterator< rgbImageType > backIt(backgroundImage, backgroundImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< rgbImageType > foreIt(foregroundImage, foregroundImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbImageType >     outputIt(outputImage, outputImage->GetRequestedRegion());


    using rgbPixelFloat = itk::RGBPixel<float>;
    rgbPixelFloat backPixel, forePixel, outputPixelFloat;


    while ( !backIt.IsAtEnd() )
    {

        backPixel = static_cast<rgbPixelFloat>(backIt.Get())/255.f;
        forePixel = static_cast<rgbPixelFloat>(foreIt.Get())/255.f;

        if(forePixel[0] < alpha )
        {
            outputPixelFloat[0]=(2*backPixel[0]*forePixel[0]) + std::pow(backPixel[0],2.f)*(1-2*forePixel[0]);
        }
        else
        {
            outputPixelFloat[0] = 2*backPixel[0]*(1-forePixel[0]) + std::sqrt(backPixel[0])*(2*forePixel[0]-1);
        }

        if( forePixel[1] < alpha )
        {
             outputPixelFloat[1]=(2*backPixel[1]*forePixel[1]) + std::pow(backPixel[1],2.f)*(1-2*forePixel[1]);
        }
        else
        {
            outputPixelFloat[1] = 2*backPixel[1]*( 1-forePixel[1]) + std::sqrt(backPixel[1])*(2*forePixel[1]-1);
        }

        if( forePixel[2] < alpha )
        {
             outputPixelFloat[2]=(2*backPixel[0]*forePixel[2]) + std::pow(backPixel[2],2.f)*(1-2*forePixel[2]);
        }
        else
        {
            outputPixelFloat[2] = 2*backPixel[2]*(1-forePixel[2]) + std::sqrt(backPixel[2])*(2*forePixel[2]-1);
        }


        outputIt.Set(static_cast<rgbPixelType>(outputPixelFloat*255));

        ++backIt;
        ++foreIt;
        ++outputIt;
    }

}




template<typename pixelType>
void OverlayRGBImageFilter<pixelType>:: overlay()
{
    if(outputImage.IsNull())
    {
        outputImage  = rgbImageType::New();
        outputImage->SetRegions(backgroundImage->GetRequestedRegion());
        outputImage->Allocate();
    }


    //create iterators
    itk::ImageRegionConstIterator< rgbImageType > backIt(backgroundImage, backgroundImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< rgbImageType > foreIt(foregroundImage, foregroundImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbImageType >     outputIt(outputImage, outputImage->GetRequestedRegion());


    rgbPixelType outputPixel;

    using rgbPixelFloat = itk::RGBPixel<float>;
    rgbPixelFloat backPixel, forePixel, outputPixelFloat;

    while ( !backIt.IsAtEnd() )
    {

         //std::cout<<foreIt.Get()<<std::endl;

        backPixel = static_cast<rgbPixelFloat>(backIt.Get())/255.f;
        forePixel = static_cast<rgbPixelFloat>(foreIt.Get())/255.f;
        //rgbAlpha = forePixel*alpha;

        //red channel
        if( backPixel[0] < alpha )
        {

            outputPixelFloat[0] =  2 * backPixel[0] * forePixel[0] ;

        }
        else
        {
            outputPixelFloat[0] =  1 - 2*(1 - backPixel[0])*(1 - forePixel[0]) ;
        }

        //green channel
        if( backPixel[1] < alpha )
        {

            outputPixelFloat[1] =  2*backPixel[1]*forePixel[1] ;

        }
        else
        {
            outputPixelFloat[1] =  1 - 2*(1-backPixel[1])*(1-forePixel[1]) ;
        }

        //blue channel
        if( backPixel[2] < alpha )
        {

            outputPixelFloat[2] =  2*backPixel[2]*forePixel[2] ;

        }
        else
        {
            outputPixelFloat[2] =  1 - 2*(1-backPixel[2])*(1-forePixel[2]);
        }


        outputPixel = static_cast<rgbPixelType>(outputPixelFloat*255);


        outputIt.Set(outputPixel);

        ++backIt;
        ++foreIt;
        ++outputIt;
    }

    /*
    using rescaleRGBImageFilterType = RescaleRGBImageFilter<pixelType, pixelType>;
    std::unique_ptr<rescaleRGBImageFilterType> rescaleRGBImageFilter(new rescaleRGBImageFilterType());

    rescaleRGBImageFilter->setInput(outputImage);
    rescaleRGBImageFilter->setMinInputValue(itk::NumericTraits<rgbPixelType>::Zero);
    rescaleRGBImageFilter->setMaxInputValue(itk::NumericTraits<rgbPixelType>::Zero+(2*255*255));
    rescaleRGBImageFilter->setMinOutputValue(itk::NumericTraits<rgbPixelType>::Zero);
    rescaleRGBImageFilter->setMaxOutputValue(itk::NumericTraits<rgbPixelType>::Zero+255);

    rescaleRGBImageFilter->rescale();

    outputImage = rescaleRGBImageFilter->getOutput();
    */


}




