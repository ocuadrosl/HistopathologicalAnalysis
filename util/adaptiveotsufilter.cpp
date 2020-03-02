#include "adaptiveotsufilter.h"


template<typename InputImageT, typename OutputImageT>
AdaptiveOtsuFilter<InputImageT, OutputImageT>::AdaptiveOtsuFilter()
{

}


template<typename InputImageT, typename OutputImageT>
void AdaptiveOtsuFilter<InputImageT, OutputImageT>::ComputeLocalThresholds()
{

    pointSet = PointSetType::New();

    PointsContainerPointer  pointsContainer = pointSet->GetPoints();

    PointDataContainerPointer  pointDataContainer = PointDataContainer::New();

    //pointSet->SetPointData( pointDataContainer );

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

    using otsuFilterType = itk::OtsuThresholdImageFilter<InputImageT, InputImageT>;


    VectorPixelType V;


    unsigned i=0;
    PointSetPointType point;



    using roiFilterT = RegionOfInterestFilter<InputImageT, InputImageT>;





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

            auto roiFilter = std::make_unique<roiFilterT>();
            roiFilter->SetInputImage(inputImage);
            roiFilter->SetRegionOfInterest(region);
            roiFilter->Compute();




            otsuFilter->SetInput(roiFilter->GetRegionOfInterest());
            otsuFilter->SetNumberOfHistogramBins(50);
            otsuFilter->Update();


            //std::cout<<otsuFilter->GetThreshold()<<std::endl;


            V[0] = static_cast<InputCoordType>(otsuFilter->GetThreshold());

            pointDataContainer->InsertElement(i,V);
            inputImage->TransformIndexToPhysicalPoint(index, point);
            pointsContainer->InsertElement(i,point);


            //thresholdImage->SetPixel(index,otsuFilter->GetThreshold() );

            index[0] = upperIndex[0];

            ++i;

        }

        index[1] = upperIndex[1];
        index[0] = 0;

    }




    pointSet->SetPoints(pointsContainer);
    pointSet->SetPointData(pointDataContainer);

    typename SDAFilterType::ArrayType ncps;
    ncps.Fill(50);

    SDAFilterPointer filter = SDAFilterType::New();
    filter->SetSplineOrder( 3 );
    filter->SetNumberOfControlPoints( ncps );
    filter->SetNumberOfLevels( 5 );


    filter->SetOrigin( inputImage->GetOrigin() );
    filter->SetSpacing( inputImage->GetSpacing() );

    filter->SetSize( size);
    filter->SetInput( pointSet );
    filter->Update();


    IndexFilterPointer componentExtractor = IndexFilterType::New();
    componentExtractor->SetInput( filter->GetOutput() );
    componentExtractor->SetIndex( 0 );
    componentExtractor->Update();
    thresholdImage = componentExtractor->GetOutput();




    using rescaleFilterType2= itk::RescaleIntensityImageFilter<InputImageT, itk::Image<unsigned char,2>>;
    typename rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(thresholdImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();



   // VTKViewer::visualize<itk::Image<unsigned char,2>>(rescaleFilter->GetOutput() ,"B-Spline");

    //VTKViewer::visualize<InputImageT>(thresholdImage ,"B-Spline");


}


template<typename InputImageT, typename OutputImageT>
void AdaptiveOtsuFilter<InputImageT, OutputImageT>::SetInputImage(InputImageP image)
{


    this->inputImage = image;

    //VTKViewer::visualize<itk::Image<unsigned char,2>>(rescaleFilter->GetOutput() ,"input");
}


template<typename InputImageT, typename OutputImageT>
typename AdaptiveOtsuFilter<InputImageT, OutputImageT>::OutputImageP AdaptiveOtsuFilter<InputImageT, OutputImageT>::GetOutput()
{

    return outputImage;
}

template<typename InputImageT, typename OutputImageT>
void AdaptiveOtsuFilter<InputImageT, OutputImageT>::Compute()
{
    outputImage = OutputImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->FillBuffer(255);

    ComputeLocalThresholds();



 /*  using rescaleFilterType= itk::RescaleIntensityImageFilter<InputImageT, itk::Image<unsigned ,2>>;
    typename rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(inputImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();



    typename rescaleFilterType::Pointer rescaleFilter2 = rescaleFilterType::New();
    rescaleFilter2->SetInput(thresholdImage);
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);
    rescaleFilter2->Update();

*/

    itk::ImageRegionConstIterator<InputImageT> iIt(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionConstIterator<InputImageT> tIt(thresholdImage, thresholdImage->GetRequestedRegion());
    itk::ImageRegionIterator<OutputImageT> oIt(outputImage, outputImage->GetRequestedRegion());

   //std::cout<<inputImage->GetRequestedRegion()<<std::endl;


    for(; !iIt.IsAtEnd(); ++iIt, ++tIt, ++oIt)
    {
        //std::cout<<iIt.Get()<<" - "<<tIt.Get()<<std::endl;

        if(iIt.Get() >= tIt.Get())
        {
            //std::cout<<iIt.Get()<<" - "<<tIt.Get()<<std::endl;
            oIt.Set(0);
        }

    }

    //VTKViewer::visualize<OutputImageT>(outputImage ,"Adaptive Otsu");


}














