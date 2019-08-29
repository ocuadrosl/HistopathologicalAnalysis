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
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::extractSubImages()
{

    IO::printWait("Extracting sub-images");

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

        subImages.push_back(roi);
        //VTKViewer<pixelComponentT>::visualizeGray(*roiImages.rbegin(), "ROI");
    }


    IO::printOK("Extracting sub-images");


}



template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::setGrayImage(grayImageP image)
{
    inputImage = image;
}


template<typename pixelComponentT>
void LabelMapToMultipleGrayImagesFilter<pixelComponentT>::resizeSubImages(unsigned shrinkFactor)
{

    using shrinkImageFilterT = itk::ShrinkImageFilter< grayImageType, grayImageType>;


    subImagesVT resizedROIs;

    for(typename subImagesVT::iterator it = subImages.begin();  it != subImages.end(); ++it)
    {


        typename shrinkImageFilterT::Pointer shrinkImageFilter = shrinkImageFilterT::New();
        shrinkImageFilter->SetShrinkFactors(shrinkFactor);
        shrinkImageFilter->SetInput(*it);
        shrinkImageFilter->Update();
        (*it) = shrinkImageFilter->GetOutput();

    }

    IO::printOK("Resizing sub-images");


}


template<typename pixelComponentT>
void
LabelMapToMultipleGrayImagesFilter<pixelComponentT>::writeSubImages(std::string directory ,std::string prefix ,std::string format)
{


    using imageChar  = itk::Image< unsigned char, 2 >;
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
    unsigned subI = 1; //roi index
    for(typename subImagesVT::iterator it = subImages.begin();  it != subImages.end(); ++it, ++subI)
    {


        fileName = prefix+"_"+std::to_string(subI) + "."+format;

        //std::cout<<directory+fileName<<std::endl;

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


    IO::printOK("Writing sub-images");

}


template<typename pixelComponentT>
typename LabelMapToMultipleGrayImagesFilter<pixelComponentT>::subImagesVT
LabelMapToMultipleGrayImagesFilter<pixelComponentT>::getSubImages() const
{
    return subImages;

}













