#include "roiextractor.h"


ROIExtractor::ROIExtractor():
    kernelSize(5), densityThreshold(50)
{

}



void  ROIExtractor::setKernelSize(short kernelSize)
{
    this->kernelSize = kernelSize;
}



void ROIExtractor::setImage(rgbImageP inputImage)
{

    this->inputImage = inputImage;
}



void ROIExtractor::computeDensity(bool showResult)
{

    //IO::printWait("Computing density");

    //rgb to gray image, otsu does not work with rgb images
    rgbToGrayImage();

    //Otsu threshold
    grayImageP binaryImage =  grayImageType::New();
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


    //allocating densityImage
    densityImage = grayImageType::New();
    densityImage->SetRegions(binaryImage->GetRequestedRegion());
    densityImage->Allocate();
    densityImage->FillBuffer(itk::NumericTraits<typename grayImageType::PixelType>::Zero);


    //iterators
    iteratorIndexType itI(binaryImage, binaryImage->GetRequestedRegion()); // inputImage iterator
    iteratorIndexType itD(densityImage, densityImage->GetRequestedRegion()); //densityImage iterator

    //to avoid unsigned long  - signed long comparison
    std::vector<long> imageSize(2);
    imageSize[0] = static_cast<long>(binaryImage->GetRequestedRegion().GetSize()[0]);
    imageSize[1] = static_cast<long>(binaryImage->GetRequestedRegion().GetSize()[1]);


    pixelComponentT  density = 0;
    short counter;
    for (itI.GoToBegin() , itD.GoToBegin(); !itI.IsAtEnd(); ++itI, ++itD)
    {

        if(itI.Get()== FOREGROUND ) //otsu inside value
        {
            index = itI.GetIndex();

            lowerIndex[0] = (index[0] - maskHalfSize >= 0) ? index[0] - maskHalfSize : 0 ;
            lowerIndex[1] = (index[1] - maskHalfSize >= 0) ? index[1] - maskHalfSize : 0 ;

            upperIndex[0] = (index[0] + maskHalfSize < imageSize[0]) ? index[0] + maskHalfSize : imageSize[0] - 1;
            upperIndex[1] = (index[1] + maskHalfSize < imageSize[1]) ? index[1] + maskHalfSize : imageSize[1] - 1;



            localRegion.SetIndex(lowerIndex);
            localRegion.SetUpperIndex(upperIndex);

            //local kernel iterator
            iteratorIndexType itK(binaryImage, localRegion);

            counter = 0;

            for (itK.GoToBegin() ; !itK.IsAtEnd(); ++itK)
            {
                if(itK.Get() == FOREGROUND) //otsu inside value
                {
                    ++counter;
                }

            }

            density = static_cast<pixelComponentT>( (counter * 100) / maxDensity);

            itD.Set(density);
            //itD.Set( density >= densityThreshold ? density : 0 ) ;

        }


    }

    IO::printOK("Computing density");

    //visualizing
    if(showResult)
    {
        VTKViewer::visualize<grayImageType>(densityImage, "Density");
    }



}



void ROIExtractor::blendColorMap(bool showResult)
{

    if(colorMapImage.IsNull())
    {
        IO::printFail("Blending colormap", "Call densityToColorMap before");
        return;

    }

    using OverlayRGBImageFilterT = OverlayRGBImageFilter<  rgbImageType >;
    std::unique_ptr<OverlayRGBImageFilterT> overlayImageFilter(new OverlayRGBImageFilterT());
    overlayImageFilter->setBackgroundImage(inputImage);
    overlayImageFilter->setForegroundImage(colorMapImage);
    overlayImageFilter->setAlpha(0.8);
    overlayImageFilter->softLigh();

    IO::printOK("Blending colormap");

    if(showResult)
    {

        VTKViewer::visualize<rgbImageType>(overlayImageFilter->getOutput(), "Blended colormap");
    }


}


void ROIExtractor::densityToColorMap(bool showResult)
{

    if(densityImage.IsNull())
    {
        IO::printFail("Density to colormap", "Call computeDensity before");
        return;

    }


    //Jet colormap
    //Todo add more colormaps
    using SpecificColormapType = itk::Function::JetColormapFunction<pixelComponentT, rgbPixelType >;
    typename SpecificColormapType::Pointer colormap = SpecificColormapType::New();

    colormap->SetMinimumInputValue(densityThreshold);
    colormap->SetMaximumInputValue(100);
    colormap->SetMinimumRGBComponentValue(0);
    colormap->SetMaximumRGBComponentValue(255);


    colorMapImage  = rgbImageType::New();
    colorMapImage->SetRegions(densityImage->GetRequestedRegion());
    colorMapImage->Allocate();
    colorMapImage->FillBuffer( itk::NumericTraits<rgbPixelType>::Zero+255);



    itk::ImageRegionConstIterator< grayImageType > inputIt(densityImage, densityImage->GetRequestedRegion());
    itk::ImageRegionIterator< rgbImageType >     outputIt(colorMapImage, colorMapImage->GetRequestedRegion());

    while ( !inputIt.IsAtEnd() )
    {
        if(inputIt.Get() > 0) //lowest density
        {
            outputIt.Set( colormap->operator()( inputIt.Get() ) );
        }

        ++inputIt;
        ++outputIt;
    }


    IO::printOK("Density to colormap");

    if(showResult)
    {
        VTKViewer::visualize<rgbImageType>(colorMapImage, "Colormap");
    }



}

