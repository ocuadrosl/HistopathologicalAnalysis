#ifndef QUADTREE_H
#define QUADTREE_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <memory>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkLabelOverlayImageFilter.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

#include "../util/vtkviewer.h"

#include "quadnode.h"

template<typename imageT>
class QuadTree
{

    using imageP      = typename imageT::Pointer;
    using quadNodeT   = QuadNode<imageT>;
    using grayImageT  = itk::Image<unsigned,2>;
    using grayImageP  = typename grayImageT::Pointer;
    using pixelT      = typename imageT::PixelType;




public:
    QuadTree();

    void build();
    void setImage(const imageP& inputImage);
    unsigned getLeavesNumber() const;

    grayImageP getLabelImage();


private:

    std::unique_ptr<quadNodeT> root;

    imageP inputImage;
    grayImageP labelImage;

    unsigned minimumSize=50;

    void recursiveBuild(std::unique_ptr<quadNodeT>& node);
    void createLabelImage(std::unique_ptr<quadNodeT>& node, unsigned label);

    unsigned labelAux=0;

    unsigned getNumberOfPoints(std::unique_ptr<quadNodeT>& node, const pixelT& refPixel);


};

template class QuadTree<itk::Image<unsigned, 2>>;
//template class QuadTree<itk::Image<unsigned char, 2>>;
template class QuadTree<itk::Image<float, 2>>;
using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class QuadTree<imageUnsigned>;

#endif // QUADTREE_H
