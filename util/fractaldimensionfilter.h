#ifndef FRACTALDIMENSIONFILTER_H
#define FRACTALDIMENSIONFILTER_H

#include <itkImage.h>
#include <cmath>
#include <numeric>
#include <set>
#include <itkImageRegionConstIterator.h>


#include "../util/math.h"


template<typename ImageT>
class FractalDimensionFilter
{

    using ImageP = typename ImageT::Pointer;

    const unsigned background=0;


public:
    FractalDimensionFilter();
    void SetInputImage(ImageP InputImage);
    void Compute();
    void SetUnitTileLenght(unsigned lenght); //in pixels
    void SetResolution(unsigned reolution);
    void SetScale(float scale);
    void PrintWarningsOff();

    float GetDimension() const;




private:

    ImageP InputImage;

    unsigned unitTileLenght = 10;
    unsigned resolution     = 2;
    float    scale          = 0.5f; //must be in [0 1];
    float    dimension      = 0.f; //result
    bool     printWarnings  = true;


    unsigned CountTiles(unsigned unitTileLenght);


};

template class FractalDimensionFilter<itk::Image<unsigned char, 2>>;
template class FractalDimensionFilter<itk::Image<unsigned, 2>>;

#endif // FRACTALDIMENSIONFILTER_H
