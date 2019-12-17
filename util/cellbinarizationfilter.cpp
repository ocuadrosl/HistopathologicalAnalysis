#include "cellbinarizationfilter.h"

template<typename rgbImageT>
CellBinarizationFilter<rgbImageT>::CellBinarizationFilter()
{

}
template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::setImage(rgbImageP inputImage)
{
    this->inputImage = inputImage;

    //rgb to gray-scale
    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter<rgbImageT, grayImageT>;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();

}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::gaussianBlur()
{

 /*   using FilterType = itk::DiscreteGaussianImageFilter<grayImageT, floatImageT>;
    FilterType::Pointer smoothFilter = FilterType::New();

    smoothFilter->SetVariance(sigma);
    smoothFilter->SetMaximumKernelWidth(17);
    smoothFilter->SetInput(grayImage);

    smoothFilter->Update();

    //float to unsigned
    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
    rescaleFilter2->SetInput(smoothFilter->GetOutput());
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);


    logImage = rescaleFilter2->GetOutput();


    VTKViewer::visualize<grayImageT>(logImage ,"blur Image");
*/


    //Laplacian of Gaussian
    using floatImageT = itk::Image<float,2>;

    //unsigned to float
    using rescaleFilterType = itk::RescaleIntensityImageFilter<grayImageT, floatImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(grayImage);
    rescaleFilter->SetOutputMinimum(0.f);
    rescaleFilter->SetOutputMaximum(1.f);


    std::unique_ptr<LoGFilter<floatImageT,floatImageT>> logFilter(new LoGFilter<floatImageT,floatImageT>());
    logFilter->setImage(rescaleFilter->GetOutput());
    logFilter->setSigma(0.25f);
    logFilter->setKernelSize(51);
    logFilter->compute();


    //VTKViewer::visualize<floatImageT>(logFilter->getOutput() ,"LoG float Image");

    //float to unsigned
    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
    rescaleFilter2->SetInput(logFilter->getOutput());
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);


    logImage = rescaleFilter2->GetOutput();


    VTKViewer::visualize<grayImageT>(logImage ,"LoG Image");


}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::histogramEqualization()
{

/*
    //local equalization
    using adaptiveHistogramEqualizationFilterT = itk::AdaptiveHistogramEqualizationImageFilter<grayImageT>;
    adaptiveHistogramEqualizationFilterT::Pointer adaptiveHistogramEqualizationFilter = adaptiveHistogramEqualizationFilterT::New();



    adaptiveHistogramEqualizationFilter->SetAlpha(0);
    adaptiveHistogramEqualizationFilter->SetBeta(0);

     // int                                                         radiusSize = std::stoi(argv[5]);
    adaptiveHistogramEqualizationFilterT::ImageSizeType radius;
    radius.Fill(100);
    adaptiveHistogramEqualizationFilter->SetRadius(radius);
    adaptiveHistogramEqualizationFilter->SetInput(logImage);
    adaptiveHistogramEqualizationFilter->Update();

    equalizedImage = adaptiveHistogramEqualizationFilter->GetOutput();


    VTKViewer::visualize<grayImageT>(equalizedImage ,"local equalization");
*/


    using ImageCalculatorFilterT = itk::MinimumMaximumImageCalculator<grayImageT>;

    ImageCalculatorFilterT::Pointer imageCalculatorF = ImageCalculatorFilterT::New();
    imageCalculatorF->SetImage(logImage);
    imageCalculatorF->Compute();


    equalizedImage = grayImageT::New();
    equalizedImage->SetRegions(logImage->GetRequestedRegion());
    equalizedImage->Allocate();

    itk::ImageRegionIterator<grayImageT> itB(logImage, logImage->GetRequestedRegion());
    itk::ImageRegionIterator<grayImageT> itE(equalizedImage, equalizedImage->GetRequestedRegion());

    std::cout<<imageCalculatorF->GetMinimum()<< ","<<imageCalculatorF->GetMaximum()<<std::endl;

    Math::MinMax<unsigned, unsigned> minMax(imageCalculatorF->GetMinimum(), imageCalculatorF->GetMaximum(), 0, 255);

    while(!itB.IsAtEnd())
    {

        itE.Set(minMax(itB.Get()));

        ++itB;
        ++itE;
    }


    VTKViewer::visualize<grayImageT>(equalizedImage ,"equalization");



}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::computeHistogram()
{


    using ImageToHistogramFilterType = itk::Statistics::ImageToHistogramFilter<grayImageT>;

    typename ImageToHistogramFilterType::HistogramType::MeasurementVectorType lowerBound(numberOfBins);
    lowerBound.Fill(0);

    typename ImageToHistogramFilterType::HistogramType::MeasurementVectorType upperBound(numberOfBins);
    upperBound.Fill(255);


    typename ImageToHistogramFilterType::HistogramType::SizeType size(1); //gray-scale
    size.Fill(numberOfBins);



    typename ImageToHistogramFilterType::Pointer imageToHistogramFilter = ImageToHistogramFilterType::New();
    imageToHistogramFilter->SetInput(equalizedImage);
    imageToHistogramFilter->SetHistogramBinMinimum(lowerBound);
    imageToHistogramFilter->SetHistogramBinMaximum(upperBound);
    imageToHistogramFilter->SetHistogramSize(size);
    imageToHistogramFilter->Update();

    typename ImageToHistogramFilterType::HistogramType  *frequency = imageToHistogramFilter->GetOutput();


    //std::cout << "f = [ ";
    for (unsigned int i = 0; i < frequency->GetSize()[0]; ++i)
    {
        //std::cout << frequency->GetFrequency(i);
        histogram.push_back(frequency->GetFrequency(i));

        if (i != frequency->GetSize()[0] - 1)
        {
            //  std::cout << ", ";
        }
    }

    //std::cout << " ]" << std::endl;





}



