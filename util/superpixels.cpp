#include "superpixels.h"


template<typename imageT>
SuperPixels<imageT>::SuperPixels()
{


}

template<typename imageT>
void SuperPixels<imageT>::setImage(inputImageP inputImage)
{

    this->inputImage = inputImage;


}

template<typename imageT>
void SuperPixels<imageT>::rgbToLabImage()
{

    using rgbToXyzFilterT = ColorConverterFilter<imageT, labImageT>;
    std::unique_ptr< rgbToXyzFilterT> rgbToXyzFilter(new rgbToXyzFilterT());
    rgbToXyzFilter->setInput(inputImage);
    rgbToXyzFilter->rgbToXyz();

    using xyzToLabFilterT = ColorConverterFilter<labImageT, labImageT>;
    std::unique_ptr< xyzToLabFilterT> xyzToLabFilter(new xyzToLabFilterT());
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();

    labImage = xyzToLabFilter->getOutput();


}
template<typename imageT>
void SuperPixels<imageT>::create()
{
    rgbToLabImage();
    createLabelImage();
    updateMeans();

    converge();

}

template<typename imageT>
inline bool SuperPixels<imageT>::isEdge(const labelImageT::IndexType& index)
{

    //4-connected


}


template<typename imageT>
void SuperPixels<imageT>::converge()
{




}
template<typename imageT>
void SuperPixels<imageT>::updateMeans()
{



    means.resize(spNumber, itk::NumericTraits<labPixelT>::Zero);
    centroids.resize(spNumber, std::vector<double>(2,0));
    spSizes.resize(spNumber, 0);

    itk::ImageRegionConstIterator<labImageT> labIt(labImage, labImage->GetRequestedRegion());
    itk::ImageRegionConstIteratorWithIndex<labelImageT> labelIt(labelImage, labelImage->GetRequestedRegion());

    unsigned  label;
    labelImageT::IndexType labelIndex;
    labPixelT labColor;

    for (;!labIt.IsAtEnd(); ++labIt, ++labelIt)
    {

        label      = labelIt.Get();
        labelIndex = labelIt.GetIndex();
        labColor   = labIt.Get();

        means[label][0] += labColor[0];
        means[label][1] += labColor[1];
        means[label][2] += labColor[2];

        centroids[label][0] += labelIndex[0];
        centroids[label][1] += labelIndex[1];

        ++spSizes[label];
    }


    for(unsigned i =0 ; i< spNumber ;++i)
    {
        centroids[i][0] /= spSizes[i];
        centroids[i][1] /= spSizes[i];

        means[i][0] /= spSizes[i];
        means[i][1] /= spSizes[i];
        means[i][2] /= spSizes[i];

    }

}


template<typename imageT>
void SuperPixels<imageT>::createLabelImage()
{



    labelImage = labelImageT::New();
    labelImage->SetRegions(inputImage->GetRequestedRegion());
    labelImage->Allocate();


    itk::ImageRegionIteratorWithIndex<labelImageT> it(labelImage, labelImage->GetRequestedRegion());


    auto size = inputImage->GetRequestedRegion().GetSize();

    unsigned spWidth =  (size[0]-1)/sideLength;
    unsigned spHeight = (size[1]-1)/sideLength;

    Math::MinMax<unsigned, unsigned> minMaxW(0, size[0]-1, 0, spWidth);
    Math::MinMax<unsigned, unsigned> minMaxH(0, size[1]-1, 0, spHeight);


    unsigned w, h;
    for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {

       w = minMaxW(static_cast<unsigned>(it.GetIndex()[0]));
       h = minMaxH(static_cast<unsigned>(it.GetIndex()[1]));

      // std::cout<<x<<","<<y<<std::endl;

       it.Set(w*spHeight+h);
     }


    //updating thge number of super pixels
    spNumber = (spWidth*spHeight+spHeight)+1;

    //VTKViewer::visualize<labelImageT>(labelImage, "Super pixels initial grid");


}
