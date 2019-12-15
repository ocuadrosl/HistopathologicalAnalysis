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





//local includes
#include "../util/customprint.h"
#include "../util/superpixels.h"
#include "../util/logfilter.h"
#include "../util/vtkviewer.h"
#include "../util/overlayrgbimagefilter.h"
#include "../util/cellbinarizationfilter.h"
#include "../util/math.h"


template<typename rgbImageT>
class CellSegmentator
{
public:
    CellSegmentator();

    using imageP     = typename rgbImageT::Pointer;
    using pixelCompT = typename rgbImageT::PixelType;

    using grayImageT = itk::Image<unsigned,2>;
    using grayImageP =  grayImageT::Pointer;

    using grayImageDoubleT = itk::Image<double,2>;
    using grayImageDP =  grayImageDoubleT::Pointer;

    //Using float because double type is not allowed...
    using vectorImageT = itk::Image<itk::CovariantVector<float, 2>, 2>;
    using vectorImageP = vectorImageT::Pointer;


    using imageDoubleIt = itk::ImageRegionConstIterator<grayImageDoubleT>;
    using imageDoubleItIndex = itk::ImageRegionConstIteratorWithIndex<grayImageDoubleT>;


    using cellsT = std::vector<std::vector<typename rgbImageT::IndexType>>;


    using superPixelsT =  SuperPixels<rgbImageT>;

    //setters

    void setImage(imageP inputImage);


    //getters
    vectorImageP getGradients() const;


    //methods
    void computeGradients();
    void findCellNuclei();
    void visualize();

    void superPixels();




private:

    vectorImageP outputImage;
    imageP       inputImage;
    grayImageP   grayImage;
    grayImageP   multiplyImage;
    grayImageP   eqImage;
    grayImageP   blurMaskImage;
    grayImageP   cellNuclei;
    grayImageP   blurImage;
    grayImageP   labelImage; //superpixels


    cellsT cells;

    std::vector<grayImageDP> LogNorm;

    double   sigmaMin = 2;
    double   sigmaMax = 8;
    double   stepSize = 1;

    unsigned radius = 5;
    unsigned kernelSize = 5;


    //std::unique_ptr<superPixelsT> superPixelsP;

    void createGrayImage();
    void computeLoGNorm();
    void computeEuclideanMap();
    void computeLocalMinimum();

    void extractCellsFromSuperPixels();




};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellSegmentator<rgbImageU>;


#endif // CELLSEGMENTATOR_H
