#ifndef QUADNODE_H
#define QUADNODE_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <vector>
#include <memory>

template <typename imageT>
class QuadNode
{

    using indexT = typename imageT::IndexType;
    using quadNodeT = QuadNode<imageT>;
    using quadNodeP = std::unique_ptr<quadNodeT>;
    using childrenT = std::vector<std::unique_ptr<quadNodeT>>;



public:
    QuadNode();
    QuadNode(const indexT& begin, const indexT& end);

    const indexT& getBegin() const;
    const indexT& getEnd()   const;

    void setChild(unsigned index, quadNodeP&& node);
    quadNodeP& getChild(unsigned index);


    bool isLeaf(){return children[0] == nullptr;}


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
