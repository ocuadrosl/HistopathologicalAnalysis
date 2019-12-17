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


    using imageFloatT = itk::Image<float, 2>;
    using imageFloatP = imageFloatT::Pointer;


public:
    LoGFilter();

    void setImage(inputImageP inputImage);
    void compute(bool show=false, bool echo=false);
    void setSigma(float sigma);
    void setKernelSize(unsigned kernelSize);

    outputImageP getOutput();



private:
    inputImageP  inputImage;
    outputImageP outputImage;
    imageFloatP kernel;

    unsigned kernelSize = 5;
    float   sigma      = 1.5;

    void createKernel(bool show=false);


};


template class LoGFilter< itk::Image<unsigned,2>,  itk::Image<unsigned,2>>;
template class LoGFilter< itk::Image<unsigned,2>,  itk::Image<double  ,2>>;
template class LoGFilter< itk::Image<unsigned,2>,  itk::Image<float  ,2>>;
template class LoGFilter< itk::Image<float,2>,  itk::Image<float  ,2>>;



#endif // LOGFILTER_H