/*

*/



typename ROIExtractor::grayImageP
ROIExtractor::otsuThreshold()
{


    using otsuType = itk::OtsuThresholdImageFilter< grayImageType, grayImageType >;
    typename otsuType::Pointer otsuFilter = otsuType::New();
    otsuFilter->SetInput(grayImage);
    otsuFilter->SetOutsideValue(BACKGROUND);
    otsuFilter->SetInsideValue(FOREGROUND);

    otsuFilter->Update();

    VTKViewer::visualize<grayImageType>(otsuFilter->GetOutput(), "Otsu");

    return otsuFilter->GetOutput();

}



void ROIExtractor::rgbToGrayImage()
{
    using rgbToLuminanceFilterType = itk::RGBToLuminanceImageFilter< rgbImageType, grayImageType >;
    typename rgbToLuminanceFilterType::Pointer rgbToLuminancefilter = rgbToLuminanceFilterType::New();
    rgbToLuminancefilter->SetInput( inputImage );
    rgbToLuminancefilter->Update();
    grayImage = rgbToLuminancefilter->GetOutput();

}


typename ROIExtractor::rgbImageP
ROIExtractor::getColorMap() const
{
     return colorMapImage;
}


void ROIExtractor::setDensityThreshold(pixelComponentT threshold)
{
    densityThreshold = threshold;
}


void ROIExtractor::computeConnectedComponents(bool showResult)
{

    //divide density into high and low density images
    grayImageP highDensity, lowDensity;
    divideDensityIntoHighAndLow(highDensity, lowDensity);

    //delete it
    //VTKViewer<pixelComponentT>::visualizeGray(highDensity, "High density");
    //VTKViewer<pixelComponentT>::visualizeGray(lowDensity, "Low density");

    //connected components
    using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter <grayImageType, grayImageType>;
    typename ConnectedComponentImageFilterType::Pointer connectedComponentImageFilter =   ConnectedComponentImageFilterType::New ();
    //connected->SetBackgroundValue(255);
    connectedComponentImageFilter->SetInput(highDensity);


    //labelImage to labelMap
    using LabelImageToLabelMapFilterType = itk::LabelImageToLabelMapFilter <grayImageType>;
    typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
    labelImageToLabelMapFilter->SetInput(connectedComponentImageFilter->GetOutput());
    labelImageToLabelMapFilter->Update();
    connectedComponents = labelImageToLabelMapFilter->GetOutput();


    IO::printOK("Computing connected components");

    if(showResult)
    {
        using RGBFilterType = itk::LabelToRGBImageFilter<grayImageType, rgbImageType>;
        typename RGBFilterType::Pointer rgbFilter = RGBFilterType::New();
        rgbFilter->SetInput( connectedComponentImageFilter->GetOutput() );
        rgbFilter->Update();
        VTKViewer::visualize<rgbImageType>(rgbFilter->GetOutput(), "Connected components");
    }




}



void ROIExtractor::divideDensityIntoHighAndLow(grayImageP &highDensity, grayImageP &lowDensity)
{

    highDensity  = grayImageType::New();
    lowDensity  = grayImageType::New();

    highDensity->SetRegions(densityImage->GetRequestedRegion());
    highDensity->Allocate();
    highDensity->FillBuffer(itk::NumericTraits<pixelComponentT>::Zero);

    lowDensity->SetRegions(densityImage->GetRequestedRegion());
    lowDensity->Allocate();
    lowDensity->FillBuffer(itk::NumericTraits<pixelComponentT>::Zero);

    itk::ImageRegionConstIterator< grayImageType > inputIt(densityImage, densityImage->GetRequestedRegion());
    itk::ImageRegionIterator< grayImageType > highIt(highDensity, highDensity->GetRequestedRegion());
    itk::ImageRegionIterator< grayImageType > lowIt(lowDensity, lowDensity->GetRequestedRegion());

    //aux var
    auto pixelValue = inputIt.Get();

    while (!inputIt.IsAtEnd())
    {

        pixelValue = inputIt.Get();
        //std::cout<<pixelValue<<std::endl;

        if(pixelValue >= densityThreshold)
        {
            highIt.Set(pixelValue);
        }
        else if (pixelValue > 0) // no zero density
        {
            lowIt.Set(pixelValue) ;
        }

        ++inputIt;
        ++highIt;
        ++lowIt;
    }

    //VTKViewer<pixelComponentT>::visualizeGray(highDensity, "High density");
    //VTKViewer<pixelComponentT>::visualizeGray(lowDensity, "Low density");

}


typename ROIExtractor::labelMapP
ROIExtractor::getConnectedComponents() const
{
    return connectedComponents;
}


typename ROIExtractor::grayImageP
ROIExtractor::getGrayImage() const
{
    return grayImage;
}


























