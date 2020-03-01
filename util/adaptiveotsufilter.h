#ifndef ADAPTIVEOTSUFILTER_H
#define ADAPTIVEOTSUFILTER_H

#include <itkImage.h>
#include <itkBSplineDecompositionImageFilter.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

#include "../util/vtkviewer.h"


template<typename InputImageT, typename OutputImageT>
class AdaptiveOtsuFilter
{


     using InputImageP = typename InputImageT::Pointer;

public:
    AdaptiveOtsuFilter();

    void SetInputImage(InputImageP inputImage);

public:


    unsigned numberOfControlPoints=200;
    unsigned numberOfSamples=200;
    unsigned regionSize=5;

    InputImageP inputImage;
    InputImageP thresholdImage;

    void ComputeLocalThresholds();

};


template class AdaptiveOtsuFilter<itk::Image<unsigned, 2>, itk::Image<unsigned, 2>>;
template class AdaptiveOtsuFilter<itk::Image<float   , 2>, itk::Image<unsigned, 2>>;
#endif // ADAPTIVEOTSUFILTER_H
