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


template<typename imageT>
void CellSegmentator<imageT>::superPixels()
{


    using superPixelsT =  SuperPixels<imageT>;

    std::unique_ptr<superPixelsT> superPixels(new superPixelsT());

    superPixels->setImage(inputImage);
    superPixels->create();

     IO::printOK("Creating Super Pixels");

}

template<typename imageT>
void CellSegmentator<imageT>::findCellNuclei()
{

    if(grayImage.IsNull())
    {
        createGrayImage();
    }

    computeLoGNorm();
    computeEuclideanMap();
    computeSurface();

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

    LogNorm.clear();

    //computing LoG for various sigmas

    using logFilterT = LoGFilter<grayImageT, grayImageDoubleT>;
    std::unique_ptr<logFilterT> logFilter(new logFilterT);
    logFilter->setImage(grayImage);



    using multiplyFilterT = itk::MultiplyImageFilter<grayImageDoubleT, grayImageDoubleT, grayImageDoubleT>;





    for(double sigma=sigmaMin; sigma <= sigmaMax; sigma += stepSize)
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


    using cellBinarizationFilterT = CellBinarizationFilter<rgbImageT>;
    std::unique_ptr<cellBinarizationFilterT> cellBinarizationF(new cellBinarizationFilterT);
    cellBinarizationF->setImage(inputImage);
    cellBinarizationF->compute();




    itk::ViewImage<grayImageT>::View(cellBinarizationF->getOutput(), "Threshold");


    using signedMaurerDistanceMapImageFilterT =   itk::SignedMaurerDistanceMapImageFilter<grayImageT, grayImageDoubleT>;
    signedMaurerDistanceMapImageFilterT::Pointer distanceMapImageFilter =   signedMaurerDistanceMapImageFilterT::New();
    distanceMapImageFilter->SetInput(cellBinarizationF->getOutput());
    distanceMapImageFilter->InsideIsPositiveOn();

    distanceMapImageFilter->Update();
    euclideanMap = distanceMapImageFilter->GetOutput();
    //itk::ViewImage<grayImageDoubleT>::View(euclideanMap, "Euclidean Map");


}


template<typename imageT>
void CellSegmentator<imageT>::computeSurface()
{


    imageDoubleIt  mapIt(euclideanMap, euclideanMap->GetRequestedRegion());
    itk::ImageRegionConstIterator<grayImageT>  grayIt(grayImage, grayImage->GetRequestedRegion());


    surface = grayImageDoubleT::New();
    surface->SetRegions(grayImage->GetRequestedRegion());
    surface->Allocate();
    surface->FillBuffer(sigmaMin);

    itk::ImageRegionIterator<grayImageDoubleT> surfIt(surface, surface->GetRequestedRegion());


    std::vector<imageDoubleIt> normIts;
    for(auto logIt = LogNorm.begin(); logIt != LogNorm.end(); ++logIt)
    {
        normIts.push_back(imageDoubleIt(*logIt , (*logIt)->GetRequestedRegion()));
    }


    using ImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<grayImageDoubleT>;



    for(mapIt.GoToBegin(); !mapIt.IsAtEnd(); ++mapIt, ++grayIt, ++surfIt)
    {

        double multTmp=0;
        double sigmaTmp = 0;
        double sigmaMax = computeSigmaMAX(mapIt);

        auto vecIt = normIts.begin();

        double maxTmp = (*vecIt).Get()*grayIt.Get();
        for(double sigma = sigmaMin; sigma <= sigmaMax; sigma += stepSize, ++vecIt)
        {

            multTmp = (*vecIt).Get() * grayIt.Get();

            if(multTmp > maxTmp)
            {
                sigmaTmp = sigma;
                maxTmp = multTmp;
            }

        }

        surfIt.Set(sigmaTmp);


    }

    //showing results

    using rescaleFilterType = itk::RescaleIntensityImageFilter<grayImageDoubleT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(surface);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();


    itk::ImageRegionIterator<grayImageT>  it(rescaleFilter->GetOutput(), rescaleFilter->GetOutput()->GetRequestedRegion());
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        //it.Set(255-it.Get());

    }

    using rgbPixelChar = itk::RGBPixel< unsigned char >;
    using rgbImageChar = itk::Image< rgbPixelChar, 2 >;

    using toColormapFilterType = itk::ScalarToRGBColormapImageFilter<grayImageT, rgbImageChar>;
    toColormapFilterType::Pointer toColormapFilter = toColormapFilterType::New();
    toColormapFilter->SetInput(rescaleFilter->GetOutput());
    toColormapFilter->SetColormap(toColormapFilterType::Hot);
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


    VTKViewer::visualize<rgbImageChar>(toColormapFilter->GetOutput() ,"Surface");
    VTKViewer::visualize<imageT>(overlayRGBImageFilter->getOutput() ,"Surface");

    IO::printOK("Computing Surface");




}


template<typename imageT>
inline double CellSegmentator<imageT>::computeSigmaMAX(imageDoubleIt it)
{

    return std::max(sigmaMin, std::min(sigmaMax, 2*it.Get()));
}












