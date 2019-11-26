#ifndef QUADNODE_H
#define QUADNODE_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <vector>

template <typename imageT>
class QuadNode
{

    using indexT = typename imageT::IndexType;
    using childrenT = std::vector< std::unique_ptr<QuadNode<imageT>> >;


public:
    QuadNode();

private:

    indexT begin;
    indexT end;

    childrenT children;



};

template class QuadNode<itk::Image<unsigned, 2>>;
template class QuadNode<itk::Image<unsigned char, 2>>;

using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class QuadNode<imageUnsigned>;



#endif // QUADNODE_H
