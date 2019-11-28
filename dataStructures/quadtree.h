#ifndef QUADTREE_H
#define QUADTREE_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <memory>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include "quadnode.h"

template<typename imageT>
class QuadTree
{

    using imageP      = typename imageT::Pointer;
    using quadNodeT   = QuadNode<imageT>;
    using grayImageT  = itk::Image<unsigned int,2>;
    using grayImageP  = typename grayImageT::Pointer;


public:
    QuadTree();

    void build();
    void setImage(const imageP& inputImage);

    grayImageP getLabelImage();


private:

    std::unique_ptr<quadNodeT> root;

    imageP inputImage;
    grayImageP labelImage;

    unsigned minimumSize=100;

    void recursiveBuild(std::unique_ptr<quadNodeT>& node);
    void createLabelImage(std::unique_ptr<quadNodeT>& node, unsigned label);

    unsigned labelAux=1;


};

template class QuadTree<itk::Image<unsigned, 2>>;
template class QuadTree<itk::Image<unsigned char, 2>>;

using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class QuadTree<imageUnsigned>;

#endif // QUADTREE_H
