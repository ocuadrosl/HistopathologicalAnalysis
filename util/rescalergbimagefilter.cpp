#include "rescalergbimagefilter.h"


template<typename inputPixelType, typename outputPixelType>
RescaleRGBImageFilter<inputPixelType, outputPixelType>::RescaleRGBImageFilter()
{

}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::setInput(rgbInputImagePointer inputImage)
{
    this->inputImage = inputImage;

}

template<typename inputPixelType, typename outputPixelType>
typename RescaleRGBImageFilter<inputPixelType, outputPixelType>::rgbOutputImagePointer
RescaleRGBImageFilter<inputPixelType, outputPixelType>::getOutput() const
{
    return outputImage;

}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::rescaleAuto()
{

    //compute min and max inputImage values

    using minMaxRGBImageCalculatorType = MinMaxRGBImageCalculator<rgbInputImageType>;
    std::unique_ptr<minMaxRGBImageCalculatorType>  minMaxRGBImageCalculator(new minMaxRGBImageCalculatorType());
    minMaxRGBImageCalculator->setImage(inputImage);
    minMaxRGBImageCalculator->compute();

    auto minInputValue = minMaxRGBImageCalculator->getMinValues();
    auto maxInputValue = minMaxRGBImageCalculator->getMaxValues();

    //allocate output image
    outputImage  = rgbOutputImageType::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    //iterators
    itk::ImageRegionConstIterator< rgbInputImageType > inputIt(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbOutputImageType >     outputIt(outputImage, outputImage->GetRequestedRegion());

    rgbInputPixelType  inputPixel;
    rgbOutputPixelType  outputPixel;

    //min-max template function objects
    Math::MinMax<inputPixelType, outputPixelType> minMax0(minInputValue[0], maxInputValue[0], 0, 255);
    Math::MinMax<inputPixelType, outputPixelType> minMax1(minInputValue[1], maxInputValue[1], 0, 255);
    Math::MinMax<inputPixelType, outputPixelType> minMax2(minInputValue[2], maxInputValue[2], 0, 255);

    //rescaling
    while ( !inputIt.IsAtEnd() )
    {

        inputPixel = inputIt.Get();

        outputPixel[0] = minMax0( inputPixel[0] );
        outputPixel[1] = minMax1( inputPixel[1] );
        outputPixel[2] = minMax2( inputPixel[2] );

        outputIt.Set(outputPixel);

        ++inputIt;
        ++outputIt;
    }


}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::rescale()
{


    //allocate output image
    outputImage  = rgbOutputImageType::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    //iterators
    itk::ImageRegionConstIterator< rgbInputImageType > inputIt(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbOutputImageType >     outputIt(outputImage, outputImage->GetRequestedRegion());

    rgbInputPixelType  inputPixel;
    rgbOutputPixelType  outputPixel;

    //min-max template function objects
    Math::MinMax<inputPixelType, outputPixelType> minMax0(minInputValue[0], maxInputValue[0], minOutputValue[0], maxOutputValue[0]);
    Math::MinMax<inputPixelType, outputPixelType> minMax1(minInputValue[1], maxInputValue[1], minOutputValue[1], maxOutputValue[1]);
    Math::MinMax<inputPixelType, outputPixelType> minMax2(minInputValue[2], maxInputValue[2], minOutputValue[2], maxOutputValue[2]);

    //rescaling
    while ( !inputIt.IsAtEnd() )
    {

        inputPixel = inputIt.Get();

        outputPixel[0] = minMax0( inputPixel[0] );
        outputPixel[1] = minMax1( inputPixel[1] );
        outputPixel[2] = minMax2( inputPixel[2] );

        outputIt.Set(outputPixel);

        ++inputIt;
        ++outputIt;
    }


}










template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::setMinInputValue(const rgbInputPixelType& minValue)
{
    this->minInputValue = minValue;
}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>:: setMaxInputValue(const rgbInputPixelType& maxValue)
{
    this->maxInputValue = maxValue;
}

template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::setMinOutputValue(const rgbOutputPixelType& minValue)
{
    this->minOutputValue = minValue;
}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>:: setMaxOutputValue(const rgbOutputPixelType& maxValue)
{
    this->maxOutputValue = maxValue;
}















