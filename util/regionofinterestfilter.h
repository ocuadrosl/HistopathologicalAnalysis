#ifndef REGIONOFINTERESTFILTER_H
#define REGIONOFINTERESTFILTER_H

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkRescaleIntensityImageFilter.h>

#include "../util/vtkviewer.h"
#include "../util/vtkviewer.h"
template<typename InputImageT, typename OutputImageT>
class RegionOfInterestFilter
{


    using InputImageP  = typename InputImageT::Pointer;
    using OutputImageP = typename OutputImageT::Pointer;
    using RegionT      = typename InputImageT::RegionType;

public:

    RegionOfInterestFilter();

    void SetInputImage(InputImageP inputImage);
    void SetRegionOfInterest(RegionT roi);
    OutputImageP GetRegionOfInterest();
    void Compute();


private:


    InputImageP  inputImage;
    OutputImageP outputImage;
    RegionT roi;

};


template class RegionOfInterestFilter<itk::Image<float,2> , itk::Image<float,2> >;
template class RegionOfInterestFilter<itk::Image<unsigned,2> , itk::Image<unsigned,2> >;

#endif // REGIONOFINTERESTFILTER_H

