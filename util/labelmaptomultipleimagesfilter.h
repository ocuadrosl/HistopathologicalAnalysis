#ifndef LABELMAPTOMULTIPLEIMAGESFILTER_H
#define LABELMAPTOMULTIPLEIMAGESFILTER_H

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



template<typename imageT, typename labelMapT>
class LabelMapToMultipleImagesFilter
{
public:


    using pixelT = typename imageT::PixelType;
    using imageP = typename imageT::Pointer;


    using labelMapP   = typename labelMapT::Pointer;
    using labelObjT = typename labelMapT::LabelObjectType;

    //vector of images
    using imagesT = std::vector<imageP>;


    //setters
    void setLabelMap(labelMapP  labelMap);
    void setImage(imageP image);

    //getters
    imagesT getImages() const;


    //process
    void extractImages();
    void resizeImages(unsigned shrinkFactor=2);

    void writeImages(std::string directory, std::string prefix = "image", std::string format="tiff");

    LabelMapToMultipleImagesFilter();

private:

    imagesT   images;
    labelMapP labelMap;
    imageP    inputImage;
    unsigned  sizeThreshold;

};

template class LabelMapToMultipleImagesFilter< itk::Image<unsigned, 2>, itk::LabelMap< itk::LabelObject<unsigned, 2> > >;
template class LabelMapToMultipleImagesFilter< itk::Image<itk::RGBPixel<unsigned>, 2>, itk::LabelMap< itk::LabelObject<unsigned, 2> > >;

//template class LabelMapToMultipleImagesFilter< itk::Image<unsigned int, 2u>, itk::LabelMap<itk::LabelObject<unsigned int, 2u> > >;



#endif // LABELMAPTOMULTIPLEIMAGESFILTER_H
