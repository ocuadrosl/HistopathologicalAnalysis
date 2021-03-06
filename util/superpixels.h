#ifndef SUPERPIXELS_H
#define SUPERPIXELS_H

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNeighborhoodIterator.h>
#include <itkConstNeighborhoodIterator.h>

#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelToRGBImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkImageFileWriter.h>

//local includes
#include "../util/colorconverterfilter.h"
#include "../util/math.h"
#include "../util/vtkviewer.h"

#include "../dataStructures/quadtree.h"




template<typename imageT>
class SuperPixels
{

    //types aliases

    using inputImageP = typename imageT::Pointer;

    //CIELAB image type
    using labPixelT = itk::RGBPixel<float>;
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

    void setInitialGrid(labelImageP labelImage);
    void setSpNumber(unsigned number);

    unsigned GetNumberOfSuperPixels() const;

    labelImageP getLabelImage() const;


private:

    labImageP   labImage;
    inputImageP inputImage;
    labelImageP labelImage;

    spColorMeansT colorMeans;
    spIndexMeansT indexMeans;
    spSizesT spSizes;

    unsigned sideLength = 15; //20
    float   lambda1    = 1;
    float   lambda2    = 0.5;
    unsigned iterations = 20; //25

    unsigned spNumber = 0;


    void rgbToLabImage();
    void initRegularGrid();
    void updateMeans();
    void converge();
    void initQuadTreeGrid();

    inline bool costFunction(unsigned cLabel, unsigned nLabel,  const labPixelT& cPixel,  const labIndexT& cIndex);


};

//using imageDouble   = itk::Image<itk::RGBPixel<double  >, 2>;
using imageUnsigned = itk::Image<itk::RGBPixel<unsigned>, 2>;

//template class SuperPixels<itk::Image<float   , 2>>;
//template class SuperPixels<itk::Image<unsigned, 2>>;
template class SuperPixels<imageUnsigned>;


#endif // SUPERPIXELS_H
