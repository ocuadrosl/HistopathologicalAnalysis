#ifndef CELLSEGMENTATOR_H
#define CELLSEGMENTATOR_H

#include <itkImage.h>
#include <itkGradientImageFilter.h>
#include <itkCovariantVector.h>

//local includes
#include "../util/customprint.h"

template<typename imageT>
class CellSegmentator
{
public:
    CellSegmentator();

    using imageP     = typename imageT::Pointer;
    using pixelCompT = typename imageT::PixelType;


    using gradientFilterT = itk::GradientImageFilter<imageT, float>;

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



private:

    vectorImageP outputImage;
    imageP       inputImage;

};


template class CellSegmentator<itk::Image<unsigned, 2>>;
template class CellSegmentator<itk::Image<double  , 2>>;


#endif // CELLSEGMENTATOR_H
