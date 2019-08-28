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

    for(unsigned int objI = 0; objI < labelMap->GetNumberOfLabelObjects(); ++objI)
    {
        labelObject = labelMap->GetNthLabelObject(objI);



        for(unsigned int pixelId = 0; pixelId < labelObject->Size(); pixelId++)
        {
            //std::cout << labelObject->GetIndex(pixelId);
        }



    }







}
