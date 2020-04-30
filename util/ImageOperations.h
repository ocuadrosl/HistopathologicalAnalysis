#ifndef IMAGEOPERATIONS_H
#define IMAGEOPERATIONS_H

#include <vector>

//Dlib includes
#include <dlib/array2d.h>
#include <dlib/image_transforms.h>


namespace util
{
/*
ImageT is a Dlib image type
index = col, row
*/
template <typename ImageT, typename IndexT = std::vector<unsigned long>>
inline unsigned ExtractNeighborhood(const ImageT& inputImage, const IndexT& index , const unsigned& neighborhoodSize, ImageT& roi)
{

    if(neighborhoodSize % 2 == 0)
    {
        std::cerr<<"ERROR: neighborhood must be odd"<<std::endl;
        return 0;
    }

    const unsigned& colum = index[0];
    const unsigned& row   = index[1];

    const auto imageRowSize = inputImage.nr();
    const auto imageColSize = inputImage.nc();

    unsigned  neighSizeCenter = neighborhoodSize/2;

    //Define begin index
    unsigned  imageRowBegin = (neighSizeCenter > row  ) ? 0 : row   - neighSizeCenter;
    unsigned  imageColBegin = (neighSizeCenter > colum) ? 0 : colum - neighSizeCenter;


    //Define actual neighbprhood size

    unsigned imageRowEnd = (row   + neighSizeCenter >= imageRowSize) ? imageRowSize :  row   + neighSizeCenter+1;
    unsigned imageColEnd = (colum + neighSizeCenter >= imageColSize) ? imageColSize :  colum + neighSizeCenter+1;


    /* std::cout<<index<<std::endl;
    std::cout<<imageColBegin<<" "<<imageRowBegin<<std::endl;
    std::cout<<imageColEnd - imageColBegin<<" "<<imageRowEnd - imageRowBegin<<std::endl;
    std::cout<<std::endl;
    */

    roi.set_size(imageRowEnd - imageRowBegin, imageColEnd - imageColBegin);
    unsigned roiR=0;
    for(unsigned r = imageRowBegin; r < imageRowEnd ; ++r)
    {
        unsigned roiC=0;
        for(unsigned c=imageColBegin; c < imageColEnd; ++c)
        {

            roi[roiR][roiC++] = inputImage[r][c];
        }
        ++roiR;
    }

    return 1;

}


}



#endif // IMAGEOPERATIONS_H
