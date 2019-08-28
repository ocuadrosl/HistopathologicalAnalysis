#include "labelmaptomultiplegrayimagesfilter.h"


template<typename pixelComponentT>
LabelMapToMultipleGrayImagesFilter<pixelComponentT>::LabelMapToMultipleGrayImagesFilter():
    sizeThreshold(1000)
{

}


template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::setLabelMap( labelMapP labelMap)
{
    this->labelMap = labelMap;
}

template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::extractROIs()
{

    labelObjectT * labelObj;



    //white color
    const auto white = itk::NumericTraits<pixelComponentT>::Zero+255;

    for(unsigned int objI = 0; objI < labelMap->GetNumberOfLabelObjects(); ++objI)
    {
        labelObj = labelMap->GetNthLabelObject(objI);

        //aux index
        auto index = labelObj->GetIndex(0);
        if(labelObj->Size() <  sizeThreshold)
        {
            continue;
        }

        auto roi  = grayImageType::New();
        roi->SetRegions(inputImage->GetRequestedRegion());
        roi->Allocate();
        roi->FillBuffer(white);

        for(unsigned int pixelId = 0; pixelId < labelObj->Size(); ++pixelId)
        {
            //std::cout << labelObject->GetIndex(pixelId);
            index = labelObj->GetIndex(pixelId);
            roi->SetPixel( index, inputImage->GetPixel(index));

        }

        roiImages.push_back(roi);
        //VTKViewer<pixelComponentT>::visualizeGray(*roiImages.rbegin(), "ROI");
    }


}



template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::setGrayImage(grayImageP image)
{
    inputImage = image;
}


template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::resizeROIs(unsigned shrinkFactor)
{

    using shrinkImageFilterT = itk::ShrinkImageFilter< grayImageType, grayImageType>;


    roisT resizedROIs;

    for(typename roisT::iterator it = roiImages.begin();  it != roiImages.end(); ++it)
    {


        typename shrinkImageFilterT::Pointer shrinkImageFilter = shrinkImageFilterT::New();
        shrinkImageFilter->SetShrinkFactors(shrinkFactor);
        shrinkImageFilter->SetInput(*it);
        shrinkImageFilter->Update();
        (*it) = shrinkImageFilter->GetOutput();

    }


}


template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::writeROIs(std::string directory)
{


    using imageChar = itk::Image< unsigned char, 2 >;
    using WriterType = itk::ImageFileWriter<imageChar>;
    typename WriterType::Pointer writer = WriterType::New();

    //add forward slash
    if(*directory.rbegin() != '/')
    {
        directory.push_back('/');
    }


    //TIFF only supports short, float, and char types

    using castFilterCastType = itk::CastImageFilter< grayImageType, imageChar >;
    typename castFilterCastType::Pointer castFilter = castFilterCastType::New();



    std::string fileName="";
    unsigned roiI = 1; //roi index
    for(typename roisT::iterator it = roiImages.begin();  it != roiImages.end(); ++it, ++roiI)
    {


        fileName = "roi_"+std::to_string(roiI) + ".tiff";

        std::cout<<directory+fileName<<std::endl;

        castFilter->SetInput(*it);
        castFilter->Update();

       // VTKViewer<unsigned char>::visualizeGray(castFilter->GetOutput(), "High density");

        writer->SetFileName(directory+fileName);
        writer->SetInput(castFilter->GetOutput());

        try
        {
            writer->Update();
        }
        catch( itk::ExceptionObject & error )
        {
            std::cerr << "Error: " << error << std::endl;
            return;
        }

    }






}
















