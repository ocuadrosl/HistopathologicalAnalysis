#ifndef LOGFILTER_H
#define LOGFILTER_H

#include <itkImage.h>
#include <itkNeighborhoodIterator.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkNeighborhoodInnerProduct.h>
#include <itkNeighborhoodOperator.h>
#include <itkImageRegion.h>

#include "../util/customprint.h"
#include "../util/vtkviewer.h"
#include "../util/math.h"

template<typename imageT>
class LoGFilter
{


    using imageP = typename imageT::Pointer;

    using doubleImageT = itk::Image<double, 2>;
    using doubleImageP = doubleImageT::Pointer;



public:
    LoGFilter();

    void setImage(imageP inputImage);
    void compute();


private:
    imageP       inputImage;
    doubleImageP       outputImage;
    doubleImageP kernel;

    unsigned kernelSize = 5;
    double   sigma      = 0.5;

    void createKernel();


};


template class LoGFilter< itk::Image<unsigned,2>>;


#endif // LOGFILTER_H
