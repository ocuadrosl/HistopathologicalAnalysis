#include "cellsegmentator.h"


template<typename imageT>
CellSegmentator<imageT>::CellSegmentator()
{

}

template<typename imageT>
void CellSegmentator<imageT>::setImage(imageP inputImage)
{

    this->inputImage = inputImage;


}


template<typename imageT>
void CellSegmentator<imageT>::computeGradients()
{

    // Create and setup a gradient filter

    typename gradientFilterT::Pointer gradientFilter = gradientFilterT::New();
    gradientFilter->SetInput( inputImage );
    gradientFilter->Update();
    outputImage = gradientFilter->GetOutput();

    IO::printOK("Computing Gradients");

}

template<typename imageT>
typename CellSegmentator<imageT>::vectorImageP
CellSegmentator<imageT>::getGradients() const
{
    return outputImage;
}
