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
inline bool SuperPixels<imageT>::changeLabel(unsigned cLabel, unsigned nLabel,  const labPixelT& cPixel, const labPixelT& nPixel)
{

    //get centroids
    auto cCent = indexMeans[cLabel];
    auto nCent = indexMeans[nLabel];

    auto cMean = colorMeans[cLabel];
    auto nMean = colorMeans[nLabel];

    //todo kmeans function here




}


template<typename imageT>
void SuperPixels<imageT>::converge()
{


    using constNeighborhoodIteratorT = itk::ConstNeighborhoodIterator<labImageT>;
    using neighborhoodIteratorT = itk::NeighborhoodIterator<labelImageT>;

    constNeighborhoodIteratorT::RadiusType radius;
    radius.Fill(1);


    neighborhoodIteratorT  labelIt(radius,labelImage, labelImage->GetRequestedRegion()); //should be const
    constNeighborhoodIteratorT  labIt(radius,labImage, labImage->GetRequestedRegion());

    //offsets
    constNeighborhoodIteratorT::OffsetType top = {{0,-1}};




    //center and neighboor pixels
    unsigned cLabel, nLabel;
    labPixelT cPixel, nPixel;


    for (labIt.GoToBegin(), labelIt.GoToBegin() ;!labIt.IsAtEnd(); ++labIt, ++labelIt)
    {
        //4-connected
        //top
        cLabel = labelIt.GetCenterPixel();
        nLabel = labelIt.GetPixel(top);

        cPixel = labIt.GetCenterPixel();
        nPixel = labIt.GetPixel(top);


    }








}
template<typename imageT>
void SuperPixels<imageT>::updateMeans()
{



    colorMeans.resize(spNumber, itk::NumericTraits<labPixelT>::Zero);
    indexMeans.resize(spNumber, std::vector<double>(2,0));
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

        colorMeans[label][0] += labColor[0];
        colorMeans[label][1] += labColor[1];
        colorMeans[label][2] += labColor[2];

        indexMeans[label][0] += labelIndex[0];
        indexMeans[label][1] += labelIndex[1];

        ++spSizes[label];
    }


    for(unsigned i =0 ; i< spNumber ;++i)
    {
        indexMeans[i][0] /= spSizes[i];
        indexMeans[i][1] /= spSizes[i];

        colorMeans[i][0] /= spSizes[i];
        colorMeans[i][1] /= spSizes[i];
        colorMeans[i][2] /= spSizes[i];

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
