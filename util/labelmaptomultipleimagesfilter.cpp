#include "labelmaptomultipleimagesfilter.h"

template<typename imageT, typename labelMapT>
LabelMapToMultipleImagesFilter<imageT, labelMapT>::LabelMapToMultipleImagesFilter():
    sizeThreshold(1000)
{

}


template<typename imageT, typename labelMapT>
void LabelMapToMultipleImagesFilter<imageT, labelMapT>::setLabelMap(labelMapP labelMap)
{
     this->labelMap = labelMap;
}



template<typename imageT, typename labelMapT>
void LabelMapToMultipleImagesFilter<imageT, labelMapT>::extractImages()
{

    IO::printWait("Extracting images");

    labelObjT * labelObj;


    //white color
    const auto white = itk::NumericTraits<pixelT>::Zero+255;

    for(unsigned int objI = 0; objI < labelMap->GetNumberOfLabelObjects(); ++objI)
    {

        labelObj = labelMap->GetNthLabelObject(objI);

        if(labelObj->Size() <  sizeThreshold)
        {
            continue;
        }


        auto image  = imageT::New();
        image->SetRegions(inputImage->GetRequestedRegion());
        image->Allocate();
        image->FillBuffer(white);

        //aux index
        auto index = labelObj->GetIndex(0);
        for(unsigned int pixelId = 0; pixelId < labelObj->Size(); ++pixelId)
        {
            index = labelObj->GetIndex(pixelId);
            image->SetPixel( index, inputImage->GetPixel(index));

        }


        images.push_back(image);

    }

    IO::printOK("Extracting images");

}



template<typename imageT, typename labelMapT>
void LabelMapToMultipleImagesFilter<imageT, labelMapT>::setImage(imageP inputImage)
{
    this->inputImage = inputImage;
}




template<typename imageT, typename labelMapT>
void LabelMapToMultipleImagesFilter<imageT, labelMapT>::resizeImages(unsigned shrinkFactor)
{

    using shrinkImageFilterT = itk::ShrinkImageFilter< imageT, imageT>;

    for(typename imagesT::iterator it = images.begin();  it != images.end(); ++it)
    {


        typename shrinkImageFilterT::Pointer shrinkImageFilter = shrinkImageFilterT::New();
        shrinkImageFilter->SetShrinkFactors(shrinkFactor);
        shrinkImageFilter->SetInput(*it);
        shrinkImageFilter->Update();
        (*it) = shrinkImageFilter->GetOutput();

    }

    IO::printOK("Resizing images");



}



template<typename imageT, typename labelMapT>
void LabelMapToMultipleImagesFilter<imageT, labelMapT>::writeImages(std::string directory, std::string prefix, std::string format)
{


    //TODO add support RGB images
    // std::cout<<typeid(pixelT).name()<<std::endl;

    using imageChar  = itk::Image< unsigned char, 2 >;
    using WriterType = itk::ImageFileWriter<imageChar>;

    typename WriterType::Pointer writer = WriterType::New();

    //add forward slash
    if(*directory.rbegin() != '/')
    {
        directory.push_back('/');
    }

    //TIFF only supports short, float, and char types
    using castFilterCastType = itk::CastImageFilter< imageT, imageChar >;
    typename castFilterCastType::Pointer castFilter = castFilterCastType::New();


    std::string fileName="";
    unsigned imgI = 1; //roi index
    for(typename imagesT::iterator it = images.begin();  it != images.end(); ++it, ++imgI)
    {

        fileName = prefix+"_"+std::to_string(imgI) + "."+format;

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

    IO::printOK("Writing images");

}


