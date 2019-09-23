#ifndef CELLSEGMENTATOR_H
#define CELLSEGMENTATOR_H

#include <itkImage.h>
#include <itkGradientImageFilter.h>
#include <itkCovariantVector.h>
#include <itkRGBToLuminanceImageFilter.h>

//local includes
#include "../util/customprint.h"
#include "../util/superpixels.h"

template<typename imageT>
class CellSegmentator
{
public:
    CellSegmentator();

    using imageP     = typename imageT::Pointer;
    using pixelCompT = typename imageT::PixelType;




    //Using float because double type is not allowed...
    using vectorImageT = itk::Image<itk::CovariantVector<float, 2>, 2>;
    using vectorImageP = vectorImageT::Pointer;


    //setters

    void setImage(imageP inputImage);


    //getters
    vectorImageP getGradients() const;


    //methods
    void computeGradients();
    void visualize();

    void superPixels();




private:

    vectorImageP outputImage;
    imageP       inputImage;




};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class CellSegmentator<rgbImageU>;


#endif // CELLSEGMENTATOR_H
