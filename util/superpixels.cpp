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

/*

    //testing.....
    using castFilterType = itk::CastImageFilter<imageT, labImageT>;
    typename castFilterType::Pointer castfilter = castFilterType::New();
    castfilter->SetInput(inputImage);
    castfilter->Update();
    labImage = castfilter->GetOutput();

    //labImage = inputImage;
*/

}
template<typename imageT>
void SuperPixels<imageT>::create()
{
    rgbToLabImage();

    initRegularGrid();

    //initQuadTreeGrid();


    for(unsigned i =1; i <= iterations ;++i)
    {
        //std::cout<<i<<std::endl;
        updateMeans();
        converge();
    }
   //show();


}

template<typename imageT>
inline bool SuperPixels<imageT>::costFunction(unsigned cLabel, unsigned nLabel,  const labPixelT& cPixel, const labIndexT& cIndex)
{

    //get centroids
    auto cSPCent = indexMeans[cLabel];
    auto nSPCent = indexMeans[nLabel];

    auto cSPMean = colorMeans[cLabel];
    auto nSPMean = colorMeans[nLabel];


    float cost1 = 0;
    float cost2 = 0;

    const auto sedColor = Math::squaredEuclideanDistance<labPixelT>;
    const auto sedIndex = Math::squaredEuclideanDistance<labIndexT, 2>;

    cost1 = lambda1 * sedColor(cPixel, cSPMean) + lambda2 * sedIndex(cIndex, cSPCent);
    cost2 = lambda1 * sedColor(cPixel, nSPMean) + lambda2 * sedIndex(cIndex, nSPCent);

    return cost1 > cost2;

}


