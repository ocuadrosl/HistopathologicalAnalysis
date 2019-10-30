#include "overlayrgbimagefilter.h"
template<typename imageT>
OverlayRGBImageFilter<imageT>::OverlayRGBImageFilter(): foregroundAlpha(0.5)
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
void OverlayRGBImageFilter<imageT>::setForegroundAlpha(float alpha)
{

    this->foregroundAlpha = alpha;

}

template<typename imageT>
void OverlayRGBImageFilter<imageT>::softLigh()
{


    outputImage  = imageT::New();
    outputImage->SetRegions(backgroundImage->GetRequestedRegion());
    outputImage->Allocate();


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

        if(forePixel[0] < foregroundAlpha )
        {
            outputPixelFloat[0]=(2*backPixel[0]*forePixel[0]) + std::pow(backPixel[0],2.f)*(1-2*forePixel[0]);
        }
        else
        {
            outputPixelFloat[0] = 2*backPixel[0]*(1-forePixel[0]) + std::sqrt(backPixel[0])*(2*forePixel[0]-1);
        }

        if( forePixel[1] < foregroundAlpha )
        {
             outputPixelFloat[1]=(2*backPixel[1]*forePixel[1]) + std::pow(backPixel[1],2.f)*(1-2*forePixel[1]);
        }
        else
        {
            outputPixelFloat[1] = 2*backPixel[1]*( 1-forePixel[1]) + std::sqrt(backPixel[1])*(2*forePixel[1]-1);
        }

        if( forePixel[2] < foregroundAlpha )
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
inline float OverlayRGBImageFilter<imageT>:: blendMode(float background, float foreground, std::string mode)
{

    if(mode == "multiply")
    {
        return background*foreground;
    }

    //TODO implement other modes

    return 0;


}

template<typename imageT>
void OverlayRGBImageFilter<imageT>::alphaBlending()
{


    outputImage  = imageT::New();
    outputImage->SetRegions(backgroundImage->GetRequestedRegion());
    outputImage->Allocate();

    //create iterators
    itk::ImageRegionConstIterator< imageT > backIt(backgroundImage, backgroundImage->GetRequestedRegion());
    itk::ImageRegionConstIterator< imageT > foreIt(foregroundImage, foregroundImage->GetRequestedRegion());
    itk::ImageRegionIterator< imageT >     outputIt(outputImage, outputImage->GetRequestedRegion());


    rgbPixelT outputPixel;

    using rgbPixelFloat = itk::RGBPixel<float>;
    rgbPixelFloat backPixel, forePixel, outputPixelFloat;


    float aBack = backgroundAlpha * (1.f - foregroundAlpha);
    float aFore = foregroundAlpha * (1.f - backgroundAlpha);
    float aBoth = backgroundAlpha * foregroundAlpha;


    while ( !backIt.IsAtEnd() )
    {

        backPixel = static_cast<rgbPixelFloat>(backIt.Get())/255.f;
        forePixel = static_cast<rgbPixelFloat>(foreIt.Get())/255.f;
        //rgbAlpha = forePixel*alpha;

        outputPixelFloat[0] = aBack*backPixel[0] + aFore*forePixel[0] + aBoth*blendMode(backPixel[0], forePixel[0], "multiply");
        outputPixelFloat[1] = aBack*backPixel[1] + aFore*forePixel[1] + aBoth*blendMode(backPixel[1], forePixel[1], "multiply");
        outputPixelFloat[2] = aBack*backPixel[2] + aFore*forePixel[2] + aBoth*blendMode(backPixel[2], forePixel[2], "multiply");

        outputPixel = static_cast<rgbPixelT>(outputPixelFloat*255);

        outputIt.Set(outputPixel);

        ++backIt;
        ++foreIt;
        ++outputIt;
    }




}

template<typename imageT>
void OverlayRGBImageFilter<imageT>:: overlay()
{

    outputImage  = imageT::New();
    outputImage->SetRegions(backgroundImage->GetRequestedRegion());
    outputImage->Allocate();



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
        if( backPixel[0] < foregroundAlpha )
        {

            outputPixelFloat[0] =  2 * backPixel[0] * forePixel[0] ;

        }
        else
        {
            outputPixelFloat[0] =  1 - 2*(1 - backPixel[0])*(1 - forePixel[0]) ;
        }

        //green channel
        if( backPixel[1] < foregroundAlpha )
        {

            outputPixelFloat[1] =  2*backPixel[1]*forePixel[1] ;

        }
        else
        {
            outputPixelFloat[1] =  1 - 2*(1-backPixel[1])*(1-forePixel[1]) ;
        }

        //blue channel
        if( backPixel[2] < foregroundAlpha )
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




