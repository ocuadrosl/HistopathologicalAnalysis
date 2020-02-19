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
    cannyFilter->SetVariance(10);
    //cannyFilter->SetUpperThreshold(100);
    //cannyFilter->SetLowerThreshold(0);
    //std::cout<<cannyFilter->GetLowerThreshold()

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

        VTKViewer::visualize<grayImageT>(edges ,"Canny");
    }



}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::computeRayFetures(bool show)
{

    //defs

    diffMap = floatImageT::New();
    diffMap->SetRegions(edges->GetRequestedRegion());
    diffMap->Allocate();
    diffMap->FillBuffer(0);

    const auto euclideanDistance =  Math::euclideanDistance<grayImageT::IndexType, 2, float>;

    using VectorType = itk::CovariantVector<float, 2>;
    VectorType cosSin;
    cosSin[0] = std::cos(90.f * 3.14f/180.f);
    cosSin[1] = std::sin(90.f * 3.14f/180.f);

    const float pi = 3.14f;
    const float angleStep = 30;

    ofstream wekaFile;
    wekaFile.open("/home/oscar/test/weka_file.arff");

    wekaFile<<"@RELATION dataset"<<std::endl;
    wekaFile<<"@ATTRIBUTE color NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE orien NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE diffe NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE class {1,2}"<<std::endl;
    wekaFile<<"@DATA"<<std::endl;


    auto imgSize = edges->GetRequestedRegion().GetSize();
    using indexT = floatImageT::IndexType;


    unsigned nearEdgesFlag=0;
    float radius=1.f;
    float dFirst, dSecond, orientation, diff; //distance to

    std::string features;
    for(unsigned row=0; row< imgSize[1]; ++row)
    {
        for(unsigned col=0; col< imgSize[0]; ++col)
        {

            indexT index = {{col, row}};
            features = "";
            features += std::to_string(BImage->GetPixel(index))+",";

            indexT indexAux, firstEdge, secondEdge;

            radius = 1.f;
            nearEdgesFlag=0;
            while(nearEdgesFlag < 2)
            {
                for(float angle=0.f; angle < 2.f*pi; angle += (2.f*pi/angleStep))
                {
                    indexAux[0] = index[0] + static_cast<int>(radius*std::cos(angle));
                    indexAux[1] = index[1] + static_cast<int>(radius*std::sin(angle));

                    if(index[0]==indexAux[0] && index[1]==indexAux[1] ){ continue; }

                    if(!edges->GetRequestedRegion().IsInside(indexAux)){ continue; }

                    if(edges->GetPixel(indexAux) == 255) //is edge?
                    {
                        if(nearEdgesFlag == 0) //first
                        {
                            firstEdge = indexAux;
                            dFirst =  euclideanDistance(index, firstEdge);


                            cosSin[0] = std::cos(angle);
                            cosSin[1] = std::sin(angle);
                            orientation  = (gradient->GetPixel(firstEdge)/gradient->GetPixel(firstEdge).GetNorm()) * cosSin;
                            features += std::to_string(orientation)+",";

                            ++nearEdgesFlag;
                            continue;
                        }
                        else if(nearEdgesFlag == 1 && (firstEdge[0]!= indexAux[0] || firstEdge[1]!= indexAux[1])) //second
                        {
                            secondEdge = indexAux;
                            dSecond =  euclideanDistance(index, secondEdge);
                            ++nearEdgesFlag;
                            continue;
                        }
                        else if(nearEdgesFlag >= 2)
                        {
                            break;
                        }

                    }

                }

                radius++;

            }

            //std::cout<<(dFirst-dSecond)/dFirst<<std::endl;
            //diff = (dFirst - dSecond)/dFirst;

            diff = std::abs(dFirst - dSecond);
            features += std::to_string(diff);

            wekaFile<<features<<std::endl;

            diffMap->SetPixel(index, diff);
            //diffMap->SetPixel(secondEdge, 255);


        }

    }
    wekaFile.close();


    IO::printOK("Compute ray features");

    if(show)
    {
        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(diffMap);
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(255);
        rescaleFilter->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"Diff");
    }





}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::computeDistanceDifferences(bool show)
{

    ofstream wekaFile;
    wekaFile.open("/home/oscar/test/weka_file.arff");

    wekaFile<<"@RELATION dataset"<<std::endl;
    wekaFile<<"@ATTRIBUTE color NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE orien NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE diffe NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE class {1,2}"<<std::endl;
    wekaFile<<"@DATA"<<std::endl;


    diffMap = floatImageT::New();
    diffMap->SetRegions(edges->GetRequestedRegion());
    diffMap->Allocate();


    using edgesIterator = itk::ImageRegionConstIteratorWithIndex<grayImageT>;
    edgesIterator itEdges(edges,   edges->GetRequestedRegion());

    grayImageT::RegionType regionAux;

    grayImageT::IndexType indexAux, upperIndexAux;

    auto upperIndex = edges->GetRequestedRegion().GetUpperIndex();

    const auto euclideanDistance =  Math::euclideanDistance<grayImageT::IndexType, 2, float>;

    using VectorType = itk::CovariantVector<float, 2>;


    VectorType cosSin;
    cosSin[0] = std::cos(90.f * 3.14f/180.f);
    cosSin[1] = std::sin(90.f * 3.14f/180.f);

    //nearestContourDir[0] /= static_cast<float>(nearestContourDir.GetNorm());
    //nearestContourDir[1] /= static_cast<float>(nearestContourDir.GetNorm());


    float dRows, dCols; //distance to

    std::string features = "";



    itk::ImageRegionConstIterator<floatImageT> imgIt(BImage, BImage->GetRequestedRegion());


    for( ;!itEdges.IsAtEnd(); ++itEdges, ++imgIt)
    {

        features = "";

        features += std::to_string(imgIt.Get())+",";

        //rows direction

        indexAux = upperIndexAux = itEdges.GetIndex();

        indexAux[1] = (indexAux[1] == upperIndex[1]) ? indexAux[1] : indexAux[1] + 1;

        upperIndexAux[1] = upperIndex[1];

        regionAux.SetIndex(indexAux);
        regionAux.SetUpperIndex(upperIndexAux);

        edgesIterator itRows(edges, regionAux);

        while(!itRows.IsAtEnd())
        {
            if(itRows.Get() == 255) //is edge?
            {
                indexAux = itRows.GetIndex();
                break;
            }
            ++itRows;

        }

        dRows = euclideanDistance(indexAux, itEdges.GetIndex());


        //Orientation using gradient in row direction
        float orientation  = (gradient->GetPixel(indexAux)/gradient->GetPixel(indexAux).GetNorm()) * cosSin;
        features += std::to_string(orientation)+",";


        //cols direction
        indexAux = upperIndexAux = itEdges.GetIndex();

        indexAux[0] = (indexAux[0] == upperIndex[0]) ? indexAux[0] : indexAux[0] + 1;

        upperIndexAux[0] = upperIndex[0];

        regionAux.SetIndex(indexAux);
        regionAux.SetUpperIndex(upperIndexAux);

        edgesIterator itCols(edges, regionAux);

        while(!itCols.IsAtEnd() )
        {
            if(itCols.Get() == 255) //is edge?
            {
                indexAux = itCols.GetIndex();
                break;
            }
            ++itCols;

        }

        dCols = euclideanDistance(indexAux, itEdges.GetIndex());

        //std::cout<<indexAux<<"->"<<itEdges.GetIndex()<<std::endl;
        //std::cout<<dCols<<std::endl;

        float diff = (dRows == 0.f) ? 0 : (dRows - dCols)/dRows;
        //std::cout<<diff<<std::endl;


        //features += std::to_string(diff)+","+std::to_string((std::rand()%2)+1);
        features += std::to_string(diff);

        std::cout<<features<<std::endl;

        diffMap->SetPixel(itEdges.GetIndex(), diff);

        wekaFile<<features<<std::endl;
        //std::cout<<count++<<std::endl;

    }

    wekaFile.close();

    IO::printOK("Fetures vector");

    if(show)
    {
        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(diffMap);
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(255);
        rescaleFilter->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"Diff");
    }




}






