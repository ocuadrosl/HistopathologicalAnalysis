#ifndef LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H
#define LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H


#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkLabelMap.h>
#include <itkLabelObject.h>
#include <vector>

template<typename pixelComponentT>
class LabelMapToMultipleGrayImagesFilter
{
public:

    //gray scale type alias
    using grayImageType    = itk::Image< pixelComponentT, 2>;
    using grayImagePointer = typename grayImageType::Pointer;

    using labelObjectT = itk::LabelObject<pixelComponentT, 2>;
    using labelMapT    = itk::LabelMap<labelObjectT>;
    using labelMapP    = typename labelMapT::Pointer;

    using imagesT = std::vector<grayImagePointer> ;

    LabelMapToMultipleGrayImagesFilter();


    //setters
    void setLabelMap(labelMapP labelMap);

    void createImages();

private:

    imagesT   images;
    labelMapP labelMap;

};


template  class LabelMapToMultipleGrayImagesFilter<unsigned int>;
template  class LabelMapToMultipleGrayImagesFilter<float>;
#endif // LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H
