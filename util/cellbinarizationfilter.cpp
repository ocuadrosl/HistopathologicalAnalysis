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
/*
    using FilterType = itk::DiscreteGaussianImageFilter<grayImageT, grayImageT>;
    FilterType::Pointer smoothFilter = FilterType::New();

    smoothFilter->SetVariance(sigma);
    smoothFilter->SetMaximumKernelWidth(17);
    smoothFilter->SetInput(grayImage);

    smoothFilter->Update();
    blurImage = smoothFilter->GetOutput();
*/
   // VTKViewer::visualize<grayImageT>(blurImage ,"blur Image");

//TODO HEEEEEERERERERERE

    using imageFloat = itk::Image<float, 2>;


    using FilterType = itk::RescaleIntensityImageFilter<grayImageT, imageFloat>;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(grayImage);
    filter->SetOutputMinimum(0.f);
    filter->SetOutputMaximum(1.f);
    filter->Update();

    std::unique_ptr<LoGFilter<imageFloat,imageFloat>> logFilter(new LoGFilter<imageFloat,imageFloat>());
    logFilter->setImage(filter->GetOutput());
    logFilter->setSigma(0.25);
    logFilter->setKernelSize(17);
    logFilter->compute(1,1);



    using FilterType2 = itk::RescaleIntensityImageFilter<imageFloat, grayImageT>;
    FilterType2::Pointer filter2 = FilterType2::New();
    filter2->SetInput(logFilter->getOutput());
    filter2->SetOutputMinimum(0);
    filter2->SetOutputMaximum(255);
    filter2->Update();

    blurImage = filter2->GetOutput();


    VTKViewer::visualize<grayImageT>(blurImage ,"blur Image");


}

template<typename rgbImageT>
void CellBinarizationFilter<rgbImageT>::histogramEqualization()
{




    using AdaptiveHistogramEqualizationImageFilterType = itk::AdaptiveHistogramEqualizationImageFilter<grayImageT>;
    AdaptiveHistogramEqualizationImageFilterType::Pointer adaptiveHistogramEqualizationImageFilter =
    AdaptiveHistogramEqualizationImageFilterType::New();



    adaptiveHistogramEqualizationImageFilter->SetAlpha(0);

      //float beta = std::stod(argv[4]);
    adaptiveHistogramEqualizationImageFilter->SetBeta(0);

     // int                                                         radiusSize = std::stoi(argv[5]);
    AdaptiveHistogramEqualizationImageFilterType::ImageSizeType radius;
    radius.Fill(50);
    adaptiveHistogramEqualizationImageFilter->SetRadius(radius);
    adaptiveHistogramEqualizationImageFilter->SetInput(blurImage);
    adaptiveHistogramEqualizationImageFilter->Update();

    eqImage = adaptiveHistogramEqualizationImageFilter->GetOutput();


 VTKViewer::visualize<grayImageT>(eqImage ,"local equalization");


/*
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

        itE.Set(minMax(itB.Get()));

        ++itB;
        ++itE;
    }


    VTKViewer::visualize<grayImageT>(eqImage ,"equalization");

*/

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
    thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
    thresholdFilter->SetInput(eqImage);
    thresholdFilter->SetLowerThreshold(0);
    thresholdFilter->SetUpperThreshold(threshold);
    thresholdFilter->SetOutsideValue(255);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->Update();

    binaryImage  = thresholdFilter->GetOutput();



    //blur mask
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
    for(unsigned i = 0; i <= 85; ++i )
    {
        if(max <= histogram[i])
        {
            max = histogram[i];
            threshold = i;
        }

    }


    std::cout<<threshold<<std::endl;
/*
    p1d::Persistence1D p;
    p.RunPersistence(histogram);

    std::vector< p1d::TPairedExtrema > Extrema;
    p.GetPairedExtrema(Extrema, 0);

    std::vector<unsigned> maxPeaks;
    for(auto it = Extrema.begin(); it != Extrema.end(); it++)
    {
        maxPeaks.push_back(static_cast<unsigned>((*it).MaxIndex));

        std::cout<<(*it).MaxIndex<< ", ";

    }

    std::sort(maxPeaks.begin(), maxPeaks.end());

    threshold = maxIndex1;
    for(auto it = maxPeaks.begin(); it != maxPeaks.end(); ++it)
    {
        if(maxIndex1 < *it &&  *it < maxIndex2)
        {
            threshold = *it;

        }

    }

*/

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
    return eqImage;
}




