#include "labelmaptomultiplegrayimagesfilter.h"


template<typename pixelComponentT>
LabelMapToMultipleGrayImagesFilter<pixelComponentT>::LabelMapToMultipleGrayImagesFilter()
{

}


template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::setLabelMap( labelMapP labelMap)
{
    this->labelMap = labelMap;
}

template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::createImages()
{

    labelObjectT * labelObject;

    for(unsigned int i = 0; i < labelMap->GetNumberOfLabelObjects(); ++i)
    {
        labelObject = labelMap->GetNthLabelObject(i);
    }




}
