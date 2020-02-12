#include "cellsegmentator.h"


template<typename rgbImageT>
CellSegmentator<rgbImageT>::CellSegmentator()
{

}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::setImage(imageP inputImage)
{

    this->inputImage = inputImage;

}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::CreateImageB(bool show)
{

    //RGB to XYZ
    using rgbToXyzFilterT = ColorConverterFilter<rgbImageT, labImageT>;
    std::unique_ptr< rgbToXyzFilterT> rgbToXyzFilter(new rgbToXyzFilterT());
    rgbToXyzFilter->setInput(inputImage);
    rgbToXyzFilter->rgbToXyz();


    //XYZ to LAB
    using xyzToLabFilterT = ColorConverterFilter<labImageT, labImageT>;
    std::unique_ptr< xyzToLabFilterT> xyzToLabFilter(new xyzToLabFilterT());
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();


    //Extracting B channel
    using ExtractChannelFilterT = ExtractChannelFilter<labImageT, floatImageT>;
    std::unique_ptr<ExtractChannelFilterT> extractChannelFilter(new ExtractChannelFilterT());

    extractChannelFilter->setImputImage(xyzToLabFilter->getOutput());
    extractChannelFilter->extractChannel(2);



    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, floatImageT>;
    rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(extractChannelFilter->getOutputImage());
    rescaleFilter->SetOutputMinimum(-1.f);
    rescaleFilter->SetOutputMaximum(1.f);
    rescaleFilter->Update();
    BImage = rescaleFilter->GetOutput();


    //BImage = extractChannelFilter->getOutputImage();


    //visualizing
    if(show)
    {

        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
        rescaleFilter2->SetInput(BImage);
        rescaleFilter2->SetOutputMinimum(0);
        rescaleFilter2->SetOutputMaximum(255);
        rescaleFilter2->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter2->GetOutput() ,"b channel Image");
    }


}