template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::binaryThreholding()
{
    using thresholdFilterType = itk::BinaryThresholdImageFilter<grayImageT, grayImageT>;

    /*
    thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
    thresholdFilter->SetInput(equalizedImage);
    thresholdFilter->SetLowerThreshold(0);
    thresholdFilter->SetUpperThreshold(threshold);
    thresholdFilter->SetOutsideValue(255);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->Update();

    binaryImage  = thresholdFilter->GetOutput();
*/


    //blur mask
    thresholdFilterType::Pointer thresholdFilter2 = thresholdFilterType::New();
    thresholdFilter2->SetInput(equalizedImage);
    thresholdFilter2->SetLowerThreshold(0);
    thresholdFilter2->SetUpperThreshold(threshold);
    thresholdFilter2->SetOutsideValue(0);
    thresholdFilter2->SetInsideValue(255);
    thresholdFilter2->Update();

    using MaskFilterType = itk::MaskImageFilter<grayImageT, grayImageT>;
    MaskFilterType::Pointer maskFilter = MaskFilterType::New();
    maskFilter->SetInput(equalizedImage);
    maskFilter->SetMaskImage(thresholdFilter2->GetOutput());
    maskFilter->SetOutsideValue(255);

    maskFilter->Update();
    blurMaskImage = maskFilter->GetOutput();


    VTKViewer::visualize<grayImageT>(blurMaskImage ,"Blur mask");


}


template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::compute()
{


    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter< rgbImageT, grayImageT >;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    grayImage = rgbToGrayFilter->GetOutput();


    gaussianBlur();
    histogramEqualization();
    computeHistogram();
    interpolateZeros();
    findThreshold();
    binaryThreholding();


}

template<typename rgbImageT>
typename CellBinarizationFilter<rgbImageT>::grayImageP
CellBinarizationFilter<rgbImageT>::getBinaryImage()
{

    return binaryImage;
}


template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::findThreshold()
{

    unsigned max = histogram[0];
    threshold = 0;
    for(unsigned i = 0; i <= 170; ++i)
    {
        if(max <= histogram[i])
        {
            max = histogram[i];
            threshold = i;
        }

    }


    std::cout<<threshold<<std::endl;


}





template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::interpolateZeros()
{


    for(auto it = histogram.begin() + 1; it != histogram.end() - 1; ++it)
    {
        if(*it == 0)
        {
            *it = (*(it-1) + *(it+1))/2;
        }

    }
/*
    std::cout<<"his"<<std::endl;
    for(auto it = histogram.begin(); it != histogram.end(); ++it)
    {
        std::cout<< *it <<", ";

    }
    std::cout<<"his"<<std::endl;
*/

}


template<typename rgbImageT>
typename CellBinarizationFilter<rgbImageT>::grayImageP
CellBinarizationFilter<rgbImageT>:: getBlurMaskImage()
{
    return blurMaskImage;
}


template<typename rgbImageT>
typename CellBinarizationFilter<rgbImageT>::grayImageP
CellBinarizationFilter<rgbImageT>:: getBlurImage()
{
    return blurImage;
}

template<typename rgbImageT>
typename CellBinarizationFilter<rgbImageT>::grayImageP
CellBinarizationFilter<rgbImageT>:: getEqualizedImage()
{
    return equalizedImage;
}




