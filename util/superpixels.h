#ifndef SUPERPIXELS_H
#define SUPERPIXELS_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionIteratorWithIndex.h>


//local includes
#include "../util/colorconverterfilter.h"
#include "../util/math.h"
#include "../util/vtkviewer.h"


template<typename imageT>
class SuperPixels
{

    //types aliases

    using inputImageP = typename imageT::Pointer;

    //CIELAB image type
    using labPixelT = itk::RGBPixel<double>;
    using labImageT = itk::Image<labPixelT, 2>;
    using labImageP = typename labImageT::Pointer;

    using labelImageT = itk::Image<unsigned, 2>;
    using labelImageP = typename labelImageT::Pointer;


    using spMeansT = std::vector<labPixelT>;
    using spCentroidsT = std::vector<std::vector<double>>;
    using spSizesT = std::vector<unsigned>;




public:

    SuperPixels();

    void setImage(inputImageP inputImage);
    void create();


private:

    labImageP   labImage;
    inputImageP inputImage;
    labelImageP  labelImage;

    spMeansT means;
    spCentroidsT centroids;
    spSizesT spSizes;

    unsigned sideLength = 10;
    double   alpha1     = 1;
    double   alpha2     = 0.5;
    unsigned iterations = 5;

    unsigned long spNumber=0;


    void rgbToLabImage();
    void createLabelImage();
    void updateMeans();
    void converge();

    inline bool isEdge(const labelImageT::IndexType& index);


};

using imageDouble   = itk::Image<itk::RGBPixel<double  >, 2>;
using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;

template class SuperPixels<imageDouble>;
template class SuperPixels<imageUnsigned>;


#endif // SUPERPIXELS_H
