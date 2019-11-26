#ifndef QUADTREE_H
#define QUADTREE_H

#include <itkImage.h>
#include <itkRGBPixel.h>

#include "quadnode.h"

template<typename imageT>
class QuadTree
{

    using imageP = typename imageT::Pointer;

public:
    QuadTree();

    void build();
    void setImage(const imageP& inputImage);


private:

    std::unique_ptr<QuadNode<imageT>> root;

    imageP inputImage;


};

template class QuadTree<itk::Image<unsigned, 2>>;
template class QuadTree<itk::Image<unsigned char, 2>>;

using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class QuadTree<imageUnsigned>;

#endif // QUADTREE_H
