#ifndef PLEURADETECTOR_H
#define PLEURADETECTOR_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapToRGBImageFilter.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>



#include <itkAttributeOpeningLabelMapFilter.h>


#include <itkLabelImageToShapeLabelMapFilter.h>
#include <itkShapeLabelObject.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>


//local includes
#include "../util/customprint.h"
#include "../util/vtkviewer.h"
#include "../util/inputOutput.h"
#include "../util/fractaldimensionfilter.h"
#include "../util/extractchannelfilter.h"

#include "../util/colorconverterfilter.h"


template<typename RGBImageT>
class PleuraDetector
{

    using RGBImageP  =  typename RGBImageT::Pointer;

    using GrayImageT = itk::Image<unsigned, 2>;
    using GrayImageP = GrayImageT::Pointer;

    //default pixel values for binary images, i.e., edges, otsu, and etc
    const GrayImageT::PixelType Background = 0;
    const GrayImageT::PixelType Foreground = 255;

    using LabelType = unsigned;
    using ShapeLabelObjectType = itk::ShapeLabelObject<LabelType, 2>;
    using LabelMapType = itk::LabelMap<ShapeLabelObjectType>;
    using LabelMapP = LabelMapType::Pointer;



    using FloatImageT = itk::Image<float,2>;
    using FloatImageP = FloatImageT::Pointer;



public:

    PleuraDetector();
    void SetInputImage(RGBImageP InputImage);
    void Detect();




private:

    RGBImageP InputImage;


    //Auxiliary functions
    GrayImageP  HistogramEqualization(GrayImageP grayImage, float alpha=1, float beta=0, unsigned radiusSize=5, bool show=false);
    GrayImageP  EdgeDetectionCanny(GrayImageP grayImage, bool show=false);
    LabelMapP   ConnectedComponets(GrayImageP grayImage, unsigned threhold = 0,  bool show=false);
    FloatImageP ComputeFractalDimension(LabelMapP components,  float threshold, bool show=false);
    FloatImageP ComputeRoundness(LabelMapP components, float threshold, bool show=false);
    RGBImageP   RemoveBackground(float lThreshold=90.f, float aThreshold = 5.f, float bThresold = 5.f, bool show=false);


};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class PleuraDetector<rgbImageU>;

//using rgbImageUChar = itk::Image<itk::RGBPixel<unsigned char>, 2>;
//template class PleuraDetector<rgbImageUChar>;

#endif // PLEURADETECTION_H