template<typename imageT>
void SuperPixels<imageT>::converge()
{


    using flagImageT = itk::Image<bool, 2>;
    flagImageT::Pointer flagImage = flagImageT::New();
    flagImage->SetRegions(inputImage->GetRequestedRegion());
    flagImage->Allocate();
    flagImage->FillBuffer(false);




    using neighborhoodIteratorT = itk::NeighborhoodIterator<labelImageT>;



    neighborhoodIteratorT::RadiusType radius;
    radius.Fill(1);


    itk::NeighborhoodIterator<labelImageT>       labelIt(radius,labelImage, labelImage->GetRequestedRegion()); //should be const
    itk::ConstNeighborhoodIterator<labImageT>  labIt(radius,labImage, labImage->GetRequestedRegion());
    itk::NeighborhoodIterator<flagImageT>        flagIt(radius, flagImage, flagImage->GetRequestedRegion());


    //offsets
    neighborhoodIteratorT::OffsetType top    = {{ 0,-1}};
    neighborhoodIteratorT::OffsetType bottom = {{ 0, 1}};
    neighborhoodIteratorT::OffsetType left   = {{-1, 0}};
    neighborhoodIteratorT::OffsetType right  = {{ 1, 0}};




    //center and neighboor pixels
    unsigned cLabel, nLabel;
    labPixelT cPixel, nPixel;
    labIndexT cIndex;

    labIt.GoToBegin();
    labelIt.GoToBegin();
    flagIt.GoToBegin();
    for (;!labIt.IsAtEnd(); ++labIt, ++labelIt, ++flagIt)
    {
        //4-connected

        cLabel = labelIt.GetCenterPixel();
        cPixel = labIt.GetCenterPixel(); //LAB color
        cIndex = labelIt.GetIndex();

        //TOP
        nLabel = labelIt.GetPixel(top);

        if(cLabel != nLabel && flagIt.GetPixel(top)==false)
        {
            //change the label
            if(costFunction(cLabel, nLabel, cPixel, cIndex))
            {
                labelIt.SetCenterPixel(nLabel);
                flagIt.SetCenterPixel(true);
                continue;
            }
        }

        //bottom
        nLabel = labelIt.GetPixel(bottom);

        if(cLabel != nLabel && flagIt.GetPixel(bottom)==false)
        {
            //change the label
            if(costFunction(cLabel, nLabel, cPixel, cIndex))
            {
                labelIt.SetCenterPixel(nLabel);
                flagIt.SetCenterPixel(true);
                continue;
            }

        }

        //right

        nLabel = labelIt.GetPixel(right);

        if(cLabel != nLabel && flagIt.GetPixel(right)==false)
        {

            //change the label
            if(costFunction(cLabel, nLabel, cPixel, cIndex))
            {
                labelIt.SetCenterPixel(nLabel);
                flagIt.SetCenterPixel(true);
                continue;
            }
        }

        //left

        nLabel = labelIt.GetPixel(left);

        if(cLabel != nLabel && flagIt.GetPixel(left)==false)
        {

            //change the label
            if(costFunction(cLabel, nLabel, cPixel, cIndex))
            {
                labelIt.SetCenterPixel(nLabel);
                flagIt.SetCenterPixel(true);
                continue;
            }
        }


    }


}
template<typename imageT>
void SuperPixels<imageT>::updateMeans()
{

    //colorMeans.clear();
    //indexMeans.clear();
    //spSizes.clear();

    colorMeans = spColorMeansT(spNumber, itk::NumericTraits<labPixelT>::Zero);

    labIndexT index;
    index.Fill(0.0);
    indexMeans = spIndexMeansT(spNumber, index);

    spSizes = spSizesT(spNumber, 0);

    itk::ImageRegionConstIterator<labImageT> labIt(labImage, labImage->GetRequestedRegion());
    itk::ImageRegionConstIteratorWithIndex<labelImageT> labelIt(labelImage, labelImage->GetRequestedRegion());

    //std::cout<<labImage->GetRequestedRegion()<<std::endl;
    //std::cout<<labelImage->GetRequestedRegion()<<std::endl;


    labIt.GoToBegin();
    labelIt.GoToBegin();
    for (;!labIt.IsAtEnd(); ++labIt, ++labelIt)
    {

        const auto &label      = labelIt.Get();
        const auto &labelIndex = labelIt.GetIndex();
        const auto &labColor   = labIt.Get();

        //std::cout<<label<<std::endl;
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
void SuperPixels<imageT>:: setSpNumber(unsigned number)
{

    spNumber = number;
}


template<typename imageT>
void SuperPixels<imageT>::setInitialGrid(labelImageP labelImage)
{

    this->labelImage = labelImage;

}

template<typename imageT>
void SuperPixels<imageT>::initQuadTreeGrid()
{

    std::unique_ptr<QuadTree<imageT>> quadTree(new QuadTree<imageT>());

    quadTree->setImage(inputImage);
    quadTree->build();
    labelImageP labelImage  =  quadTree->getLabelImage();

}


template<typename imageT>
void SuperPixels<imageT>::initRegularGrid()
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

}


template<typename imageT>
void SuperPixels<imageT>::show()
{


    using rgbImageT   = itk::Image<itk::RGBPixel<unsigned char>, 2>;

    rgbImageT::Pointer spImage = rgbImageT::New();
    spImage->SetRegions(inputImage->GetRequestedRegion());
    spImage->Allocate();



    using regionIteratorT = itk::ImageRegionIterator<imageT>;
    using constNeighborhoodIteratorT = itk::ConstNeighborhoodIterator<labelImageT>;

    typename constNeighborhoodIteratorT::RadiusType radius;
    radius.Fill(1);

    constNeighborhoodIteratorT  labelIt(radius,labelImage, labelImage->GetRequestedRegion());
    regionIteratorT  it(inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator<rgbImageT>  spIt(spImage, spImage->GetRequestedRegion());



    labelIt.GoToBegin();
    it.GoToBegin();
    spIt.GoToBegin();

    typename rgbImageT::PixelType red;
    red.SetRed(255);

    unsigned cLabel;
    while (!it.IsAtEnd())
    {
        spIt.Set(it.Get());

        cLabel = labelIt.GetCenterPixel();


        if(cLabel != labelIt.GetPixel({{0,-1}}))
        {
            spIt.Set(red);
        }
        if(cLabel != labelIt.GetPixel({{0,1}}))
        {
            spIt.Set(red);
        }
        if(cLabel != labelIt.GetPixel({{-1,0}}))
        {
            spIt.Set(red);
        }

        if(cLabel != labelIt.GetPixel({{1,0}}))
        {
            spIt.Set(red);
        }


        ++it;
        ++labelIt;
        ++spIt;

    }


    VTKViewer::visualize<rgbImageT>(spImage, "Super Pixels");

}


template<typename imageT>
typename SuperPixels<imageT>::labelImageP
SuperPixels<imageT>::getLabelImage() const
{
    return labelImage;
}














