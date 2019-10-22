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

    using logFilterT = LoGFilter<grayImageT, grayImageD>;
    std::unique_ptr<logFilterT> logFilter(new logFilterT);
    logFilter->setImage(grayImage);



    using multiplyFilterT = itk::MultiplyImageFilter<grayImageD, grayImageD, grayImageD>;
    multiplyFilterT::Pointer multiplyFilter = multiplyFilterT::New();



    for(double sigma=sigmaMin; sigma<= sigmaMax; sigma += 0.1)
    {

       logFilter->setSigma(sigma);
       logFilter->compute();

       multiplyFilter->SetInput(logFilter->getOutput());
       multiplyFilter->SetConstant(sigma*sigma);

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



}







