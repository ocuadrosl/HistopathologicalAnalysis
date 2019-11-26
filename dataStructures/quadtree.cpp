#include "quadtree.h"

template <typename imageT>
QuadTree<imageT>::QuadTree()
{

    root = nullptr;


}

template <typename imageT>
void QuadTree<imageT>::setImage(const imageP& inputImage)
{
    this->inputImage = inputImage;
}


template <typename imageT>
void QuadTree<imageT>::build()
{

    root = std::make_unique<QuadNode<imageT>>();


}

