#ifndef CELLSEGMENTATOR_H
#define CELLSEGMENTATOR_H

#include <itkImage.h>
#include <itkGradientImageFilter.h>
#include <itkCovariantVector.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkTileImageFilter.h>


//local includes
#include "../util/customprint.h"
#include "../util/superpixels.h"
#include "../util/logfilter.h"
#include "itkViewImage.h"

template<typename imageT>
class CellSegmentator
{
public:
    CellSegmentator();

    using imageP     = typename imageT::Pointer;
    using pixelCompT = typename imageT::PixelType;

    using grayImageT = itk::Image<unsigned,2>;
    using grayImageP =  grayImageT::Pointer;

    using grayImageD = itk::Image<double,2>;
    using grayImageDP =  grayImageD::Pointer;

    //Using float because double type is not allowed...
    using vectorImageT = itk::Image<itk::CovariantVector<float, 2>, 2>;
    using vectorImageP = vectorImageT::Pointer;

    using image3DT =  itk::Image<double, 3>;
    using image3DP = image3DT::Pointer;




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
    image3DP     LoGNorm;

    double sigmaMax = 1;
    double sigmaMin = 0;

    void createGrayImage();
    void computeLoGNorm();




};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellSegmentator<rgbImageU>;


#endif // CELLSEGMENTATOR_H
