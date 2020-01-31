#include "cellsegmentator.h"


template<typename imageT>
CellSegmentator<imageT>::CellSegmentator()
{

}

template<typename imageT>
void CellSegmentator<imageT>::setImage(imageP inputImage)
{

    this->inputImage = inputImage;

}


template<typename imageT>
void CellSegmentator<imageT>::computeGradients()
{

    // Create and setup a gradient filter
    if(grayImage.IsNull())
    {
        createGrayImage();
    }


    using gradientFilterT = itk::GradientImageFilter<grayImageT, float>;
    typename gradientFilterT::Pointer gradientFilter = gradientFilterT::New();
    gradientFilter->SetInput( grayImage );
    gradientFilter->Update();
    outputImage = gradientFilter->GetOutput();

    IO::printOK("Computing Gradients");

}

template<typename imageT>
typename CellSegmentator<imageT>::vectorImageP
CellSegmentator<imageT>::getGradients() const
{
    return outputImage;
}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::superPixels()
{

    using superPixelsT =  SuperPixels<rgbImageT>;

    std::unique_ptr<superPixelsT> superPixels(new superPixelsT());
    superPixels = std::make_unique<superPixelsT>();


    //rescale to 0 - 1
    using rescaleFilterType = itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(blurImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();




    using FilterType = itk::CastImageFilter<grayImageT, rgbImageT>;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(rescaleFilter->GetOutput());
    filter->Update();

    superPixels->setImage(filter->GetOutput()); //input image
    //superPixels->setImage(blurImage); //input image

    /*
    std::unique_ptr<QuadTree<grayImageT>> quadTree(new QuadTree<grayImageT>());

    quadTree->setImage(cellNuclei);
    quadTree->build();

    superPixels->setInitialGrid(quadTree->getLabelImage());
    superPixels->setSpNumber(quadTree->getLeavesNumber());

*/
    superPixels->create();
    superPixels->show();


    labelImage = superPixels->getLabelImage();

    extractCellsFromSuperPixels();


     IO::printOK("Creating Super Pixels");

}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCellNuclei()
{

    //CIELAB image type
    using labPixelT = itk::RGBPixel<float>;
    using labImageT = itk::Image<labPixelT, 2>;

    //RGB to Lab
    using rgbToXyzFilterT = ColorConverterFilter<rgbImageT, labImageT>;
    std::unique_ptr< rgbToXyzFilterT> rgbToXyzFilter(new rgbToXyzFilterT());
    rgbToXyzFilter->setInput(inputImage);
    rgbToXyzFilter->rgbToXyz();

    using xyzToLabFilterT = ColorConverterFilter<labImageT, labImageT>;
    std::unique_ptr< xyzToLabFilterT> xyzToLabFilter(new xyzToLabFilterT());
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();

    auto labImage = xyzToLabFilter->getOutput();


    //Extract B channel
    using ExtractChannelFilterT = ExtractChannelFilter< labImageT, floatImageT>;
    std::unique_ptr<ExtractChannelFilterT> extractChannelFilter(new ExtractChannelFilterT());

    extractChannelFilter->setImputImage(labImage);
    extractChannelFilter->extractChannel(2);



    //rescale to 0 - 1
    using rescaleFilterType = itk::RescaleIntensityImageFilter<floatImageT, floatImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(extractChannelFilter->getOutputImage());
    rescaleFilter->SetOutputMinimum(0.f);
    rescaleFilter->SetOutputMaximum(1.f);
    rescaleFilter->Update();



    //laplacian
    using smoothFilterT = itk::SmoothingRecursiveGaussianImageFilter<floatImageT, floatImageT>;
    smoothFilterT::Pointer smoothFilter = smoothFilterT::New();
    smoothFilter->SetNormalizeAcrossScale(false);
    smoothFilter->SetInput(rescaleFilter->GetOutput());
    smoothFilter->SetSigma(10);
    smoothFilter->Update();

    blurImage = smoothFilter->GetOutput();

    //visualizing
    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
    rescaleFilter2->SetInput(rescaleFilter->GetOutput());
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);
    rescaleFilter2->Update();

    VTKViewer::visualize<grayImageT>(rescaleFilter2->GetOutput() ,"b channel Image");




    //visualizing
    using rescaleFilterType3= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType3::Pointer rescaleFilter3 = rescaleFilterType3::New();
    rescaleFilter3->SetInput(blurImage);
    rescaleFilter3->SetOutputMinimum(0);
    rescaleFilter3->SetOutputMaximum(255);
    rescaleFilter3->Update();

    VTKViewer::visualize<grayImageT>(rescaleFilter3->GetOutput() ,"Smooth b channel Image");




    //computeLocalMinimum();

    //TO-DO delete this
    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter< rgbImageT, grayImageT >;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    VTKViewer::visualize<grayImageT>(rgbToGrayFilter->GetOutput() ,"Gray-scale image");





}


