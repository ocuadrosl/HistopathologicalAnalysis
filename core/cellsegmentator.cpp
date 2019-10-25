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
       logFilter->compute();

       multiplyFilterT::Pointer multiplyFilter = multiplyFilterT::New();
       multiplyFilter->SetInput(logFilter->getOutput());
       multiplyFilter->SetConstant(sigma*sigma);
       multiplyFilter->Update();

       LogNorm.push_back(multiplyFilter->GetOutput());

       //itk::ViewImage<grayImageD>::View(logFilter->getOutput(), "sigma");
       //itk::ViewImage<grayImageD>::View(multiplyFilter->GetOutput(), "sigma 2");

    }


}


template<typename imageT>
void CellSegmentator<imageT>::computeEuclideanMap()
{


    using otsuType = itk::OtsuThresholdImageFilter< grayImageT, grayImageT >;
    typename otsuType::Pointer otsuFilter = otsuType::New();
    otsuFilter->SetInput(grayImage);
    otsuFilter->SetOutsideValue(255);
    otsuFilter->SetInsideValue(0);

    otsuFilter->Update();

    //itk::ViewImage<grayImageT>::View(otsuFilter->GetOutput(), "otsu");


    using signedMaurerDistanceMapImageFilterT =   itk::SignedMaurerDistanceMapImageFilter<grayImageT, grayImageDoubleT>;
    signedMaurerDistanceMapImageFilterT::Pointer distanceMapImageFilter =   signedMaurerDistanceMapImageFilterT::New();
    distanceMapImageFilter->SetInput(otsuFilter->GetOutput());
    distanceMapImageFilter->InsideIsPositiveOn();

    distanceMapImageFilter->Update();
    euclideanMap = distanceMapImageFilter->GetOutput();
    itk::ViewImage<grayImageDoubleT>::View(euclideanMap, "Euclidean Map");


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


    /*using FilterType = itk::RescaleIntensityImageFilter<grayImageD, grayImageD>;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(surface);
    filter->SetOutputMinimum(0);
    filter->SetOutputMaximum(255);
*/
    itk::ViewImage<grayImageDoubleT>::View(surface ,"Surface");

    IO::printOK("Computing Surface");




}


template<typename imageT>
inline double CellSegmentator<imageT>::computeSigmaMAX(imageDoubleIt it)
{

    return std::max(sigmaMin, std::min(sigmaMax, 2*it.Get()));
}