template<typename rgbImageT>
void CellSegmentator<rgbImageT>::overlay(grayImageP image)
{

    using FilterType = itk::CastImageFilter<grayImageT, rgbImageT>;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(image);
    filter->Update();

    using overlayFilterType = OverlayRGBImageFilter<rgbImageT>;
    std::unique_ptr<overlayFilterType> overlayFilter(new overlayFilterType());

    overlayFilter->setBackgroundImage(inputImage);
    overlayFilter->setForegroundImage(filter->GetOutput());
    overlayFilter->setForegroundAlpha(0.5);
    overlayFilter->overlay();



    VTKViewer::visualize<rgbImageT>(overlayFilter->getOutput() ,"Overlay");



}



template<typename imageT>
void CellSegmentator<imageT>::ComputeGradients()
{

    // Create and setup a gradient filter
    using GradientFilterType = itk::GradientImageFilter<floatImageT, float>;
    GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(BImage);
    gradientFilter->Update();

    gradient = gradientFilter->GetOutput();

    IO::printOK("Compute gradient");

}


template<typename imageT>
void CellSegmentator<imageT>::superPixels()
{



}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCells()
{

    CreateImageB(true);
    edgeDetection(true);

    ComputeGradients();
    //computeDistanceDifferences(true);
    computeRayFetures(true);


    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(diffMap);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();

    overlay(rescaleFilter->GetOutput());

}




