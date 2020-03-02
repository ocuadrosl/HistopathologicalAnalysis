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
#include <itkPointSet.h>
#include <itkVector.h>
#include <itkBSplineScatteredDataPointSetToImageFilter.h>
#include <itkVectorIndexSelectionCastImageFilter.h>


#include "../util/vtkviewer.h"
#include "../util/regionofinterestfilter.h"


template<typename InputImageT, typename OutputImageT>
class AdaptiveOtsuFilter
{


    using InputImageP = typename InputImageT::Pointer;

    using OutputImageP = typename OutputImageT::Pointer;


    //amigo
    using InputPointType = typename InputImageT::PointType;
    using InputCoordType = typename InputPointType::CoordRepType ;

    using  VectorType =  itk::Vector< InputCoordType, 1 >;

    using VectorImageType = itk::Image< VectorType, 2 >;
    using VectorPixelType =  typename VectorImageType::PixelType;

    using PointSetType = itk::PointSet<VectorPixelType, 2 >;
    typedef typename PointSetType::Pointer                   PointSetPointer;
    typedef typename PointSetType::PointType                 PointSetPointType;

    //using PointSetPointer = typename PointSetType::Pointer;


    typedef typename PointSetType::PointsContainerPointer    PointsContainerPointer;
    typedef typename PointSetType::PointDataContainerPointer PointDataContainerPointer;
    typedef typename PointSetType::PointDataContainer        PointDataContainer;


    typedef itk::BSplineScatteredDataPointSetToImageFilter< PointSetType, VectorImageType > SDAFilterType;
    typedef typename SDAFilterType::Pointer SDAFilterPointer;

    typedef itk::VectorIndexSelectionCastImageFilter< VectorImageType,  InputImageT > IndexFilterType;
    typedef typename IndexFilterType::Pointer IndexFilterPointer;





public:
    AdaptiveOtsuFilter();

    void SetInputImage(InputImageP inputImage);
    void Compute();
    OutputImageP GetOutput();

public:

    unsigned regionSize=20;

    InputImageP inputImage;
    OutputImageP outputImage;
    InputImageP thresholdImage;

    PointSetPointer  pointSet;


    void ComputeLocalThresholds();

};


template class AdaptiveOtsuFilter<itk::Image<unsigned, 2>, itk::Image<unsigned, 2>>;
template class AdaptiveOtsuFilter<itk::Image<float   , 2>, itk::Image<unsigned, 2>>;
#endif // ADAPTIVEOTSUFILTER_H
