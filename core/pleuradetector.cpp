#include "pleuradetector.h"

template<typename InputImageT>
PleuraDetector<InputImageT>::PleuraDetector()
{

}


template<typename InputImageT>
void PleuraDetector<InputImageT>::SetInputImage(ImageP inputImage)
{

    this->inputImage = inputImage;

}

template<typename InputImageT>
typename PleuraDetector<InputImageT>::GrayImageP
PleuraDetector<InputImageT>::FillHoles(GrayImageP grayImage, bool show)
{

    using FilterType = itk::VotingBinaryIterativeHoleFillingImageFilter<GrayImageT>;
    FilterType::InputSizeType radius;
    radius.Fill(50);

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(grayImage);
    filter->SetRadius(radius);
    filter->SetMajorityThreshold(5);
    filter->SetBackgroundValue(itk::NumericTraits<GrayImageT::PixelType>::Zero);
    filter->SetForegroundValue(itk::NumericTraits<GrayImageT::PixelType>::max());
    filter->SetMaximumNumberOfIterations(100);
    filter->Update();

    if(show)
    {

        VTKViewer::visualize<GrayImageT>(filter->GetOutput());
    }


    IO::printOK("Filling holes");

    return filter->GetOutput();

}


template<typename InputImageT>
void PleuraDetector<InputImageT>::GeodesicActiveCountour(GrayImageP grayImage, bool show)
{


    using GrayImageFloatT = itk::Image<float,2>;

    using RescaleType = itk::RescaleIntensityImageFilter<GrayImageT, GrayImageFloatT>;
    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetInput(grayImage);
    rescale->SetOutputMinimum(0.f);
    rescale->SetOutputMaximum(1.f);


    using SmoothingFilterType = itk::CurvatureAnisotropicDiffusionImageFilter<GrayImageFloatT, GrayImageFloatT>;
    SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
    smoothing->SetTimeStep(0.125);
    smoothing->SetNumberOfIterations(5);
    smoothing->SetConductanceParameter(9.0);
    smoothing->SetInput(rescale->GetOutput());




    unsigned sigma = 5;
    using GradientFilterType = itk::GradientMagnitudeRecursiveGaussianImageFilter<GrayImageFloatT, GrayImageFloatT>;
    GradientFilterType::Pointer gradientMagnitude = GradientFilterType::New();
    gradientMagnitude->SetSigma(sigma);
    gradientMagnitude->SetInput(smoothing->GetOutput());




    double alpha = 50;
    double beta  = 500;
    using SigmoidFilterType = itk::SigmoidImageFilter<GrayImageFloatT, GrayImageFloatT>;
    SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
    sigmoid->SetOutputMinimum(0.0);
    sigmoid->SetOutputMaximum(1.0);
    sigmoid->SetAlpha(alpha);
    sigmoid->SetBeta(beta);
    sigmoid->SetInput(gradientMagnitude->GetOutput());

    sigmoid->Update();
    //std::cout<<"pass"<<std::endl;


    using FastMarchingFilterType = itk::FastMarchingImageFilter<GrayImageFloatT, GrayImageFloatT>;
    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();


    float propagationScaling = 1.f;
    unsigned numberOfIterations=5;

    using GeodesicActiveContourFilterType = itk::GeodesicActiveContourLevelSetImageFilter<GrayImageFloatT, GrayImageFloatT>;
    GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();
    geodesicActiveContour->SetPropagationScaling(propagationScaling);
    geodesicActiveContour->SetCurvatureScaling(1.0);
    geodesicActiveContour->SetAdvectionScaling(1.0);
    geodesicActiveContour->SetMaximumRMSError(0.02);
    geodesicActiveContour->SetNumberOfIterations(numberOfIterations);
    geodesicActiveContour->SetInput(fastMarching->GetOutput());
    geodesicActiveContour->SetFeatureImage(sigmoid->GetOutput());



    using ThresholdingFilterType = itk::BinaryThresholdImageFilter<GrayImageFloatT, GrayImageT>;
    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
    thresholder->SetLowerThreshold(-1000.0);
    thresholder->SetUpperThreshold(0.0);
    thresholder->SetOutsideValue(0);
    thresholder->SetInsideValue(255);
    thresholder->SetInput(geodesicActiveContour->GetOutput());

    using NodeContainer = FastMarchingFilterType::NodeContainer;
    using NodeType = FastMarchingFilterType::NodeType;

    GrayImageFloatT::IndexType seedPosition;
    seedPosition[0] = 200;
    seedPosition[1] = 200;

    NodeContainer::Pointer seeds = NodeContainer::New();
    NodeType               node;
    node.SetValue(0);
    node.SetIndex(seedPosition);

    seeds->Initialize();
    seeds->InsertElement(0, node);

    fastMarching->SetTrialPoints(seeds);
    fastMarching->SetSpeedConstant(1.0);

    if(show)
    {
        VTKViewer::visualize<GrayImageT>(thresholder->GetOutput() ,"Geodesic active countour");

    }





}


