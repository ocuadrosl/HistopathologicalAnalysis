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
#include <itkNeighborhoodIterator.h>


//local includes
#include "../util/customprint.h"
#include "../util/vtkviewer.h"


template<typename InputImageT>
class PleuraDetector
{

    using ImageP  =  typename InputImageT::Pointer;

    using GrayImageT = itk::Image<unsigned, 2>;
    using GrayImageP = GrayImageT::Pointer;

    //default pixel values for binary images, i.e., edges, otsu, and etc
    const GrayImageT::PixelType Background = 0;
    const GrayImageT::PixelType Foreground = 255;




public:
    PleuraDetector();
    void SetInputImage(ImageP inputImage);
    void Detect();


private:

    ImageP inputImage;

    //Auxiliary functions
    GrayImageP FillHoles(GrayImageP grayImage, bool show=false); //nope
    void GeodesicActiveCountour(GrayImageP grayImage, bool show=false); //nope


    GrayImageP EdgeDetectionCanny(GrayImageP grayImage, bool show=false);
    GrayImageP ConnectedComponets(GrayImageP grayImage, bool show=false);
    void ComputeLocalFeatures(GrayImageP grayImage, GrayImageP components, unsigned radius=1, bool show=false);


};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class PleuraDetector<rgbImageU>;

using rgbImageUChar = itk::Image<itk::RGBPixel<unsigned char>, 2>;
template class PleuraDetector<rgbImageUChar>;

#endif // PLEURADETECTION_H