template<typename rgbImageT>
typename CellSegmentator<rgbImageT>::floatImageP
CellSegmentator<rgbImageT>::computeDistances(floatImageP inputImage, bool show)
{

    auto outputImage =  floatImageT::New();
    outputImage->SetRegions(BImage->GetRequestedRegion());
    outputImage->Allocate();



    using neighborhoodIteratorT = itk::NeighborhoodIterator<floatImageT>;
    neighborhoodIteratorT::RadiusType radius;
    radius.Fill(10);

    itk::NeighborhoodIterator<floatImageT> it(radius , inputImage, inputImage->GetRequestedRegion());
    itk::ImageRegionIterator<floatImageT>  dIt(outputImage, outputImage->GetRequestedRegion());



    float mean1, size1;
    float mean2, size2;
    float distance;
    float side;
    float weight;


    while(!it.IsAtEnd())
    {
        /* distance = mean1 = mean2 = size1 = size2 = 0.f;

         for(unsigned i=0; i< it.Size(); ++i)
         {

             if (it.GetPixel(i) < 0.f) //cells
             {
                 mean1 += it.GetPixel(i);
                 ++size1;
             }
             else
             {
                 mean2 += it.GetPixel(i);
                 ++size2;
             }


             //distance += std::exp(-std::abs(it.GetCenterPixel() - it.GetPixel(i)));
         }

         size1 /= it.Size();
         size2 /= it.Size();

         side = (size1 > size2)? -1 : 1;
         weight = (size1 > size2)? size1 : size2;

         dIt.Set(it.GetCenterPixel() + (( side - it.GetCenterPixel()) * weight) );
*/


        dIt.Set( (it.GetCenterPixel() < 0)? it.GetCenterPixel() : 1);


        ++it;
        ++dIt;
    }


    if(show)
    {

        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(outputImage);
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(255);
        rescaleFilter->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"distance map");
    }


    return outputImage;


}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::LabelRoughly()
{

    using ImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<floatImageT>;

    ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
    imageCalculatorFilter->SetImage(BImage);
    imageCalculatorFilter->Compute();


    float min = imageCalculatorFilter->GetMinimum();
    float max = imageCalculatorFilter->GetMaximum();

    float blockSize = (max - min)/2.f;

    float threshold = min + blockSize;


    labelMap =  grayImageT::New();
    labelMap->SetRegions(BImage->GetRequestedRegion());
    labelMap->Allocate();

    itk::ImageRegionIterator<floatImageT> BIt(BImage, BImage->GetRequestedRegion());
    itk::ImageRegionIterator<grayImageT> labelIt(labelMap, labelMap->GetRequestedRegion());


    while(!BIt.IsAtEnd())
    {

        labelIt.Set( (BIt.Get() <= threshold)? 0 : 255 );

        BIt.Set((BIt.Get() <= threshold)? BIt.Get() : 0);

        ++BIt;
        ++labelIt;

    }

    //VTKViewer::visualize<grayImageT>(labelMap ,"Label Map");

    //visualizing


    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
    rescaleFilter2->SetInput(BImage);
    rescaleFilter2->SetOutputMinimum(0);
    rescaleFilter2->SetOutputMaximum(255);
    rescaleFilter2->Update();

    VTKViewer::visualize<grayImageT>(rescaleFilter2->GetOutput() ,"Labels");




}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>:: GaussianBlur(bool show)
{

    using smoothFilterT = itk::SmoothingRecursiveGaussianImageFilter<floatImageT, floatImageT>;
    smoothFilterT::Pointer smoothFilter = smoothFilterT::New();
    smoothFilter->SetNormalizeAcrossScale(false);
    smoothFilter->SetInput(BImage);
    smoothFilter->SetSigma(5);
    smoothFilter->Update();

    blurImage = smoothFilter->GetOutput();



    if(show)
    {

        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(blurImage);
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(255);
        rescaleFilter->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"distance map");
    }


}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::edgeDetection(bool show)
{

    using CannyFilterType = itk::CannyEdgeDetectionImageFilter<floatImageT, floatImageT>;
    CannyFilterType::Pointer cannyFilter = CannyFilterType::New();
    cannyFilter->SetInput(BImage);
    cannyFilter->SetVariance(20);
    //cannyFilter->SetUpperThreshold(upperThreshold);
    //cannyFilter->SetLowerThreshold(lowerThreshold);
    cannyFilter->Update();

    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(cannyFilter->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();
    edges = rescaleFilter->GetOutput();

    if(show)
    {

        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"Canny");
    }



}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::computeDistanceDifferences(bool show)
{



    diffMap = floatImageT::New();
    diffMap->SetRegions(edges->GetRequestedRegion());
    diffMap->Allocate();


    using edgesIterator = itk::ImageRegionConstIteratorWithIndex<grayImageT>;
    edgesIterator itEdges(edges,   edges->GetRequestedRegion());

    grayImageT::RegionType regionAux;

    grayImageT::IndexType indexAux;

    auto upperIndex = edges->GetRequestedRegion().GetUpperIndex();

    const auto euclideanDistance =  Math::euclideanDistance<grayImageT::IndexType, 2>;

    float dRows, dCols; //distance to

    for( ;!itEdges.IsAtEnd(); ++itEdges)
    {
        if(itEdges.Get()==255)
        {
            diffMap->SetPixel(itEdges.GetIndex(), 255);
            continue;
        }

        //rows direction
        indexAux = itEdges.GetIndex();
        regionAux.SetIndex(itEdges.GetIndex());

        indexAux[1] = upperIndex[1];
        regionAux.SetUpperIndex(indexAux);

        edgesIterator itRows(edges, regionAux);

        while(!itRows.IsAtEnd() )
        {
            if(itRows.Get() == 255)
            {
                indexAux = itRows.GetIndex();
                break;
            }
            ++itRows;

        }

        dRows = euclideanDistance(indexAux, itEdges.GetIndex());

        //cols direction
        indexAux = itEdges.GetIndex();
        regionAux.SetIndex(itEdges.GetIndex());

        indexAux[0] = upperIndex[0];
        regionAux.SetUpperIndex(indexAux);

        edgesIterator itCols(edges, regionAux);

        while(!itCols.IsAtEnd() )
        {
            if(itCols.Get() == 255)
            {
                indexAux = itCols.GetIndex();
                break;
            }
            ++itCols;

        }

        dCols = euclideanDistance(indexAux, itEdges.GetIndex());


        //std::cout<<dCols<<std::endl;
        //tigstd::cout<<std::abs(dCols - dRows)<<std::endl;
        //float diff = (dCols - dRows)/dCols;
        diffMap->SetPixel(itEdges.GetIndex(), (std::abs(dCols-dRows)< 2)?0:255);



    }

    if(show)
    {
        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(diffMap);
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(255);
        rescaleFilter->Update();
        edges = rescaleFilter->GetOutput();


        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"Diff");
    }




}




template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCells()
{

    CreateImageB();
    //GaussianBlur(true);
    edgeDetection();
    computeDistanceDifferences(true);


    //LabelRoughly();
    //auto map = computeDistances(BImage, true);
    //map = computeDistances(map, true);
    //map = computeDistances(map, true);
    //map = computeDistances(map, true);
    //map = computeDistances(map, true);
    // map = computeDistances(map, true);


}


template<typename imageT>
void CellSegmentator<imageT>::superPixels()
{



}





