#include "roiextractor.h"



ROIExtractor::ROIExtractor()
{

}

void ROIExtractor::setImage(grayImageType::Pointer inputImage)
{

    this->image = inputImage;
}

void ROIExtractor::process()
{

    grayImageType::Pointer binaryImage =  grayImageType::New();
    binaryImage = otsuThreshold();

    /*visualizing*/
    std::unique_ptr<QuickView> viewer(new QuickView());
    viewer->AddImage(binaryImage.GetPointer());
    viewer->Visualize();


}

ROIExtractor::grayImageType::Pointer ROIExtractor::otsuThreshold()
{

    using otsuType = itk::OtsuMultipleThresholdsImageFilter< grayImageType, grayImageType >;
    otsuType::Pointer filter = otsuType::New();
    filter->SetInput( this->image );
    //filter->SetNumberOfHistogramBins( 50 );
    filter->SetNumberOfThresholds( 1 );
    //filter->SetLabelOffset( 100 );
    filter->Update();

    using rescaleType = itk::RescaleIntensityImageFilter< grayImageType, grayImageType >;
    rescaleType::Pointer rescaler = rescaleType::New();
    rescaler->SetInput( filter->GetOutput() );
    rescaler->SetOutputMinimum( 0 );
    rescaler->SetOutputMaximum( 255 );
    rescaler->Update();

    return rescaler->GetOutput();

}

ROIExtractor::rgbImageType::Pointer ROIExtractor::getColorMapImage() const
{
     return colorMap;
}



