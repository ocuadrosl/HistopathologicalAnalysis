#include "rgbhistogramfilter.h"

template< typename rgbImageT >
RGBHistogramFilter<rgbImageT>::RGBHistogramFilter()
{

    minPossibleValues.Set(0  , 0  , 0 );
    maxPossibleValues.Set(255, 255, 255);

}


template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::setImage(imageP inputImage)
{

    this->inputImage = inputImage;

}


template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>:: setNumberOfBins(unsigned red, unsigned green, unsigned blue)
{
    nBinsRed   = red;
    nBinsGreen = green;
    nBinsBlue  = blue;
}


template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::setMaxPossibleValues(const pixelT& max)
{
    maxPossibleValues = max;
}

template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::setMinPossibleValues(const pixelT& min)
{
    minPossibleValues = min;
}

template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::setMaxPossibleValues(pixelCompT r, pixelCompT g, pixelCompT b)
{
    maxPossibleValues.Set(r, g, b);
}

template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::setMinPossibleValues(pixelCompT r, pixelCompT g, pixelCompT b)
{
    minPossibleValues.Set(r, g, b);

}
template< typename rgbImageT >
typename RGBHistogramFilter<rgbImageT>::rgbHistogramT
RGBHistogramFilter<rgbImageT>::getHistogram()
{

    return rgbHistogram;

}


template< typename rgbImageT >
typename RGBHistogramFilter<rgbImageT>::rgbHistogramT
RGBHistogramFilter<rgbImageT>::getCumulativeDistribution()
{

    return rgbCumulativeDistribution;

}


template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::computeComulativeDistribution()
{

    //allocating the cumulative distribution
    rgbCumulativeDistribution = rgbHistogramT(3);

    rgbCumulativeDistribution[0] = histogramT(rgbHistogram[0].size(), 0);
    rgbCumulativeDistribution[1] = histogramT(rgbHistogram[1].size(), 0);
    rgbCumulativeDistribution[2] = histogramT(rgbHistogram[2].size(), 0);



    //cI = channel iterator
    for (unsigned cI = 0; cI< 3; ++cI)
    {
        auto hIt  = rgbHistogram[cI].begin()+1;
        auto cuIt = rgbCumulativeDistribution[cI].begin()+1;

        *(cuIt-1) = *(hIt-1);

        while(hIt != rgbHistogram[cI].end() )
        {
            (*cuIt) = *(cuIt-1) + *(hIt);

            ++hIt;
            ++cuIt;

            std::cout<<*hIt<<std::endl;
        }
    }


    IO::printOK("Cumulative distribution");

}


template< typename rgbImageT >
void RGBHistogramFilter<rgbImageT>::computeHistogram()
{

    //Number of bins was not set
    if(nBinsRed == 0)
    {
        nBinsRed   = maxPossibleValues[0] - minPossibleValues[0];
        nBinsGreen = maxPossibleValues[1] - minPossibleValues[1];
        nBinsBlue  = maxPossibleValues[2] - minPossibleValues[2];
    }


    //allocating the histogram
    rgbHistogram = rgbHistogramT(3);

    rgbHistogram[0] = histogramT(nBinsRed  , 0);
    rgbHistogram[1] = histogramT(nBinsGreen, 0);
    rgbHistogram[2] = histogramT(nBinsBlue , 0);


    //Function objects Min-max function
    Math::MinMax<> minMaxR(minPossibleValues[0], maxPossibleValues[0], 0, nBinsRed  );
    Math::MinMax<> minMaxG(minPossibleValues[1], maxPossibleValues[1], 0, nBinsGreen);
    Math::MinMax<> minMaxB(minPossibleValues[2], maxPossibleValues[2], 0, nBinsBlue );


    itk::ImageRegionConstIterator<rgbImageT> it(inputImage,  inputImage->GetRequestedRegion());

    pixelT pixelAux;

    while(!it.IsAtEnd())
    {
        pixelAux = it.Get();

        ++rgbHistogram[0][ std::floor(minMaxR(pixelAux[0])) ];
        ++rgbHistogram[1][ std::floor(minMaxG(pixelAux[1])) ];
        ++rgbHistogram[2][ std::floor(minMaxB(pixelAux[2])) ];
        ++it;

    }


    IO::printOK("Computing Histogram");

}
