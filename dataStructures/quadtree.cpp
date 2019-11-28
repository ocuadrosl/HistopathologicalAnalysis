#include "quadtree.h"

template <typename imageT>
QuadTree<imageT>::QuadTree()
{


}

template <typename imageT>
void QuadTree<imageT>::setImage(const imageP& inputImage)
{
    this->inputImage = inputImage;
}


template <typename imageT>
void QuadTree<imageT>::build()
{


    const auto& begin = inputImage->GetRequestedRegion().GetIndex();
    const auto& end   = inputImage->GetRequestedRegion().GetUpperIndex();
    root = std::make_unique<quadNodeT>(begin, end);


    recursiveBuild(root);

    std::cout<<"Quadtree [OK]"<<std::endl;

}


template <typename imageT>
void QuadTree<imageT>::recursiveBuild(std::unique_ptr<quadNodeT>& node)
{

    if(node == nullptr)
    {
        return;
    }




    const auto& begin = node->getBegin();
    const auto& end = node->getEnd();

    typename imageT::RegionType region;
    region.SetIndex(begin);
    region.SetUpperIndex(end);


    if(region.GetSize()[0] < minimumSize || region.GetSize()[1] < minimumSize)
    {
        return;
    }


    itk::ImageRegionConstIterator<imageT> it(inputImage, region);


    //typedefs
    using indexT = typename imageT::IndexType;
    //aux variables
    indexT  beginAux;
    indexT  endAux;

    auto flagPixel = it.Get();
    ++it;
    for( ;!it.IsAtEnd(); ++it )
    {

        if(!(flagPixel == it.Get()))
        {

            //child 0 top-left
            endAux[0] = (begin[0] + end[0])/2 - 1;
            endAux[1] = (begin[1] + end[1])/2 - 1;

            auto topLeft = std::make_unique<quadNodeT>(begin, endAux);
            node->setChild(0, std::move(topLeft));

            //child 1 top-right
            beginAux[0] = (begin[0] + end[0])/2;
            beginAux[1] = begin[1];

            endAux[0] = end[0];
            endAux[1] = (begin[1] + end[1]) / 2;

            auto topRight = std::make_unique<quadNodeT>(beginAux, endAux);
            node->setChild(1, std::move(topRight));

            //child 2 bottom-left
            beginAux[0] = begin[0];
            beginAux[1] = (begin[1] + end[1])/2;

            endAux[0] = (begin[1] + end[1])/2;
            endAux[1] = end[1];

            auto bottomLeft = std::make_unique<quadNodeT>(beginAux, endAux);
            node->setChild(2, std::move(bottomLeft));

            //child 3 bottom-right
            beginAux[0] = (begin[1] + end[1])/2;
            beginAux[1] = (begin[1] + end[1])/2;

            endAux[0] = end[0];
            endAux[1] = end[1];

            auto bottomRight = std::make_unique<quadNodeT>(beginAux, endAux);
            node->setChild(3, std::move(bottomRight));

            break;
        }

    }

    recursiveBuild(node->getChild(0));
    recursiveBuild(node->getChild(1));
    recursiveBuild(node->getChild(2));
    recursiveBuild(node->getChild(3));

    return;

}


template<typename imageT>
typename QuadTree<imageT>::grayImageP QuadTree<imageT>::getLabelImage()
{

    labelImage = grayImageT::New();
    labelImage->SetRegions(inputImage->GetRequestedRegion());
    labelImage->Allocate();

     createLabelImage(root, 100);

    return labelImage;


}



template<typename imageT>
void  QuadTree<imageT>::createLabelImage(std::unique_ptr<quadNodeT>& node, unsigned label)
{

    if(node == nullptr) { return; }

    if(node->isLeaf())
    {

        typename imageT::RegionType region;
        region.SetIndex(node->getBegin());
        region.SetUpperIndex(node->getEnd());


        itk::ImageRegionIterator<grayImageT> it(labelImage, region);

        for (; !it.IsAtEnd(); ++it)
        {
            it.Set(label);
        }

        return;
    }

    createLabelImage(node->getChild(0), label);
    //createLabelImage(node->getChild(1), label);
    //createLabelImage(node->getChild(2), label);
    //createLabelImage(node->getChild(3), label);



}


















