#include "roiextractor.h"


template<typename grayImageType>
ROIExtractor<grayImageType>::ROIExtractor():
    maskSize(3)
{

}

template<typename grayImageType>
void  ROIExtractor<grayImageType>::setMaskSize(short maskSize)
{
    this->maskSize = maskSize;
}

template<typename grayImageType>
void ROIExtractor<grayImageType>::setImage(grayImagePointer inputImage)
{

    this->inputImage = inputImage;
}

template<typename grayImageType>
void ROIExtractor<grayImageType>::process()
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
    sizeType imageSize = binaryImage->GetRequestedRegion().GetSize();

    int maxDensity = std::pow(maskSize,2);
    short counter;


    //allocate densityImage
    densityImage = grayImageType::New();
    densityImage->SetRegions(binaryImage->GetRequestedRegion());
    densityImage->Allocate();
    densityImage->FillBuffer( itk::NumericTraits<typename grayImageType::PixelType>::Zero );


    //iterators
    iteratorIndexType itI(binaryImage, binaryImage->GetRequestedRegion()); // inputImage iterator
    iteratorIndexType itD(densityImage, densityImage->GetRequestedRegion()); //densityImage iterator

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

            itD.Set((counter*100)/maxDensity);

        }


    }


    //visualizing
    std::unique_ptr<QuickView> viewer(new QuickView());
    viewer->AddImage(densityImage.GetPointer());
    viewer->Visualize();

    applyColorMap();


}

template<typename graImageType>
void ROIExtractor<graImageType>::applyColorMap()
{

    //TODO verificar si se puede cambiar esto de char....
    using rgbPixel = itk::RGBPixel<unsigned char>;
    using rgbImage = itk::Image< rgbPixel, 2 >;


    using RGBFilterType = itk::ScalarToRGBColormapImageFilter< graImageType, rgbImage>;
    typename RGBFilterType::Pointer rgbfilter = RGBFilterType::New();
    rgbfilter->SetInput( densityImage );
    rgbfilter->SetColormap( RGBFilterType::Jet );
    rgbfilter->Update();


    //visualizing
    QuickView viewer;
    viewer.AddRGBImage(rgbfilter->GetOutput());
    viewer.Visualize();

}

/*

*/
template<typename grayImageType>
typename ROIExtractor<grayImageType>::grayImagePointer
ROIExtractor<grayImageType>::otsuThreshold()
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

template<typename grayImageType>
auto ROIExtractor<grayImageType>::getColorMap() const
{
     return colorMap;
}



