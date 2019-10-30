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


    using MeasurementType = float;
    using FrequencyContainerType = itk::Statistics::DenseFrequencyContainer2;
    using FrequencyType = FrequencyContainerType::AbsoluteFrequencyType;

    using HistogramType = itk::Statistics::Histogram< MeasurementType, FrequencyContainerType >;

    HistogramType::Pointer histogram = HistogramType::New();
    histogram->SetMeasurementVectorSize(numberOfBins);


}
