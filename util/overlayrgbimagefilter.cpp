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


    rgbPixelType outputPixel;

    using rgbPixelFloat = itk::RGBPixel<float>;
    rgbPixelFloat backPixel;
    rgbPixelFloat forePixel;


    float alphaTimes255 = alpha*255;


    while ( !backIt.IsAtEnd() )
    {

        backPixel = static_cast<rgbPixelFloat>(backIt.Get());
        forePixel = static_cast<rgbPixelFloat>(foreIt.Get());

        if( backPixel[0] < alphaTimes255 )
        {
            outputPixel[0] = static_cast<pixelType>((alphaTimes255 + backPixel[0]) * forePixel[0]);
        }
        else
        {
            outputPixel[0] = static_cast<pixelType>(255 - ( (255 - backPixel[0]-alphaTimes255) * (255-forePixel[0])));
        }

        if( backPixel[1] < alphaTimes255 )
        {
            outputPixel[1] = static_cast<pixelType>((alphaTimes255 + backPixel[1]) * forePixel[1]);
        }
        else
        {
            outputPixel[1] = static_cast<pixelType>(255 - ((255 - backPixel[1]-alphaTimes255) * (255 - forePixel[1])));
        }

        if( backPixel[2] < alphaTimes255 )
        {
            outputPixel[2] = static_cast<pixelType>((alphaTimes255 + backPixel[2]) * forePixel[2]);
        }
        else
        {
            outputPixel[2] = static_cast<pixelType>(255 - ((255 - backPixel[2]-alphaTimes255) * (255 - forePixel[2])));
        }


        outputIt.Set(outputPixel);

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
    rgbPixelFloat backPixel;
    rgbPixelFloat forePixel;

    float oneMinusAlpha = 1.f - alpha;


    while ( !backIt.IsAtEnd() )
    {

        backPixel = static_cast<rgbPixelFloat>(backIt.Get());
        forePixel = static_cast<rgbPixelFloat>(foreIt.Get());

        outputPixel[0] = static_cast<pixelType>((alpha * forePixel[0]) + (oneMinusAlpha * backPixel[0]));
        outputPixel[1] = static_cast<pixelType>((alpha * forePixel[1]) + (oneMinusAlpha * backPixel[1]));
        outputPixel[2] = static_cast<pixelType>((alpha * forePixel[2]) + (oneMinusAlpha * backPixel[2]));

        outputIt.Set(outputPixel);

        ++backIt;
        ++foreIt;
        ++outputIt;
    }


}












