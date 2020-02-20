#ifndef CELLSEGMENTATOR_H
#define CELLSEGMENTATOR_H

#include <itkImage.h>
#include <itkGradientImageFilter.h>
#include <itkCovariantVector.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkTileImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkViewImage.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>
#include <itkImageRegionConstIterator.h>
#include <itkVectorContainer.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkMultiplyImageFilter.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkRGBPixel.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRegionalMinimaImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkLabelMapToRGBImageFilter.h>
#include <itkImageDuplicator.h>

#include <itkOtsuMultipleThresholdsImageFilter.h>
#include <itkImageLinearConstIteratorWithIndex.h>
#include <itkImageFileWriter.h>


#include "itkLiThresholdImageFilter.h"
#include "itkHuangThresholdImageFilter.h"
#include "itkIntermodesThresholdImageFilter.h"
#include "itkIsoDataThresholdImageFilter.h"
#include "itkKittlerIllingworthThresholdImageFilter.h"
#include "itkMaximumEntropyThresholdImageFilter.h"
#include "itkMomentsThresholdImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkRenyiEntropyThresholdImageFilter.h"
#include "itkShanbhagThresholdImageFilter.h"
#include "itkTriangleThresholdImageFilter.h"
#include "itkYenThresholdImageFilter.h"

#include <itkCannyEdgeDetectionImageFilter.h>

#include <itkLabelMapOverlayImageFilter.h>
#include <itkGradientImageFilter.h>
#include <itkVectorImageToImageAdaptor.h>
#include <itkCovariantVector.h>

#include <fstream>
#include <string>
//local includes
#include "../util/customprint.h"
#include "../util/superpixels.h"
#include "../util/logfilter.h"
#include "../util/vtkviewer.h"
#include "../util/overlayrgbimagefilter.h"
#include "../util/cellbinarizationfilter.h"
#include "../util/math.h"
#include "../util/overlayrgbimagefilter.h"
#include "../util/extractchannelfilter.h"
#include "../util/replaceimagechannelfilter.h"
#include "../util/text.h"


#include "graph.h"


template<typename rgbImageT>
class CellSegmentator
{
public:
    CellSegmentator();

    using imageP     = typename rgbImageT::Pointer;
    using pixelCompT = typename rgbImageT::PixelType;

    using grayImageT = itk::Image<unsigned, 2>;
    using grayImageP =  grayImageT::Pointer;

    using grayImageDoubleT = itk::Image<double,2>;
    using grayImageDP =  grayImageDoubleT::Pointer;



    //Using float because double type is not allowed...
    using vectorImageT = itk::Image<itk::CovariantVector<float, 2>, 2>;
    using vectorImageP = vectorImageT::Pointer;


    using imageDoubleIt = itk::ImageRegionConstIterator<grayImageDoubleT>;
    using imageDoubleItIndex = itk::ImageRegionConstIteratorWithIndex<grayImageDoubleT>;


    using floatImageT = itk::Image<float,2>;
    using floatImageP = floatImageT::Pointer;

    using labPixelT = itk::RGBPixel<float>;
    using labImageT = itk::Image<labPixelT, 2>;


    using CovariantImage = itk::Image<itk::CovariantVector<float, 2>, 2>;
    using CovarianteImageP = CovariantImage::Pointer;


    using  featuresVectorT = std::vector<std::vector<float>>;




    //setters

    void setImage(imageP inputImage);


    //getters
    vectorImageP getGradients() const;


    //methods

    void findCells();
    void visualize();
    void WriteFeaturesVector(const std::string& fileName);
    void ReadWekaFile(const std::string& fileName, const std::string& imageName);




private:

    vectorImageP outputImage;
    imageP       inputImage;
    grayImageP   grayImage;

    floatImageP  blurImage;
    grayImageP   edges;
    floatImageP  orientationFeatures;
    grayImageP   superPixelsLabels; //superpixels
    floatImageP  bChannel;
    floatImageP  differenceFeatures;
    CovarianteImageP gradient;

    grayImageP binaryImage;


    featuresVectorT featuresVector;

    unsigned superPixelsNumber=0;



    //std::unique_ptr<superPixelsT> superPixelsP;


    void LabelRoughly();
    void CreateImageB(bool show=false);
    floatImageP computeDistances(floatImageP inputImage , bool show=false);
    void findEdges(bool show=false);

    void ComputeGradients();
    void ComputeRayFetures(bool show = false);
    void ComputeSuperPixels(bool show = false);

    void ComputeFeaturesVector(bool show=false);// from super pixels


    void GaussianBlur(bool show=false);
    void DetectEdges(bool show=false);

    void extractCellsFromSuperPixels();



    void overlay(grayImageP image);

    void threshold(bool show=false);


};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellSegmentator<rgbImageU>;


#endif // CELLSEGMENTATOR_H
