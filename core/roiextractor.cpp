#include "roiextractor.h"




ROIExtractor::ROIExtractor():
    maskSize(3)
{
    std::cout<<typeid(pixelType).name()<<std::endl;
}


void  ROIExtractor::setMaskSize(short maskSize)
{
    this->maskSize = maskSize;
}


void ROIExtractor::setImage(grayImagePointer inputImage)
{

    this->inputImage = inputImage;
}


void ROIExtractor::extract()
{

    //Otsu threshold
    grayImagePointer binaryImage =  grayImageType::New();
    binaryImage = otsuThreshold();


    //typedefs
    using iteratorIndexType = itk::ImageRegionIteratorWithIndex<grayImageType>;
    using regionType = typename grayImageType::RegionType;
    using indexType = typename grayImageType::IndexType;
    using sizeType  = typename grayImageType::SizeType;

    //definitions
    short maskHalfSize = maskSize/2;
    indexType index, lowerIndex, upperIndex;
    regionType localRegion;


    int maxDensity = maskSize * maskSize;
    short counter;


    //allocate densityImage
    densityImage = grayImageType::New();
    densityImage->SetRegions(binaryImage->GetRequestedRegion());
    densityImage->Allocate();
    densityImage->FillBuffer( itk::NumericTraits<typename grayImageType::PixelType>::Zero );


    //iterators
    iteratorIndexType itI(binaryImage, binaryImage->GetRequestedRegion()); // inputImage iterator
    iteratorIndexType itD(densityImage, densityImage->GetRequestedRegion()); //densityImage iterator

    //to void unsigned long  - signed long comparison
    std::vector<long> imageSize(2);
    imageSize[0] = static_cast<long>(binaryImage->GetRequestedRegion().GetSize()[0]);
    imageSize[1] = static_cast<long>(binaryImage->GetRequestedRegion().GetSize()[1]);

    for (itI.GoToBegin() , itD.GoToBegin(); !itI.IsAtEnd(); ++itI, ++itD)
    {

        if(itI.Get()==0)
        {
            index = itI.GetIndex();

            lowerIndex[0] = (index[0] - maskHalfSize >= 0) ? index[0] - maskHalfSize : 0 ;
            lowerIndex[1] = (index[1] - maskHalfSize >= 0) ? index[1] - maskHalfSize : 0 ;

            upperIndex[0] = (index[0] + maskHalfSize < imageSize[0]) ? index[0] + maskHalfSize : imageSize[0] - 1;
            upperIndex[1] = (index[1] + maskHalfSize < imageSize[1]) ? index[1] + maskHalfSize : imageSize[1] - 1;



            localRegion.SetIndex(lowerIndex);
            localRegion.SetUpperIndex(upperIndex);

            iteratorIndexType itM(binaryImage, localRegion); //localMask iterator

            counter = 0;
            for (itM.GoToBegin() ; !itM.IsAtEnd(); ++itM)
            {
                if(itM.Get()==0)
                {
                    ++counter;
                }

            }

            itD.Set(static_cast<pixelType>((counter*100)/maxDensity));
            //std::cout<<(counter*100)/maxDensity<<std::endl;

        }


    }



    //visualizing
    std::unique_ptr<QuickView> viewer(new QuickView());
    viewer->AddImage(densityImage.GetPointer(), true, "Density");
    viewer->Visualize();


    applyColorMap();



}



void ROIExtractor::applyColorMap()
{


    using SpecificColormapType = itk::Function::JetColormapFunction<pixelType, rgbPixelType >;
    typename SpecificColormapType::Pointer colormap = SpecificColormapType::New();

    colormap->SetMinimumInputValue(0);
    colormap->SetMaximumInputValue(100);


    typename rgbImageType::Pointer colorMapImage  = rgbImageType::New();

    colorMapImage->SetRegions(densityImage->GetRequestedRegion());
    colorMapImage->Allocate();
    colorMapImage->FillBuffer( itk::NumericTraits<rgbPixelType>::Zero);


    itk::ImageRegionConstIterator< grayImageType > inputIt(densityImage, densityImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbImageType >     outputIt(colorMapImage, colorMapImage->GetRequestedRegion());

    while ( !inputIt.IsAtEnd() )
    {
        if(inputIt.Get()>0)
        {
            outputIt.Set(colormap->operator()( inputIt.Get() ) );
        }
        ++inputIt;
        ++outputIt;
    }


    //visualizing

    QuickView viewer;
    viewer.AddRGBImage(colorMapImage.GetPointer());
    viewer.Visualize();


}

/*

*/

typename ROIExtractor::grayImagePointer ROIExtractor::otsuThreshold()
{

    using otsuType = itk::OtsuMultipleThresholdsImageFilter< grayImageType, grayImageType >;
    typename otsuType::Pointer filter = otsuType::New();
    filter->SetInput( this->inputImage );
    //filter->SetNumberOfHistogramBins( 50 );
    filter->SetNumberOfThresholds( 1 );
    //filter->SetLabelOffset( 100 );
    filter->Update();

    using rescaleType = itk::RescaleIntensityImageFilter< grayImageType, grayImageType >;
    typename rescaleType::Pointer rescaler = rescaleType::New();
    rescaler->SetInput( filter->GetOutput() );
    rescaler->SetOutputMinimum( 0 );
    rescaler->SetOutputMaximum( 255 );
    rescaler->Update();

    //return a binary (0 - 255) image
    return rescaler->GetOutput();

}


auto ROIExtractor::getColorMap() const
{
     return colorMap;
}



