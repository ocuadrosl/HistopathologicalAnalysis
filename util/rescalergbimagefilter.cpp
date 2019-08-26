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
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::rescale()
{

    //compute min and max inputImage values

    using minMaxRGBImageCalculatorType = MinMaxRGBImageCalculator<inputPixelType>;
    std::unique_ptr<minMaxRGBImageCalculatorType>  minMaxRGBImageCalculator(new minMaxRGBImageCalculatorType());
    minMaxRGBImageCalculator->setInput(inputImage);
    minMaxRGBImageCalculator->calculate();

    auto minInputValue = minMaxRGBImageCalculator->getMinValue();
    auto maxInputValue = minMaxRGBImageCalculator->getMaxValue();


    outputImage  = rgbOutputImageType::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();

    itk::ImageRegionConstIterator< rgbInputImageType > inputIt(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbOutputImageType >     outputIt(outputImage, outputImage->GetRequestedRegion());

    rgbInputPixelType  inputPixel;
    rgbOutputPixelType  outputPixel;

    //functions alias



    Math::MinMax<inputPixelType, outputPixelType> minMax0(minInputValue[0], maxInputValue[0], 0, 255);
    Math::MinMax<inputPixelType, outputPixelType> minMax1(minInputValue[1], maxInputValue[1], 0, 255);
    Math::MinMax<inputPixelType, outputPixelType> minMax2(minInputValue[1], maxInputValue[2], 0, 255);


    while ( !inputIt.IsAtEnd() )
    {

        inputPixel = inputIt.Get();
        //std::cout<<inputPixel<<std::endl;
        outputPixel[0] = minMax0( inputPixel[0] );
        outputPixel[1] = minMax1( inputPixel[1] );
        outputPixel[2] = minMax2( inputPixel[2] );

        //std::cout<<outputPixel<<std::endl;
        outputIt.Set(outputPixel);

        ++inputIt;
        ++outputIt;
    }


}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>::setMinValue(const rgbInputPixelType& minValue)
{
    this->minValue = minValue;
}


template<typename inputPixelType, typename outputPixelType>
void RescaleRGBImageFilter<inputPixelType, outputPixelType>:: setMaxValue(const rgbInputPixelType& maxValue)
{
    this->maxValue = maxValue;
}














