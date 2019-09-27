#ifndef LOGFILTER_H
#define LOGFILTER_H

#include <itkImage.h>

#include <itkImageRegion.h>
#include <itkCastImageFilter.h>

#include "itkConvolutionImageFilter.h"


#include "../util/customprint.h"
#include "../util/vtkviewer.h"
#include "../util/math.h"

template<typename inputImageT, typename outputImageT>
class LoGFilter
{


    using inputImageP  = typename inputImageT ::Pointer;
    using outputImageP = typename outputImageT::Pointer;


    using doubleImageT = itk::Image<double, 2>;
    using doubleImageP = doubleImageT::Pointer;



public:
    LoGFilter();

    void setImage(inputImageP inputImage);
    void compute(bool show=false);


private:
    inputImageP  inputImage;
    outputImageP outputImage;
    doubleImageP kernel;

    unsigned kernelSize = 5;
    double   sigma      = 0.5;

    void createKernel(bool show=false);


};


template class LoGFilter< itk::Image<unsigned,2>,  itk::Image<unsigned,2>>;
template class LoGFilter< itk::Image<unsigned,2>,  itk::Image<double  ,2>>;



#endif // LOGFILTER_H
