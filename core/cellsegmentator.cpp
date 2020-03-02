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

    using smoothFilterT = itk::SmoothingRecursiveGaussianImageFilter<grayImageT, grayImageT>;
    smoothFilterT::Pointer smoothFilter = smoothFilterT::New();
    smoothFilter->SetNormalizeAcrossScale(false);
    smoothFilter->SetInput(resultGrayImage);
    smoothFilter->SetSigma(5);
    smoothFilter->Update();

    using rescaleFilterType2= itk::RescaleIntensityImageFilter<grayImageT, grayImageT>;
    rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
    rescaleFilter->SetInput(smoothFilter->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);
    rescaleFilter->Update();

    blurImage = rescaleFilter->GetOutput();


    itk::ImageRegionConstIterator<grayImageT> biIt(resultBinaryImage, resultBinaryImage->GetRequestedRegion());
    itk::ImageRegionIterator<grayImageT> blIt(blurImage, blurImage->GetRequestedRegion());

    for(;!biIt.IsAtEnd(); ++biIt, ++blIt)
    {
        if(biIt.Get()==0)
        {
            blIt.Set(255);

        }

    }


    IO::printOK("Gaussian blur");


    if(show)
    {

        VTKViewer::visualize<grayImageT>(blurImage ,"Gaussian Blur");
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
void CellSegmentator<rgbImageT>::Threshold(bool show)
{

    using AdaptiveOtsuFilter = AdaptiveOtsuFilter<floatImageT, grayImageT>;

    auto adaptive =  std::make_unique<AdaptiveOtsuFilter>();
    adaptive->SetInputImage(bChannel);

    adaptive->Compute();


    using FilterType = itk::MomentsThresholdImageFilter<floatImageT, grayImageT>;
    //using FilterType = itk::OtsuThresholdImageFilter<floatImageT, grayImageT>;


    //using FilterType = itk::TriangleThresholdImageFilter<floatImageT, grayImageT>;


    FilterType::Pointer thresholdFilter = FilterType::New();
    thresholdFilter->SetInput(bChannel);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->SetOutsideValue(255);
    thresholdFilter->SetNumberOfHistogramBins(400);
    thresholdFilter->Update(); // To compute threshold
    //binaryImage = thresholdFilter->GetOutput();

    binaryImage = adaptive->GetOutput();

    if(show)
    {
        VTKViewer::visualize<grayImageT>(binaryImage ,"Threshold");
    }



}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>:: binaryToSuperPixels(std::string fileName, bool show)
{



    itk::ImageRegionConstIterator<grayImageT> spIt(superPixelsLabels, superPixelsLabels->GetRequestedRegion());
    itk::ImageRegionConstIterator<grayImageT> bIt(binaryImage, binaryImage->GetRequestedRegion());


    std::vector<unsigned> pixelsCounter(superPixelsNumber,0);
    std::vector<unsigned> superPixelsSize(superPixelsNumber,0);

    for(; !spIt.IsAtEnd(); ++spIt, ++bIt)
    {
        pixelsCounter[  spIt.Get() ] += (bIt.Get()==0) ? 1 : 0;
        ++superPixelsSize[spIt.Get()];
    }

    resultBinaryImage = grayImageT::New();
    resultBinaryImage->SetRegions(superPixelsLabels->GetRequestedRegion());
    resultBinaryImage->Allocate();
    resultBinaryImage->FillBuffer(0);

    auto resultLabelImage = grayImageT::New();
    resultLabelImage->SetRegions(superPixelsLabels->GetRequestedRegion());
    resultLabelImage->Allocate();
    resultLabelImage->FillBuffer(0);


    using rgbCharImageT = itk::Image<itk::RGBPixel<unsigned char>,2>;
    auto resultRGBImage = rgbCharImageT::New();
    resultRGBImage->SetRegions(superPixelsLabels->GetRequestedRegion());
    resultRGBImage->Allocate();
    resultRGBImage->FillBuffer(255);


    itk::ImageRegionIterator<grayImageT> rbIt(resultBinaryImage, resultBinaryImage->GetRequestedRegion());
    itk::ImageRegionIterator<grayImageT> rlIt(resultLabelImage, resultLabelImage->GetRequestedRegion());
    itk::ImageRegionIterator<rgbCharImageT>  rRGBIt(resultRGBImage, resultRGBImage->GetRequestedRegion());
    itk::ImageRegionIterator<rgbImageT>  inIt(inputImage, inputImage->GetRequestedRegion());

    unsigned label=0;
    for(spIt.GoToBegin(); !spIt.IsAtEnd(); ++spIt, ++rbIt, ++inIt, ++rRGBIt, ++rlIt)
    {
        label = spIt.Get();
        if(pixelsCounter[label] > (superPixelsSize[label]*0.9))
        {
            rbIt.Set(255);
            rRGBIt.Set(inIt.Get());
            rlIt.Set(label+1);
        }

    }

    //compute gray from RGB, it is using by the gaussian filter
    using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter<rgbCharImageT, grayImageT>;
    typename  rgbToGrayFilterT::Pointer rgbToGrayF = rgbToGrayFilterT::New();
    rgbToGrayF->SetInput(resultRGBImage);
    rgbToGrayF->Update();
    resultGrayImage = rgbToGrayF->GetOutput();


    //creating gray image from rgb result

    using WriterRGBType = itk::ImageFileWriter<rgbCharImageT>;
    WriterRGBType::Pointer writerRGB = WriterRGBType::New();
    writerRGB->SetFileName(fileName+"_RGB.png");
    writerRGB->SetInput(resultRGBImage);
    writerRGB->Update();



    using charImageT = itk::Image<unsigned char,2>;

    using CastFilterType = itk::CastImageFilter<grayImageT, charImageT>;
    CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(resultBinaryImage);

    using WriterType = itk::ImageFileWriter<charImageT>;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(fileName+"_binary.png");
    writer->SetInput(castFilter->GetOutput());
    writer->Update();


    castFilter = CastFilterType::New();
    castFilter->SetInput(resultLabelImage);
    castFilter->Update();

    writer = WriterType::New();
    writer->SetFileName(fileName+"_label.png");
    writer->SetInput(castFilter->GetOutput());
    writer->Update();






    if(show)
    {




        VTKViewer::visualize<grayImageT>(resultBinaryImage ,"Result");
        VTKViewer::visualize<rgbCharImageT>(resultRGBImage ,"Result RGB");
        VTKViewer::visualize<grayImageT>(resultGrayImage ,"Result gray");

    }





    IO::printOK("Binary to Super Pixels");

}



template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findLocalMinima(bool show)
{

    using RegionalMinimaImageFilter = itk::RegionalMinimaImageFilter<grayImageT, grayImageT>;

    RegionalMinimaImageFilter::Pointer filter = RegionalMinimaImageFilter::New();

    filter->SetFlatIsMinima(true);
    filter->FullyConnectedOff();
    filter->SetInput(blurImage);

    IO::printOK("Finding Local minima");

    if(show)
    {
        VTKViewer::visualize<grayImageT>(filter->GetOutput() ,"Local Maxima");
    }

}


template<typename rgbImageT>
void CellSegmentator<rgbImageT>::setNames(const std::string& dirPath, const std::string& fileName)
{
    imageName = fileName;
    this->dirPath = dirPath;
}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCells()
{

    IO::printWait("Image: "+imageName);

    CreateImageB();
    Threshold();
    ComputeSuperPixels();
    binaryToSuperPixels(dirPath+"/"+imageName+"_result") ;
    GaussianBlur();
    findLocalMinima();

    IO::printOK("Find Cells");
    std::cout<<std::endl;

}




