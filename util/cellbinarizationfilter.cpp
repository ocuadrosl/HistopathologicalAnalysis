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
void CellBinarizationFilter<rgbImageT>:: computeHistogram()
{

    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter< rgbImageT, grayImageT >;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    grayImage = rgbToGrayFilter->GetOutput();

    using ImageToHistogramFilterType = itk::Statistics::ImageToHistogramFilter<grayImageT>;

    typename ImageToHistogramFilterType::HistogramType::MeasurementVectorType lowerBound(numberOfBins);
    lowerBound.Fill(0);

    typename ImageToHistogramFilterType::HistogramType::MeasurementVectorType upperBound(numberOfBins);
    upperBound.Fill(255);


    typename ImageToHistogramFilterType::HistogramType::SizeType size(1); //gray-scale
    size.Fill(numberOfBins);



    typename ImageToHistogramFilterType::Pointer imageToHistogramFilter = ImageToHistogramFilterType::New();
    imageToHistogramFilter->SetInput(grayImage);
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
         // std::cout << ", ";
        }
      }

      //std::cout << " ]" << std::endl;





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
void CellBinarizationFilter<rgbImageT>::compute()
{

    computeHistogram();
    computeDerivatives();
    findLocalMinimum();

}






