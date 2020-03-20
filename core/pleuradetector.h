#ifndef PLEURADETECTOR_H
#define PLEURADETECTOR_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkVotingBinaryIterativeHoleFillingImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkGeodesicActiveContourLevelSetImageFilter.h>
#include <itkFastMarchingImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelToRGBImageFilter.h>


//local includes
#include "../util/customprint.h"
#include "../util/vtkviewer.h"


template<typename InputImageT>
class PleuraDetector
{

    using ImageP  =  typename InputImageT::Pointer;

    using GrayImageT = itk::Image<unsigned, 2>;
    using GrayImageP = GrayImageT::Pointer;



public:
    PleuraDetector();
    void SetInputImage(ImageP inputImage);
    void Detect();


private:

    ImageP inputImage;

    //Auxiliary functions
    GrayImageP FillHoles(GrayImageP grayImage, bool show=false);
    void GeodesicActiveCountour(GrayImageP grayImage, bool show=false);
    GrayImageP EdgeDetectionCanny(GrayImageP grayImage, bool show=false);
    void ConnectedComponets(GrayImageP grayImage, bool show=false);


};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class PleuraDetector<rgbImageU>;

using rgbImageUChar = itk::Image<itk::RGBPixel<unsigned char>, 2>;
template class PleuraDetector<rgbImageUChar>;

#endif // PLEURADETECTION_H
