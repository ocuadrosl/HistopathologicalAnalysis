#include "roiextractor.h"


template<typename pixelType>
ROIExtractor<pixelType>::ROIExtractor():
    kernelSize(5), densityThreshold(50)
{

}


template<typename pixelType>
void  ROIExtractor<pixelType>::setKernelSize(short kernelSize)
{
    this->kernelSize = kernelSize;
}


template<typename pixelType>
void ROIExtractor<pixelType>::setImage(rgbImagePointer inputImage)
{

    this->inputImage = inputImage;
}


template<typename pixelType>
void ROIExtractor<pixelType>::extract()
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
    short maskHalfSize = kernelSize/2;
    indexType index, lowerIndex, upperIndex;
    regionType localRegion;


    int maxDensity = kernelSize * kernelSize;
    short counter;

    //allocating densityImage
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


    pixelType  density = 0;

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

            density = static_cast<pixelType>((counter*100)/maxDensity);

            //Set if the density is greater than a threshold
            itD.Set( density >= densityThreshold ? density : 0 ) ;

        }


    }



    //visualizing

    VTKViewer<pixelType>::visualizeGray(densityImage, "Density");


    densityToColorMap();
    applyColorMap();

}


template<typename pixelType>
void ROIExtractor<pixelType>::applyColorMap()
{

/*
    using FilterType = itk::MultiplyImageFilter< rgbImageType, rgbImageType, rgbImageType >;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( inputImage);
    filter->SetInput2( colorMap );
    //filter->Update();


    //visualizing

    VTKViewer<rgbImageType>::visualize(filter->GetOutput());
*/

}

template<typename pixelType>
void ROIExtractor<pixelType>::densityToColorMap()
{


    //Jet colormap
    //Todo add more colormaps
    using SpecificColormapType = itk::Function::JetColormapFunction<pixelType, rgbPixelType >;
    typename SpecificColormapType::Pointer colormap = SpecificColormapType::New();

    colormap->SetMinimumInputValue(densityThreshold);
    colormap->SetMaximumInputValue(100);


    //RGB image
    colorMapImage  = rgbImageType::New();
    colorMapImage->SetRegions(densityImage->GetRequestedRegion());
    colorMapImage->Allocate();
    colorMapImage->FillBuffer( itk::NumericTraits<rgbPixelType>::Zero+255);



    itk::ImageRegionConstIterator< grayImageType > inputIt(densityImage, densityImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbImageType >     outputIt(colorMapImage, colorMapImage->GetRequestedRegion());

    while ( !inputIt.IsAtEnd() )
    {
        if(inputIt.Get() >= densityThreshold)
        {
            outputIt.Set(colormap->operator()( inputIt.Get() ) );
        }

        ++inputIt;
        ++outputIt;
    }

    //VTKViewer<>::visualizeRGB(colorMapImage, "Colormap");


}

/*

*/


template<typename pixelType>
typename ROIExtractor<pixelType>::grayImagePointer
ROIExtractor<pixelType>::otsuThreshold()
{

    //rgb to grayscale, Ostu does not work with RGB images
    using rgbToLuminanceFilterType = itk::RGBToLuminanceImageFilter< rgbImageType, grayImageType >;
    typename rgbToLuminanceFilterType::Pointer rgbToLuminancefilter = rgbToLuminanceFilterType::New();
    rgbToLuminancefilter->SetInput( inputImage );


    using otsuType = itk::OtsuThresholdImageFilter< grayImageType, grayImageType >;
    typename otsuType::Pointer otsuFilter = otsuType::New();
    otsuFilter->SetInput( rgbToLuminancefilter->GetOutput());
    otsuFilter->SetOutsideValue(255);
    otsuFilter->SetInsideValue(0);



    otsuFilter->Update();

    VTKViewer<pixelType>::visualizeGray(otsuFilter->GetOutput(), "Otsu");

    return otsuFilter->GetOutput();

}


template<typename pixelType>
auto ROIExtractor<pixelType>::getColorMap() const
{
     return colorMapImage;
}



