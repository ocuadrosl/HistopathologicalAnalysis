#include "cellsegmentator.h"


template<typename imageT>
CellSegmentator<imageT>::CellSegmentator()
{

}

template<typename imageT>
void CellSegmentator<imageT>::setImage(imageP inputImage)
{

    this->inputImage = inputImage;

}


template<typename imageT>
void CellSegmentator<imageT>::computeGradients()
{

    // Create and setup a gradient filter
    if(grayImage.IsNull())
    {
        createGrayImage();
    }


    using gradientFilterT = itk::GradientImageFilter<grayImageT, float>;
    typename gradientFilterT::Pointer gradientFilter = gradientFilterT::New();
    gradientFilter->SetInput( grayImage );
    gradientFilter->Update();
    outputImage = gradientFilter->GetOutput();

    IO::printOK("Computing Gradients");

}

template<typename imageT>
typename CellSegmentator<imageT>::vectorImageP
CellSegmentator<imageT>::getGradients() const
{
    return outputImage;
}


template<typename imageT>
void CellSegmentator<imageT>::superPixels()
{


    using superPixelsT =  SuperPixels<imageT>;

    std::unique_ptr<superPixelsT> superPixels(new superPixelsT());

    superPixels->setImage(inputImage);
    superPixels->create();

     IO::printOK("Creating Super Pixels");

}

template<typename imageT>
void CellSegmentator<imageT>::findCellNuclei()
{

    if(grayImage.IsNull())
    {
        createGrayImage();
    }



    computeLoGNorm();

}



template<typename imageT>
void CellSegmentator<imageT>::createGrayImage()
{
    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter< imageT, grayImageT >;
    typename rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    grayImage = rgbToGrayFilter->GetOutput();


}


template<typename imageT>
void CellSegmentator<imageT>::computeLoGNorm()
{


    LoGNorm = image3DT::New();

    image3DT::RegionType region;

    image3DT::SizeType size;
    size[0] = inputImage->GetRequestedRegion().GetSize()[0];
    size[1] = inputImage->GetRequestedRegion().GetSize()[1];

    unsigned normSize = 10;
    size[2] = normSize;

    region.SetSize(size);

    LoGNorm->Allocate();


    using logFilterT = LoGFilter<grayImageT, grayImageD>;
    std::unique_ptr<logFilterT> logFilter(new logFilterT);
    logFilter->setImage(grayImage);


    itk::FixedArray<unsigned, 3> layout;
    layout[0] = 1;
    layout[1] = 1;
    layout[2] = 0;


    std::vector<grayImageDP> stack;


    for(double sigma=0.1; sigma<= 1.0; sigma += 0.1)
    {

       logFilter->setSigma(sigma);
       logFilter->compute();

       stack.push_back(logFilter->getOutput());
       (*stack.rbegin())->DisconnectPipeline();

       itk::ViewImage<grayImageD>::View(*stack.rbegin());

    }




    //TODO iterators here

    for(auto it = stack.begin(); it != stack.end(); ++it)
    {



    }




   // itk::ViewImage<image3DT>::View(LoGNorm);




}








