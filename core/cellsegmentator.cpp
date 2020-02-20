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

    bChannel = extractChannelFilter->getOutputImage();

    /*
    using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, floatImageT>;
    rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(extractChannelFilter->getOutputImage());
    rescaleFilter->SetOutputMinimum(-1.f);
    rescaleFilter->SetOutputMaximum(1.f);
    rescaleFilter->Update();
    bChannel = rescaleFilter->GetOutput();
*/




    //visualizing
    if(show)
    {

        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
        rescaleFilter2->SetInput(bChannel);
        rescaleFilter2->SetOutputMinimum(0);
        rescaleFilter2->SetOutputMaximum(255);
        rescaleFilter2->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter2->GetOutput() ,"b channel Image");
    }


}



template<typename rgbImageT>
void CellSegmentator<rgbImageT>:: GaussianBlur(bool show)
{

    using smoothFilterT = itk::SmoothingRecursiveGaussianImageFilter<floatImageT, floatImageT>;
    smoothFilterT::Pointer smoothFilter = smoothFilterT::New();
    smoothFilter->SetNormalizeAcrossScale(false);
    smoothFilter->SetInput(bChannel);
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
void CellSegmentator<rgbImageT>::DetectEdges(bool show)
{

    using CannyFilterType = itk::CannyEdgeDetectionImageFilter<floatImageT, floatImageT>;
    CannyFilterType::Pointer cannyFilter = CannyFilterType::New();
    cannyFilter->SetInput(bChannel);
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
void CellSegmentator<rgbImageT>::ComputeRayFetures(bool show)
{

    //defs

    differenceFeatures = floatImageT::New();
    differenceFeatures->SetRegions(edges->GetRequestedRegion());
    differenceFeatures->Allocate();
    differenceFeatures->FillBuffer(0);

    orientationFeatures = floatImageT::New();
    orientationFeatures->SetRegions(edges->GetRequestedRegion());
    orientationFeatures->Allocate();
    orientationFeatures->FillBuffer(0);


    const auto euclideanDistance =  Math::euclideanDistance<grayImageT::IndexType, 2, float>;

    using VectorType = itk::CovariantVector<float, 2>;
    VectorType cosSin;



    auto imgSize = edges->GetRequestedRegion().GetSize();
    using indexT = floatImageT::IndexType;

    const float pi = 3.14f;
    const float angleStep = 90;

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
            features += std::to_string(bChannel->GetPixel(index))+",";

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

                            orientationFeatures->SetPixel(index, orientation);

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
            diff = std::abs(dFirst - dSecond);

            //diff = dFirst;
            features += std::to_string(dFirst);


            differenceFeatures->SetPixel(index, dFirst);
            //diffMap->SetPixel(secondEdge, 255);


        }

    }



    IO::printOK("Compute ray features");

    if(show)
    {
        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(differenceFeatures);
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
    gradientFilter->SetInput(bChannel);
    gradientFilter->Update();

    gradient = gradientFilter->GetOutput();

    IO::printOK("Compute gradient");

}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::ComputeSuperPixels(bool show )
{

/*

    //rescale to 0 - 1
    using rescaleFilterType = itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
    rescaleFilterType::Pointer rescaleFilter = rescaleFilterType::New();
    rescaleFilter->SetInput(bChannel);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();



    using FilterType = itk::CastImageFilter<grayImageT, rgbImageT>;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(rescaleFilter->GetOutput());
    filter->Update();

*/


    using superPixelsT =  SuperPixels<rgbImageT>;

    //std::unique_ptr<superPixelsT> superPixels(new superPixelsT());
    auto superPixels = std::make_unique<superPixelsT>();


    superPixels->setImage(inputImage); //input image
    superPixels->create();


    superPixelsLabels = superPixels->getLabelImage();
    superPixelsNumber = superPixels->GetNumberOfSuperPixels();

    IO::printOK("Creating Super Pixels");

    if(show)
    {
        superPixels->show();
    }


}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::ComputeFeaturesVector(bool show)
{

    //allocate features vector
    featuresVector = featuresVectorT(superPixelsNumber, std::vector<float>(3, 0.f));

    featuresVectorT &fv = featuresVector; //alias

    itk::ImageRegionConstIterator<grayImageT> spIt(superPixelsLabels, superPixelsLabels->GetRequestedRegion());

    itk::ImageRegionConstIterator<floatImageT> bIt(bChannel, bChannel->GetRequestedRegion());
    itk::ImageRegionConstIterator<floatImageT> oriIt(orientationFeatures, orientationFeatures->GetRequestedRegion());
    itk::ImageRegionConstIterator<floatImageT> diffIt(differenceFeatures, differenceFeatures->GetRequestedRegion());

    unsigned label=0;
    auto index = spIt.GetIndex();

    std::vector<unsigned>  labelCounter(superPixelsNumber, 0);

    for(;!spIt.IsAtEnd(); ++spIt, ++bIt, ++oriIt, ++diffIt)
    {
        index = spIt.GetIndex();
        label = spIt.Get();
        fv[label][0] += bIt.Get();
        fv[label][1] += oriIt.Get();
        fv[label][2] += diffIt.Get();

        ++labelCounter[label];

    }


    for(unsigned i=0; i< superPixelsNumber; ++i)
    {

        fv[i][0] /= labelCounter[i];
        fv[i][1] /= labelCounter[i];
        fv[i][2] /= labelCounter[i];

    }


    auto testImage = floatImageT::New();
    testImage->SetRegions(edges->GetRequestedRegion());
    testImage->Allocate();
    testImage->FillBuffer(0);


    itk::ImageRegionIterator<floatImageT> it(testImage, testImage->GetRequestedRegion());


    for( spIt.GoToBegin(); !it.IsAtEnd(); ++it, ++spIt)
    {
        it.Set(fv[spIt.Get()][0]);
    }


    if(show)
    {

        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
        rescaleFilter->SetInput(testImage);
        rescaleFilter->SetOutputMinimum(0);
        rescaleFilter->SetOutputMaximum(255);
        rescaleFilter->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"test");



        overlay(rescaleFilter->GetOutput());
    }




    IO::printOK("Compute Features Vector");

}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::ReadWekaFile(const std::string& fileName, const std::string& imageName)
{
    std::ifstream wekaFile(fileName);
    std::string line;

    //reading header
    for(unsigned i=1; i<=7; ++i)
    {
        getline (wekaFile,line);
    }

    std::vector<unsigned> clusters;
    while (getline (wekaFile,line))
    {
        unsigned cluster = static_cast<unsigned>(line.back())-48;

        clusters.push_back((cluster==1)? 0 : 255 );
        //std::cout<<cluster<<std::endl;
    }

    /*for(unsigned i=0; i<clusters.size();  ++i)
    {
        //tIt.Set(clusters[spIt.Get()]);
        std::cout<<clusters[i]<<std::endl;
    }*/


    wekaFile.close();


    auto resultImage = grayImageT::New();
    resultImage->SetRegions(superPixelsLabels->GetRequestedRegion());
    resultImage->Allocate();
    resultImage->FillBuffer(0);


    itk::ImageRegionIterator<grayImageT> tIt(resultImage, resultImage->GetRequestedRegion());

    itk::ImageRegionConstIterator<grayImageT> spIt(superPixelsLabels, superPixelsLabels->GetRequestedRegion());

    for(unsigned i=0; !tIt.IsAtEnd(); ++tIt, ++spIt, ++i)
    {
        tIt.Set(clusters[spIt.Get()]);

    }



    using rescaleFilterType2= itk::RescaleIntensityImageFilter<grayImageT, itk::Image<unsigned char,2>>;
    rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(resultImage);
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();


    using WriterType = itk::ImageFileWriter<itk::Image<unsigned char,2>>;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(imageName);
    writer->SetInput(rescaleFilter->GetOutput());
    writer->Update();


    VTKViewer::visualize<grayImageT>(resultImage ,"Result");

    overlay(resultImage);



    IO::printOK("Reading weka file");


}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::WriteFeaturesVector(const std::string& fileName)
{



    std::ofstream wekaFile;
    wekaFile.open(fileName);

    wekaFile<<"@RELATION dataset"<<std::endl;
    wekaFile<<"@ATTRIBUTE color NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE orien NUMERIC"<<std::endl;
    wekaFile<<"@ATTRIBUTE diffe NUMERIC"<<std::endl;
   // wekaFile<<"@ATTRIBUTE lbp NUMERIC"<<std::endl;
    //wekaFile<<"@ATTRIBUTE class {1,2}"<<std::endl;
    wekaFile<<"@DATA"<<std::endl;


    for(unsigned i=0;i<featuresVector.size();++i)
    {

        wekaFile<<featuresVector[i][0]<<","<<featuresVector[i][1]<<","<<featuresVector[i][2]<<std::endl;


    }
    wekaFile.close();

    IO::printOK("Writing Weka File");


}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCells()
{


    CreateImageB(true);
    DetectEdges(true);
    ComputeGradients();
    ComputeRayFetures();
    ComputeSuperPixels(true);
    ComputeFeaturesVector(true);
    WriteFeaturesVector("/home/oscar/test/weka_file.arff");




    ReadWekaFile("/home/oscar/test/em.arff",  "/home/oscar/test/result.png");


}




