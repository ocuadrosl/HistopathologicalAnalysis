#include "quadnode.h"

template <typename imageT>
QuadNode<imageT>::QuadNode()
{
    //4 size vector of nullptr pointers
    children = childrenT(4);

    this->begin = {{0,0}};
    this->end   = {{0,0}};


}


template <typename imageT>
QuadNode<imageT>::QuadNode(const indexT& begin, const indexT& end)
{
    //4 size vector of nullptr pointers
    children    = childrenT(4);

    this->begin = begin;
    this->end   = end;

    //std::cout<<children.size()<<std::endl;
   //std::cout<<begin[0]<<", "<<begin[1]<<std::endl;
    //std::cout<<end[0]<<", "<<end[1]<<std::endl;
}


template <typename imageT>
const typename QuadNode<imageT>::indexT&
QuadNode<imageT>::getBegin() const
{
    return begin;
}

template <typename imageT>
const typename QuadNode<imageT>::indexT&
QuadNode<imageT>::getEnd() const
{
    return end;
}

template <typename imageT>
void QuadNode<imageT>::setChild(unsigned index, quadNodeP&& node)
{
    children[index] = std::move(node);

    /*std::cout<<index<<std::endl;
    std::cout<<children[index]->begin[0]<<", "<<children[index]->begin[1]<<std::endl;
    std::cout<<children[index]->end[0]<<", "<<children[index]->end[1]<<std::endl;
    std::cout<<std::endl;
*/


}



template <typename imageT>
typename QuadNode<imageT>::quadNodeP& QuadNode<imageT>::getChild(unsigned index)
{
    return children[index];
}



