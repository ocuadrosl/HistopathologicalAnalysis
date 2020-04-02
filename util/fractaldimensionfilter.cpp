#include "fractaldimensionfilter.h"

template <typename ImageT>
FractalDimensionFilter<ImageT>::FractalDimensionFilter()
{


}


template <typename ImageT>
void FractalDimensionFilter<ImageT>::SetInputImage(ImageP InputImage)
{

    this->InputImage = InputImage;

}

template <typename ImageT>
unsigned FractalDimensionFilter<ImageT>::CountTiles(unsigned tileLength)
{


    unsigned tilesNumber = 0;

    const auto region = InputImage->GetRequestedRegion();

    const auto upperIndexCols = region.GetUpperIndex()[0];
    const auto upperIndexRows = region.GetUpperIndex()[1];
    const auto indexCols = region.GetIndex()[0];
    const auto indexRows = region.GetIndex()[1];
    const auto sizeRows = region.GetSize()[1];
    const auto sizeCols = region.GetSize()[0];

    typename ImageT::RegionType tile;



    auto getSizeRow = [tileLength, upperIndexRows, sizeRows](unsigned tileRow)
    {
        return ( tileRow + tileLength <= upperIndexRows )? tileLength : upperIndexRows - tileRow;
    };

    auto getSizeCol = [tileLength, upperIndexCols, sizeCols](unsigned tileCol)
    {
        return ( tileCol + tileLength <= upperIndexCols )? tileLength : upperIndexCols - tileCol;
    };


    unsigned tileSizeCol, tileSizeRow;
    for(unsigned tileRow = indexRows; tileRow <= upperIndexRows ;  tileRow += tileLength)
    {


        for(unsigned tileCol = indexCols; tileCol <= upperIndexCols ;  tileCol += tileLength)
        {


            tile.SetIndex({{tileCol, tileRow}});
            tileSizeCol = getSizeCol(tileCol);
            tileSizeRow = getSizeRow(tileRow);
            tile.SetSize({{tileSizeCol, tileSizeRow}});

            itk::ImageRegionConstIterator<ImageT> it(InputImage, tile);
            for(it; !it.IsAtEnd(); ++it)
            {
                if(it.Get() != background)
                {

                    ++tilesNumber;
                    break;
                }

            }

        }

    }


    return tilesNumber;

}

template <typename ImageT>
float FractalDimensionFilter<ImageT>:: GetDimension() const
{
    return dimension;
}

template <typename ImageT>
void FractalDimensionFilter<ImageT>::SetScale(float scale)
{

    this->scale = scale;
}

template <typename ImageT>
void FractalDimensionFilter<ImageT>::SetResolution(unsigned resolution)
{
    this->resolution = resolution;

}

template <typename ImageT>
void FractalDimensionFilter<ImageT>::SetUnitTileLenght(unsigned tileLength)
{

    this->unitTileLenght = tileLength;
}


template <typename ImageT>
void FractalDimensionFilter<ImageT>::PrintWarningsOff()
{
    printWarnings = false;

}

template <typename ImageT>
void FractalDimensionFilter<ImageT>::Compute()
{


    const auto imgSize  = InputImage->GetLargestPossibleRegion().GetSize();
    if(imgSize[0] < unitTileLenght  || imgSize[1] < unitTileLenght )
    {
        auto tmpLength = unitTileLenght;
        unitTileLenght = (imgSize[0] > imgSize[1])? imgSize[1] : imgSize[0];
        if(printWarnings)
        {
            std::cout<<"Fractal Dimension Warning: Unit tile lenght reduced from "<<tmpLength<<" to "<< unitTileLenght <<std::endl;
        }

    }


    std::vector<float> scales;

    //approximating resolutions
    scales.push_back(unitTileLenght);

    for(unsigned i=1; i <= resolution-1; ++i)
    {
        float s = *scales.rbegin() * scale;
        if(s >= 1.f)
        {
            scales.push_back(std::ceil(s));
            //std::cout<<std::ceil(s)<<std::endl;
        }
        else
        {
            if(printWarnings)
            {
                std::cout<<"Fractal Dimension Warning: resolution adjusted from "<<resolution<<" to "<< i <<std::endl;
            }
            break;

        }

    }


    //lambda
    //Fractal dimension function: 1 for large tiles, 2 for small tiles
    auto fractalDim = [](unsigned tiles1, double scale1, unsigned tiles2, double scale2) //double because of implicit conversion float to double
    {

        return ( std::log(tiles2) - std::log(tiles1)) / (std::log(1/scale2) - std::log(1/scale1));

    };


    //computing fractal dimension for each scale
    std::vector<float> dimensions;
    float currentScale = 1.f;
    for(auto it = scales.begin()+1; it != scales.end(); ++it)
    {

        auto tiles1 = CountTiles(*(it-1)); //large tiles
        auto tiles2 = CountTiles(*(it)); //small tiles

        dimensions.push_back(fractalDim(tiles1, currentScale, tiles2, currentScale*scale));

        currentScale*=scale;

    }

    //average fractal dimension
    dimension = std::accumulate(dimensions.begin(), dimensions.end(), 0.f) / dimensions.size();


    //std::cout<<dimension<<std::endl;


}