template<typename InputImageT>
typename PleuraDetector<InputImageT>::GrayImageP
PleuraDetector<InputImageT>::EdgeDetectionCanny(GrayImageP grayImage, bool show)
{

    using GrayImageFloatT = itk::Image<float,2>;

    using toFloatFilterT = itk::RescaleIntensityImageFilter<GrayImageT, GrayImageFloatT>;
    toFloatFilterT::Pointer toFloatFilter = toFloatFilterT::New();
    toFloatFilter->SetInput(grayImage);
    toFloatFilter->SetOutputMinimum(0);
    toFloatFilter->SetOutputMaximum(255);

    using FilterType = itk::CannyEdgeDetectionImageFilter<GrayImageFloatT, GrayImageFloatT>;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(toFloatFilter->GetOutput());
    filter->SetVariance(5);
    filter->SetLowerThreshold(0);
    filter->SetUpperThreshold(5);

    using RescaleType = itk::RescaleIntensityImageFilter<GrayImageFloatT, GrayImageT>;
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(filter->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    if(show)
    {
        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput() ,"Canny");

    }

    return rescaler->GetOutput();


}

template<typename InputImageT>
void PleuraDetector<InputImageT>::ConnectedComponets(GrayImageP edgesImage, bool show)
{


    using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter<GrayImageT, GrayImageT>;
    ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New();
    connected->SetInput(edgesImage);
    connected->FullyConnectedOn();
    connected->SetBackgroundValue(0); //black
    connected->Update();


    typedef itk::LabelImageToLabelMapFilter<GrayImageT> LabelImageToLabelMapFilterType;
    typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
    labelImageToLabelMapFilter->SetInput(connected->GetOutput());
    labelImageToLabelMapFilter->Update();


    using  rgbImageT =  itk::Image<itk::RGBPixel<unsigned char>, 2>;
    typedef itk::LabelToRGBImageFilter<GrayImageT, rgbImageT> RGBFilterType;
    typename RGBFilterType::Pointer rgbFilter = RGBFilterType::New();
    rgbFilter->SetInput(connected->GetOutput());
    rgbFilter->Update();

    if(show)
    {

        VTKViewer::visualize<rgbImageT>(rgbFilter->GetOutput(), "Connected components");

    }



}



template<typename InputImageT>
void PleuraDetector<InputImageT>::Detect()
{

    //rgb to gray
    using rgbToGrayFilterType = itk::RGBToLuminanceImageFilter<InputImageT, GrayImageT>;
    typename rgbToGrayFilterType::Pointer rgbToGrayFilter = rgbToGrayFilterType::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    auto grayImage = rgbToGrayFilter->GetOutput();
/*
    //gray to binary
    //using FilterType = itk::MomentsThresholdImageFilter<floatImageT, grayImageT>;
    using FilterType = itk::OtsuThresholdImageFilter<GrayImageT, GrayImageT>;


    //using FilterType = itk::TriangleThresholdImageFilter<floatImageT, grayImageT>;


    FilterType::Pointer thresholdFilter = FilterType::New();
    thresholdFilter->SetInput(grayImage);
    thresholdFilter->SetInsideValue(0);
    thresholdFilter->SetOutsideValue(255);
    thresholdFilter->SetNumberOfHistogramBins(400);
    thresholdFilter->Update(); // To compute threshold
    auto binaryImage = thresholdFilter->GetOutput();

*/


    //VTKViewer::visualize<GrayImageT>(grayImage);

    //GeodesicActiveCountour(grayImage, true);
    auto edges = EdgeDetectionCanny(grayImage, false);
    ConnectedComponets(edges, true);





}
