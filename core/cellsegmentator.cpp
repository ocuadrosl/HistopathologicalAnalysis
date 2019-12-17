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



    std::unique_ptr<superPixelsT> superPixels(new superPixelsT());
    superPixels = std::make_unique<superPixelsT>();


    using FilterType = itk::CastImageFilter<grayImageT, rgbImageT>;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(multiplyImage);
    filter->Update();

    superPixels->setImage(filter->GetOutput()); //input image
    //superPixelsP->setImage(inputImage); //input image



    std::unique_ptr<QuadTree<grayImageT>> quadTree(new QuadTree<grayImageT>());

    quadTree->setImage(cellNuclei);
    quadTree->build();

    superPixels->setInitialGrid(quadTree->getLabelImage());
    superPixels->setSpNumber(quadTree->getLeavesNumber());


    superPixels->create();
    superPixels->show();


    labelImage = superPixels->getLabelImage();

    extractCellsFromSuperPixels();


     IO::printOK("Creating Super Pixels");

}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCellNuclei()
{

    //delete it
    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter<rgbImageT, grayImageT >;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    grayImage = rgbToGrayFilter->GetOutput();




    // my own implementation
    using cellBinarizationFilterT = CellBinarizationFilter<rgbImageT>;
    std::unique_ptr<cellBinarizationFilterT> cellBinarizationF(new cellBinarizationFilterT);
    cellBinarizationF->setImage(inputImage);
    cellBinarizationF->compute();

    blurMaskImage = cellBinarizationF->getBlurMaskImage();
    blurImage = cellBinarizationF->getBlurImage();
    eqImage = cellBinarizationF->getEqualizedImage();
    computeLocalMinimum();


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
void CellSegmentator<imageT>::computeLoGNorm()
{

    /*
    LogNorm.clear();

    //computing LoG for various sigmas

    using logFilterT = LoGFilter<grayImageT, grayImageDoubleT>;
    std::unique_ptr<logFilterT> logFilter(new logFilterT);
    logFilter->setImage(grayImage);

    using multiplyFilterT = itk::MultiplyImageFilter<grayImageDoubleT, grayImageDoubleT, grayImageDoubleT>;


    for(float sigma=sigmaMin; sigma <= sigmaMax; sigma += stepSize)
    {

       logFilter->setSigma(sigma);
       logFilter->setKernelSize(kernelSize);
       logFilter->compute();

       multiplyFilterT::Pointer multiplyFilter = multiplyFilterT::New();
       multiplyFilter->SetInput(logFilter->getOutput());
       multiplyFilter->SetConstant(sigma*sigma);
       multiplyFilter->Update();

       LogNorm.push_back(multiplyFilter->GetOutput());

       //itk::ViewImage<grayImageD>::View(logFilter->getOutput(), "sigma");
       //itk::ViewImage<grayImageDoubleT>::View(multiplyFilter->GetOutput(), "sigma 2");

    }
*/

}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::computeEuclideanMap()
{


    using LiFilterType             = itk::LiThresholdImageFilter<grayImageT, grayImageT>;
    using HuangFilterType          = itk::HuangThresholdImageFilter<grayImageT, grayImageT>;
    using IntermodesFilterType     = itk::IntermodesThresholdImageFilter<grayImageT, grayImageT>;
    using IsoDataFilterType        = itk::IsoDataThresholdImageFilter<grayImageT, grayImageT>;
    using KittlerIllingworthFilterType = itk::KittlerIllingworthThresholdImageFilter<grayImageT, grayImageT>;
    using LiFilterType             = itk::LiThresholdImageFilter<grayImageT, grayImageT>;
    using MaximumEntropyFilterType = itk::MaximumEntropyThresholdImageFilter<grayImageT, grayImageT>;
    using MomentsFilterType        = itk::MomentsThresholdImageFilter<grayImageT, grayImageT>;
    using OtsuFilterType           = itk::OtsuThresholdImageFilter<grayImageT, grayImageT>;
    using RenyiEntropyFilterType   = itk::RenyiEntropyThresholdImageFilter<grayImageT, grayImageT>;
    using ShanbhagFilterType       = itk::ShanbhagThresholdImageFilter<grayImageT, grayImageT>;
    using TriangleFilterType       = itk::TriangleThresholdImageFilter<grayImageT, grayImageT>;
    using YenFilterType            = itk::YenThresholdImageFilter<grayImageT, grayImageT>;

    using FilterContainerType =  std::map<std::string, itk::HistogramThresholdImageFilter<grayImageT, grayImageT>::Pointer>;
    FilterContainerType filterContainer;

    filterContainer["Huang"] = HuangFilterType::New();
    filterContainer["Intermodes"] = IntermodesFilterType::New();
    filterContainer["IsoData"] = IsoDataFilterType::New();
    filterContainer["KittlerIllingworth"] = KittlerIllingworthFilterType::New();
    filterContainer["Li"] = LiFilterType::New();
    filterContainer["MaximumEntropy"] = MaximumEntropyFilterType::New();
    filterContainer["Moments"] = MomentsFilterType::New();
    filterContainer["Otsu"] = OtsuFilterType::New();
    filterContainer["RenyiEntropy"] = RenyiEntropyFilterType::New();
    filterContainer["Shanbhag"] = ShanbhagFilterType::New();
    filterContainer["Triangle"] = TriangleFilterType::New();
    filterContainer["Yen"] = YenFilterType::New();

    std::string filterName = "MaximumEntropy";
    filterContainer[filterName]->SetInsideValue(0);
    filterContainer[filterName]->SetOutsideValue(1);
    filterContainer[filterName]->SetInput(grayImage);
    filterContainer[filterName]->Update();


    // my own implementation
    using cellBinarizationFilterT = CellBinarizationFilter<rgbImageT>;
    std::unique_ptr<cellBinarizationFilterT> cellBinarizationF(new cellBinarizationFilterT);
    cellBinarizationF->setImage(inputImage);
    cellBinarizationF->compute();

    blurMaskImage = cellBinarizationF->getBlurMaskImage();


/*
    using signedMaurerDistanceMapImageFilterT =   itk::SignedMaurerDistanceMapImageFilter<grayImageT, grayImageDoubleT>;
    signedMaurerDistanceMapImageFilterT::Pointer distanceMapImageFilter =   signedMaurerDistanceMapImageFilterT::New();
    distanceMapImageFilter->SetInput(cellBinarizationF->getBinaryImage());
    distanceMapImageFilter->InsideIsPositiveOn();

    distanceMapImageFilter->Update();
    euclideanMap = distanceMapImageFilter->GetOutput();
    //itk::ViewImage<grayImageDoubleT>::View(euclideanMap, "Euclidean Map");
*/

}


template<typename imageT>
void CellSegmentator<imageT>::computeLocalMinimum()
{



    itk::ImageRegionConstIterator<grayImageT> grayIt(grayImage, grayImage->GetRequestedRegion());
    itk::ImageRegionConstIterator<grayImageT> blurIt(eqImage, eqImage->GetRequestedRegion());


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

  /*  using rescaleFilterType = itk::RescaleIntensityImageFilter<grayImageDoubleT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
    rescaleFilter2->SetInput(surface);
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);
    rescaleFilter2->Update();
*/
    VTKViewer::visualize<grayImageT>(multiplyImage ,"multiply");


    //showing results
/*
    using rescaleFilterType = itk::RescaleIntensityImageFilter<grayImageT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(eqImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();
*/

    using RegionalMinimaImageFilter = itk::RegionalMinimaImageFilter<grayImageT, grayImageT>;
    RegionalMinimaImageFilter::Pointer regionalMinimaFilter = RegionalMinimaImageFilter::New();
    regionalMinimaFilter->SetInput(blurMaskImage);
    regionalMinimaFilter->SetBackgroundValue(0);
    regionalMinimaFilter->SetForegroundValue(255);

    regionalMinimaFilter->Update();

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










