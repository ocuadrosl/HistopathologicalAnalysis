#ifndef SUPERPIXELS_H
#define SUPERPIXELS_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNeighborhoodIterator.h>
#include <itkConstNeighborhoodIterator.h>

#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelToRGBImageFilter.h>

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
    using labIndexT = labImageT::IndexType;


    using labelImageT = itk::Image<unsigned, 2>;
    using labelImageP = typename labelImageT::Pointer;



    using spColorMeansT = std::vector<labPixelT>;
    using spIndexMeansT = std::vector<labIndexT>;
    using spSizesT = std::vector<unsigned>;




public:

    SuperPixels();

    void setImage(inputImageP inputImage);
    void create();
    void show();


private:

    labImageP   labImage;
    inputImageP inputImage;
    labelImageP  labelImage;

    spColorMeansT colorMeans;
    spIndexMeansT indexMeans;
    spSizesT spSizes;

    unsigned sideLength = 10;
    double   lambda1    = 1;
    double   lambda2    = 0.5;
    unsigned iterations = 5;

    unsigned long spNumber=0;


    void rgbToLabImage();
    void createLabelImage();
    void updateMeans();
    void converge();

    inline bool isEdge(const labelImageT::IndexType& index);

    inline bool changeLabel(unsigned cLabel, unsigned nLabel,  const labPixelT& cPixel,  const labIndexT& cIndex);


};

using imageDouble   = itk::Image<itk::RGBPixel<double  >, 2>;
using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;

template class SuperPixels<imageDouble>;
template class SuperPixels<imageUnsigned>;


#endif // SUPERPIXELS_H