template<typename imageT>
void CellSegmentator<imageT>::createGrayImage()
{
    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter< imageT, grayImageT >;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    grayImage = rgbToGrayFilter->GetOutput();


}





template<typename imageT>
void CellSegmentator<imageT>::computeLocalMinimum()
{

/*

    itk::ImageRegionConstIterator<grayImageT> grayIt(grayImage, grayImage->GetRequestedRegion());
    itk::ImageRegionConstIterator<grayImageT> blurIt(blurImage, blurImage->GetRequestedRegion());


    multiplyImage = grayImageT::New();
    multiplyImage->SetRegions(grayImage->GetRequestedRegion());
    multiplyImage->Allocate();
   // multiplyImage->FillBuffer(sigmaMin);

    itk::ImageRegionIterator<grayImageT> surfIt(multiplyImage, multiplyImage->GetRequestedRegion());


    while(!grayIt.IsAtEnd())
    {

        //std::cout<<grayIt.Get()<<" * "<<blurIt.Get()<<": "<<grayIt.Get()*blurIt.Get()<<std::endl;
        surfIt.Set(static_cast<unsigned>((static_cast<double>(grayIt.Get())/255 * static_cast<double>(blurIt.Get())/255)*255));

        ++grayIt;
        ++blurIt;
        ++surfIt;

    }
*/
  /*  using rescaleFilterType = itk::RescaleIntensityImageFilter<grayImageDoubleT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
    rescaleFilter2->SetInput(surface);
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);
    rescaleFilter2->Update();
*/
    //VTKViewer::visualize<grayImageT>(multiplyImage ,"multiply");


    //showing results
/*
    using rescaleFilterType = itk::RescaleIntensityImageFilter<grayImageT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(eqImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();
*/

    using RegionalMinimaImageFilter = itk::RegionalMinimaImageFilter<floatImageT, grayImageT>;
    RegionalMinimaImageFilter::Pointer regionalMinimaFilter = RegionalMinimaImageFilter::New();
    regionalMinimaFilter->SetInput(blurImage);
    regionalMinimaFilter->SetBackgroundValue(0);
    regionalMinimaFilter->SetForegroundValue(255);

    regionalMinimaFilter->Update();


    VTKViewer::visualize<grayImageT>(regionalMinimaFilter->GetOutput() ,"Seeds");


    return;

    using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter<grayImageT, grayImageT>;
    ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New();
    connected->SetInput(regionalMinimaFilter->GetOutput());
    connected->Update();

    std::vector<std::vector<long>> labels(connected->GetObjectCount(), std::vector<long>(3,0));


    //components const iterator
    itk::ImageRegionConstIteratorWithIndex<grayImageT> compIt(connected->GetOutput(), connected->GetOutput()->GetRequestedRegion());

    for(;!compIt.IsAtEnd(); ++compIt)
    {
        const auto & label = compIt.Get();
        const auto & index = compIt.GetIndex();
        if(label > 0)
        {
            labels[label-1][0] += index[0];
            labels[label-1][1] += index[1];

            ++labels[label-1][2];
        }

    }


    //computing index centroids
    for(auto it = labels.begin(); it != labels.end();++it)
    {

        (*it)[0] /= (*it)[2];
        (*it)[1] /= (*it)[2];

    }

    cellNuclei = grayImageT::New();
    cellNuclei->SetRegions(grayImage->GetRequestedRegion());
    cellNuclei->Allocate();
    cellNuclei->FillBuffer(0);

    //cell nuclei iterator
    itk::ImageRegionIteratorWithIndex<grayImageT> cnIt(cellNuclei, cellNuclei->GetRequestedRegion());


    for(auto it = labels.begin(); it != labels.end();++it)
    {

        const grayImageT::IndexType& index = {{ (*it)[0] , (*it)[1]  }};

        cnIt.SetIndex(index);
        cnIt.Set(255);

    }



    VTKViewer::visualize<grayImageT>(cellNuclei ,"Seeds");

/*

    //visualising

    using rgbPixelChar = itk::RGBPixel< unsigned char >;
    using rgbImageChar = itk::Image< rgbPixelChar, 2 >;

    using toColormapFilterType = itk::ScalarToRGBColormapImageFilter<grayImageT, rgbImageChar>;
    toColormapFilterType::Pointer toColormapFilter = toColormapFilterType::New();
    toColormapFilter->SetInput(cellNuclei);
    toColormapFilter->SetColormap(toColormapFilterType::Jet);
    toColormapFilter->Update();



    using castFilterType = itk::CastImageFilter<rgbImageChar, imageT>;
    typename castFilterType::Pointer castfilter = castFilterType::New();
    castfilter->SetInput(toColormapFilter->GetOutput());
    castfilter->Update();

    using overlayRGBImageFilterT = OverlayRGBImageFilter<imageT>;
    std::unique_ptr<overlayRGBImageFilterT> overlayRGBImageFilter(new overlayRGBImageFilterT);
    overlayRGBImageFilter->setBackgroundImage(inputImage);
    overlayRGBImageFilter->setForegroundImage(castfilter->GetOutput());
    overlayRGBImageFilter->setForegroundAlpha(0.5f);
    overlayRGBImageFilter->alphaBlending();


    VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"Surface");
    VTKViewer::visualize<rgbImageChar>(toColormapFilter->GetOutput() ,"Color map");
    VTKViewer::visualize<imageT>(overlayRGBImageFilter->getOutput() ,"Overlay");
*/
    IO::printOK("Computing Cell Nuclei");


}



