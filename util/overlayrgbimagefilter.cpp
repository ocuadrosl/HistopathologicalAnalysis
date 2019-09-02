#include "overlayrgbimagefilter.h"
template<typename imageT>
OverlayRGBImageFilter<imageT>::OverlayRGBImageFilter(): alpha(0.5)
{

}

template<typename imageT>
void OverlayRGBImageFilter<imageT>::setBackgroundImage(rgbImageP backgroundImage)
{
    this->backgroundImage = backgroundImage;
}


template<typename imageT>
void OverlayRGBImageFilter<imageT>::setForegroundImage(rgbImageP foregroundImage)
{
    this->foregroundImage = foregroundImage;
}


template<typename imageT>
typename OverlayRGBImageFilter<imageT>::rgbImageP
OverlayRGBImageFilter<imageT>::getOutput() const
{
    return outputImage;
}
template<typename imageT>
void OverlayRGBImageFilter<imageT>::setAlpha(double alpha)
{

    this->alpha = alpha;

}

template<typename imageT>
void OverlayRGBImageFilter<imageT>::softLigh()
{

    //allocate output image
    if(outputImage.IsNull())
    {
        outputImage  = imageT::New();
        outputImage->SetRegions(backgroundImage->GetRequestedRegion());
        outputImage->Allocate();
    }

    //create iterators
    itk::ImageRegionConstIterator<imageT> backIt  (backgroundImage, backgroundImage->GetRequestedRegion());
    itk::ImageRegionConstIterator<imageT> foreIt  (foregroundImage, foregroundImage->GetRequestedRegion());
    itk::ImageRegionIterator     <imageT> outputIt(outputImage    , outputImage    ->GetRequestedRegion());


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


        outputIt.Set(static_cast<rgbPixelT>(outputPixelFloat*255));

        ++backIt;
        ++foreIt;
        ++outputIt;
    }

}




template<typename imageT>
void OverlayRGBImageFilter<imageT>:: overlay()
{
    if(outputImage.IsNull())
    {
        outputImage  = imageT::New();
        outputImage->SetRegions(backgroundImage->GetRequestedRegion());
        outputImage->Allocate();
    }


    //create iterators
    itk::ImageRegionConstIterator< imageT > backIt(backgroundImage, backgroundImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< imageT > foreIt(foregroundImage, foregroundImage->GetRequestedRegion());
    itk::ImageRegionIterator< imageT >     outputIt(outputImage, outputImage->GetRequestedRegion());


    rgbPixelT outputPixel;

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


        outputPixel = static_cast<rgbPixelT>(outputPixelFloat*255);


        outputIt.Set(outputPixel);

        ++backIt;
        ++foreIt;
        ++outputIt;
    }



}




