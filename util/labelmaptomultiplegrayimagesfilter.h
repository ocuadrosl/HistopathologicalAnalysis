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

//local includes

#include "../util/customprint.h"

//for testing
#include "../util/vtkviewer.h"

/*


*/
//TODO implement this class to support gray and rgb images-> template it!!
template<typename pixelComponentT=unsigned int>
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
    using subImagesVT = std::vector<grayImageP>;

    //setters
    void setLabelMap (labelMapP  labelMap);
    void setGrayImage(grayImageP image);

    //getters
    subImagesVT getSubImages() const;


    //process
    void extractSubImages();
    void resizeSubImages(unsigned shrinkFactor=2);

    void writeSubImages(std::string directory,  std::string prefix = "sub-image" ,std::string format="tiff");

    LabelMapToMultipleGrayImagesFilter();

private:

    subImagesVT subImages;
    labelMapP   labelMap;
    grayImageP  inputImage;
    unsigned    sizeThreshold;





};


template  class LabelMapToMultipleGrayImagesFilter<unsigned int>;
template class LabelMapToMultipleGrayImagesFilter<short>;
template class LabelMapToMultipleGrayImagesFilter<unsigned char>;
#endif // LABELMAPTOMULTIPLEGRAYIMAGESFILTER_H