template<typename rgbImageT>
void CellSegmentator<rgbImageT>::extractCellsFromSuperPixels()
{


    using LabelObjectType = itk::LabelObject<unsigned, 2>;
    using LabelMapType = itk::LabelMap<LabelObjectType>;


    using LabelImage2LabelMapType = itk::LabelImageToLabelMapFilter<grayImageT, LabelMapType>;
    typename LabelImage2LabelMapType::Pointer convert = LabelImage2LabelMapType::New();
    convert->SetInput(labelImage);
    convert->Update();
    auto labelMap = convert->GetOutput();

    using labelMapToRGBFilterT = itk::LabelMapToRGBImageFilter<LabelMapType, rgbImageT>;
    typename labelMapToRGBFilterT::Pointer  labelMapToRGB = labelMapToRGBFilterT::New();
    labelMapToRGB->SetInput(labelMap);
    labelMapToRGB->Update();

    VTKViewer::visualize<rgbImageT>(labelMapToRGB->GetOutput() ,"SP");


    //TODO delet labels


    itk::ImageRegionConstIteratorWithIndex<grayImageT> labelIt(labelImage, labelImage->GetRequestedRegion());
    itk::ImageRegionConstIterator<grayImageT> nucleiIt(cellNuclei, cellNuclei->GetRequestedRegion());

    std::vector<unsigned> nucleiLabels;

    for(nucleiIt.GoToBegin(); !nucleiIt.IsAtEnd(); ++nucleiIt, ++labelIt)
    {

            if(nucleiIt.Get() == 255) //white
            {
                nucleiLabels.push_back(labelIt.Get());

            }
    }
    labelIt.GoToBegin();


    itk::ImageRegionConstIteratorWithIndex<rgbImageT> inputIt(inputImage, inputImage->GetRequestedRegion());


    auto cells = rgbImageT::New();
    cells->SetRegions(inputImage->GetRequestedRegion());
    cells->Allocate();


    itk::ImageRegionIterator<rgbImageT> cellsIt(cells, cells->GetRequestedRegion());


    pixelCompT red;
    red.SetRed(255);

    while(!labelIt.IsAtEnd())
    {
        //
        if(std::find(nucleiLabels.begin(), nucleiLabels.end(), labelIt.Get()) != nucleiLabels.end())
        {

            cellsIt.Set(red);

        }
        else
        {
            cellsIt.Set(inputIt.Get());

        }
        ++labelIt;
        ++inputIt;
        ++cellsIt;

    }


    VTKViewer::visualize<rgbImageT>(cells ,"Cells");

}










