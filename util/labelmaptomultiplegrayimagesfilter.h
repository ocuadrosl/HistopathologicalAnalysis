#ifndef LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H
#define LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H


#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkLabelMap.h>
#include <itkLabelObject.h>
#include <itkShrinkImageFilter.h>
#include <itkImageFileWriter.h>
#include <vector>
#include <itkCastImageFilter.h>

//for testing
#include "../util/vtkviewer.h"


template<typename pixelComponentT>
class LabelMapToMultipleGrayImagesFilter
{
public:

    //gray scale type alias
    using grayImageType    = itk::Image< pixelComponentT, 2>;
    using grayImageP = typename grayImageType::Pointer;

    //label map type alias
    using labelObjectT = itk::LabelObject<pixelComponentT, 2>;
    using labelMapT    = itk::LabelMap<labelObjectT>;
    using labelMapP    = typename labelMapT::Pointer;

    //vector of gray scale images
    using roisT = std::vector<grayImageP>;

    LabelMapToMultipleGrayImagesFilter();


    //setters
    void setLabelMap(labelMapP labelMap);
    void setGrayImage(grayImageP image);

    //process
    void extractROIs();
    void resizeROIs(unsigned shrinkFactor=2);

    void writeROIs(std::string directory);



private:

    roisT    roiImages;
    labelMapP  labelMap;
    grayImageP inputImage;
    unsigned   sizeThreshold;





};


template  class LabelMapToMultipleGrayImagesFilter<unsigned int>;
template class LabelMapToMultipleGrayImagesFilter<short>;
template class LabelMapToMultipleGrayImagesFilter<unsigned char>;
#endif // LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H
