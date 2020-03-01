#include "adaptiveotsufilter.h"


template<typename InputImageT, typename OutputImageT>
AdaptiveOtsuFilter<InputImageT, OutputImageT>::AdaptiveOtsuFilter()
{

}


template<typename InputImageT, typename OutputImageT>
void AdaptiveOtsuFilter<InputImageT, OutputImageT>::ComputeLocalThresholds()
{


    thresholdImage = InputImageT::New();
    thresholdImage->SetRegions(inputImage->GetRequestedRegion());
    thresholdImage->Allocate();
    thresholdImage->FillBuffer(0);

    using regionT = typename InputImageT::RegionType;
    using indexT  = typename InputImageT::IndexType;

    regionT region;

    auto size = inputImage->GetRequestedRegion().GetSize();
    indexT index, upperIndex;
    index.Fill(0);

    using otsuFilterType = itk::OtsuThresholdImageFilter<InputImageT, OutputImageT>;

    using roiFilterType = itk::RegionOfInterestImageFilter<InputImageT, InputImageT>;

    for(unsigned r=regionSize; r < size[1] ; r += regionSize )
    {

        upperIndex[1] = ( r < size[1]) ? r: size[1] - 1;

        for(unsigned c = regionSize; c < size[0] ; c += regionSize )
        {
            upperIndex[0] = (c < size[0]) ? c : size[0] - 1;


            //std::cout<<index<<" - "<<upperIndex<<std::endl;

            region.SetIndex(index);
            region.SetUpperIndex(upperIndex);

            auto otsuFilter = otsuFilterType::New();
            auto roiFilter = roiFilterType::New();
            roiFilter->SetInput(inputImage);
            roiFilter->SetRegionOfInterest(region);

            otsuFilter->SetInput(roiFilter->GetOutput());
            otsuFilter->SetNumberOfHistogramBins(100);
            otsuFilter->Update();


            //std::cout<<otsuFilter->GetThreshold()<<std::endl;

            thresholdImage->SetPixel(index,otsuFilter->GetThreshold() );

            index[0] = upperIndex[0];


        }

        index[1] = upperIndex[1];
        index[0] = 0;

    }




    using DecompositionType =  itk::BSplineDecompositionImageFilter<InputImageT, InputImageT>;
    typename DecompositionType::Pointer decomposition = DecompositionType::New();
    decomposition->SetSplineOrder(5);
    decomposition->SetInput(thresholdImage);
    decomposition->Update();


    //writing results
    using rescaleFilterType2= itk::RescaleIntensityImageFilter<InputImageT, itk::Image<unsigned char,2>>;
    typename rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(decomposition->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();



    VTKViewer::visualize<itk::Image<unsigned char,2>>(rescaleFilter->GetOutput() ,"B-Spline");




}


template<typename InputImageT, typename OutputImageT>
void AdaptiveOtsuFilter<InputImageT, OutputImageT>::SetInputImage(InputImageP inputImage)
{

    this->inputImage = inputImage;
}
