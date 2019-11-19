#include "cellbinarizationfilter.h"

template<typename rgbImageT>
CellBinarizationFilter<rgbImageT>::CellBinarizationFilter()
{

}
template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::setImage(rgbImageP inputImage)
{
    this->inputImage = inputImage;
}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::gaussianBlur()
{

    using FilterType = itk::DiscreteGaussianImageFilter<grayImageT, grayImageT>;
    FilterType::Pointer smoothFilter = FilterType::New();

    smoothFilter->SetVariance(sigma);
    smoothFilter->SetMaximumKernelWidth(17);
    smoothFilter->SetInput(grayImage);

    smoothFilter->Update();
    blurImage = smoothFilter->GetOutput();

    //VTKViewer::visualize<grayImageT>(blurImage ,"blur");




}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::histogramEqualization()
{


    using ImageCalculatorFilterT = itk::MinimumMaximumImageCalculator<grayImageT>;

    ImageCalculatorFilterT::Pointer imageCalculatorF = ImageCalculatorFilterT::New();
    imageCalculatorF->SetImage(blurImage);
    imageCalculatorF->Compute();


    eqImage = grayImageT::New();
    eqImage->SetRegions(blurImage->GetRequestedRegion());
    eqImage->Allocate();

    itk::ImageRegionIterator<grayImageT> itB(blurImage, blurImage->GetRequestedRegion());
    itk::ImageRegionIterator<grayImageT> itE(eqImage, eqImage->GetRequestedRegion());

    std::cout<<imageCalculatorF->GetMinimum()<< ","<<imageCalculatorF->GetMaximum()<<std::endl;

    Math::MinMax<unsigned, unsigned> minMax(imageCalculatorF->GetMinimum(), imageCalculatorF->GetMaximum(), 0, 255);

    while(!itB.IsAtEnd())
    {

        itE.Set( minMax(itB.Get()));

        ++itB;
        ++itE;
    }


    //VTKViewer::visualize<grayImageT>(eqImage ,"equalization");





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
    imageToHistogramFilter->SetInput(eqImage);
    imageToHistogramFilter->SetHistogramBinMinimum(lowerBound);
    imageToHistogramFilter->SetHistogramBinMaximum(upperBound);
    imageToHistogramFilter->SetHistogramSize(size);
    imageToHistogramFilter->Update();

    typename ImageToHistogramFilterType::HistogramType  *frequency = imageToHistogramFilter->GetOutput();


    std::cout << "f = [ ";
      for (unsigned int i = 0; i < frequency->GetSize()[0]; ++i)
      {
        std::cout << frequency->GetFrequency(i);
        histogram.push_back(frequency->GetFrequency(i));

        if (i != frequency->GetSize()[0] - 1)
        {
          std::cout << ", ";
        }
      }

      std::cout << " ]" << std::endl;





}


template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::findLocalMinimum(unsigned number)
{

    derivativeVectorT derTmp = derivatives;

    std::sort(derTmp.begin(), derTmp.end());

    for(unsigned i=0; i< number;++i)
    {
        localMinimum.push_back(derTmp[i].second);
        std::cout<<*localMinimum.rbegin()<<std::endl;
    }



}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::computeDerivatives()
{

    for(unsigned i=1; i < 255; ++i)
    {
        derivatives.push_back(std::make_pair(histogram[i] - histogram[i-1], i));
    }
}


template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::binaryThreholding()
{
    using thresholdFilterType = itk::BinaryThresholdImageFilter<grayImageT, grayImageT>;
    thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
    thresholdFilter->SetInput(eqImage);
    thresholdFilter->SetLowerThreshold(0);
    thresholdFilter->SetUpperThreshold(threshold);
    thresholdFilter->SetOutsideValue(255);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->Update();

    outputImage  = thresholdFilter->GetOutput();



    //just testing
    thresholdFilterType::Pointer thresholdFilter2 = thresholdFilterType::New();
    thresholdFilter2->SetInput(eqImage);
    thresholdFilter2->SetLowerThreshold(0);
    thresholdFilter2->SetUpperThreshold(threshold);
    thresholdFilter2->SetOutsideValue(0);
    thresholdFilter2->SetInsideValue(255);
    thresholdFilter2->Update();

    using MaskFilterType = itk::MaskImageFilter<grayImageT, grayImageT>;
    MaskFilterType::Pointer maskFilter = MaskFilterType::New();
    maskFilter->SetInput(eqImage);
    maskFilter->SetMaskImage(thresholdFilter2->GetOutput());
    maskFilter->SetOutsideValue(255);

    maskFilter->Update();


   VTKViewer::visualize<grayImageT>(maskFilter->GetOutput() ,"testing cells");


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
    findThreshold();
    binaryThreholding();





}

template<typename rgbImageT>
typename CellBinarizationFilter<rgbImageT>::grayImageP
CellBinarizationFilter<rgbImageT>::getOutput()
{

    return outputImage;
}


template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::findThreshold()
{

    FindPeaks<long int> findPeaks;
    findPeaks.setData(histogram);
    findPeaks.find();


}



