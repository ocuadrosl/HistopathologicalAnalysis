#include "pleuradetector.h"

template<typename InputImageT>
PleuraDetector<InputImageT>::PleuraDetector()
{

}


template<typename InputImageT>
void PleuraDetector<InputImageT>::SetInputImage(RGBImageP inputImage)
{

    this->InputImage = inputImage;

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
    filter->SetVariance(1);
    filter->SetLowerThreshold(0);
    filter->SetUpperThreshold(5);

    using RescaleType = itk::RescaleIntensityImageFilter<GrayImageFloatT, GrayImageT>;
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(filter->GetOutput());
    rescaler->SetOutputMinimum(Background);
    rescaler->SetOutputMaximum(Foreground);
    rescaler->Update();

    if(show)
    {
        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput() ,"Canny");

    }

    return rescaler->GetOutput();


}

template<typename InputImageT>
typename PleuraDetector<InputImageT>::LabelMapP PleuraDetector<InputImageT>::ConnectedComponets(GrayImageP edgesImage, unsigned threhold, bool show)
{


    using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter<GrayImageT, GrayImageT>;
    ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New();
    connected->SetInput(edgesImage);
    connected->FullyConnectedOn();
    connected->SetBackgroundValue(Background); //black
    connected->Update();

    using LabelImageToLabelMapFilterType =  itk::LabelImageToShapeLabelMapFilter<GrayImageT, LabelMapType>;
    typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
    labelImageToLabelMapFilter->SetInput(connected->GetOutput());
    labelImageToLabelMapFilter->Update();
    auto labelMap = labelImageToLabelMapFilter->GetOutput();

    //Remove label objects whose size is less than a threshold
    //performed in two phases according the itk doc
    if(threhold > 0)
    {
        std::vector<unsigned> labelsToRemove;
        for(unsigned i=1; i < labelMap->GetNumberOfLabelObjects(); ++i) //it starts in 1 because 0 is the background label
        {

            if(labelMap->GetLabelObject(i)->Size() <  threhold)
            {
                labelsToRemove.push_back(i);
            }

        }

        for(unsigned i : labelsToRemove)
        {
            labelMap->RemoveLabel(i);
        }
    }



    if(show)
    {

        //Label-map to RGB image
        using  rgbImageT =  itk::Image<itk::RGBPixel<unsigned char>, 2>;
        typedef itk::LabelMapToRGBImageFilter<LabelMapType, rgbImageT> RGBFilterType;
        typename RGBFilterType::Pointer labelMapToRGBFilter = RGBFilterType::New();
        labelMapToRGBFilter->SetInput(labelMap);
        labelMapToRGBFilter->Update();


        using rgbToGrayFilterT = itk::RGBToLuminanceImageFilter<rgbImageT, GrayImageT>;
        rgbToGrayFilterT::Pointer rgbToGrayFilter = rgbToGrayFilterT::New();
        rgbToGrayFilter->SetInput(labelMapToRGBFilter->GetOutput());
        rgbToGrayFilter->Update();


        //VTKViewer::visualize<GrayImageT>(rgbToGrayFilter->GetOutput(), "Connected components gray-scale");

        VTKViewer::visualize<rgbImageT>(labelMapToRGBFilter->GetOutput(), "Connected components RGB");

        //delete it
        io::WriteImage<rgbImageT>(labelMapToRGBFilter->GetOutput(), "/home/oscar/components-rgb.tiff");
        io::WriteImage<GrayImageT>(rgbToGrayFilter->GetOutput(), "/home/oscar/components-gray.tiff");


    }

    io::printOK("Connected components");


    return labelMap;

}



template<typename InputImageT>
typename PleuraDetector<InputImageT>::FloatImageP
PleuraDetector<InputImageT>::ComputeFractalDimension(LabelMapP components, float threshold, bool show)
{



    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(InputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->Allocate(Background);
    outputImage->FillBuffer(0.f);


    //using FilterType = itk::RegionOfInterestImageFilter<GrayImageT, GrayImageT>;



    for (unsigned i=0; i < components->GetNumberOfLabelObjects(); ++i)
    {

        auto labelObject = components->GetNthLabelObject(i);
        auto boundigBox = labelObject->GetBoundingBox();

        auto roiImage = GrayImageT::New();
        roiImage->SetRegions(boundigBox);
        roiImage->Allocate();
        roiImage->FillBuffer(Background);

        for(unsigned i = 0; i < labelObject->Size(); ++i)
        {
            roiImage->SetPixel(labelObject->GetIndex(i), Foreground);
        }

        auto fractalDimensionFilter = std::make_unique<FractalDimensionFilter<GrayImageT>>();
        fractalDimensionFilter->SetInputImage(roiImage);
        fractalDimensionFilter->PrintWarningsOff();
        fractalDimensionFilter->Compute();
        float dimension = fractalDimensionFilter->GetDimension();
        // std::cout<<dimension<<std::endl;

        if(dimension < threshold)
        {
            for(unsigned i = 0; i < labelObject->Size(); ++i)
            {

                outputImage->SetPixel(labelObject->GetIndex(i), dimension);
            }
        }


    }

    if(show)
    {

        using RescaleType = itk::RescaleIntensityImageFilter<FloatImageT, GrayImageT>;
        RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(outputImage);
        rescaler->SetOutputMinimum(Background);
        rescaler->SetOutputMaximum(255);
        rescaler->Update();

        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput(), "Fractal dimension");
    }



    io::printOK("Fractal dimension");

    return outputImage;
}






template<typename InputImageT>
typename PleuraDetector<InputImageT>::FloatImageP
PleuraDetector<InputImageT>::ComputeRoundness(LabelMapP components, float threshold, bool show)
{

    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(InputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->Allocate(Background);
    outputImage->FillBuffer(0.f);

    for (unsigned i=0; i < components->GetNumberOfLabelObjects(); ++i)
    {

        auto labelObject = components->GetNthLabelObject(i);

        float roundness = static_cast<float>(labelObject->GetRoundness());

        if(roundness < threshold)
        {
            for(unsigned i = 0; i < labelObject->Size(); ++i)
            {

                outputImage->SetPixel(labelObject->GetIndex(i), roundness);
            }
        }


    }

    if(show)
    {

        using RescaleType = itk::RescaleIntensityImageFilter<FloatImageT, GrayImageT>;
        RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(outputImage);
        rescaler->SetOutputMinimum(Background);
        rescaler->SetOutputMaximum(255);
        rescaler->Update();

        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput(), "Roundness");
    }



    io::printOK("Roundness");

    return outputImage;



}


template<typename InputImageT>
typename PleuraDetector<InputImageT>::GrayImageP
PleuraDetector<InputImageT>::HistogramEqualization(GrayImageP grayImage, float alpha, float beta, unsigned radiusSize, bool show)
{

    using AdaptiveHistogramEqualizationImageFilterType = itk::AdaptiveHistogramEqualizationImageFilter<GrayImageT>;
    AdaptiveHistogramEqualizationImageFilterType::Pointer adaptiveHistogramEqualizationImageFilter = AdaptiveHistogramEqualizationImageFilterType::New();

    adaptiveHistogramEqualizationImageFilter->SetAlpha(alpha);
    adaptiveHistogramEqualizationImageFilter->SetBeta(beta);

    AdaptiveHistogramEqualizationImageFilterType::ImageSizeType radius;
    radius.Fill(radiusSize);
    adaptiveHistogramEqualizationImageFilter->SetRadius(radius);

    adaptiveHistogramEqualizationImageFilter->SetInput(grayImage);

    adaptiveHistogramEqualizationImageFilter->Update();



   /* using RescaleType = itk::RescaleIntensityImageFilter<GrayImageT, GrayImageT>;
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(adaptiveHistogramEqualizationImageFilter->GetOutput());
    rescaler->SetOutputMinimum(Background);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();
*/

    auto outputImage = adaptiveHistogramEqualizationImageFilter->GetOutput();


    if(show)
    {


        VTKViewer::visualize<GrayImageT>(outputImage, "Adaptive Histogram Equalization");
    }



    io::printOK("Adaptive Histogram Equalization");



    return outputImage;

}

template<typename RGBImageT>
typename PleuraDetector<RGBImageT>::RGBImageP
PleuraDetector<RGBImageT>::RemoveBackground(float lThreshold, float aThreshold, float bThresold, bool show)
{


    //creating output image,
    auto outputImage = RGBImageT::New();
    outputImage->SetRegions(InputImage->GetRequestedRegion());
    outputImage->Allocate();


    //RGB to Lab
    using labImageT = itk::Image<itk::RGBPixel<float>, 2>;

    //RGB to XYZ
    using rgbToXyzFilterT = ColorConverterFilter<RGBImageT, labImageT>;
    auto rgbToXyzFilter = std::make_unique<rgbToXyzFilterT>();
    rgbToXyzFilter->setInput(InputImage);
    rgbToXyzFilter->rgbToXyz();


    //XYZ to LAB
    using xyzToLabFilterT = ColorConverterFilter<labImageT, labImageT>;
    auto xyzToLabFilter = std::make_unique<xyzToLabFilterT>();
    xyzToLabFilter->setInput(rgbToXyzFilter->getOutput());
    xyzToLabFilter->xyzToLab();

    auto labImage = xyzToLabFilter->getOutput();

    //computing max luminance value
    //Extracting B channel
    using ExtractChannelFilterT = ExtractChannelFilter<labImageT, FloatImageT>;
    std::unique_ptr<ExtractChannelFilterT> extractChannelFilter(new ExtractChannelFilterT());

    extractChannelFilter->setImputImage(xyzToLabFilter->getOutput());
    extractChannelFilter->extractChannel(0);

    auto lChannel = extractChannelFilter->getOutputImage();
    using ImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<FloatImageT>;

    typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
    imageCalculatorFilter->SetImage(lChannel);
    imageCalculatorFilter->Compute();
    float maxLuminance = imageCalculatorFilter->GetMaximum();
    float minLuminance = imageCalculatorFilter->GetMinimum();


    //Min max interpolation lambda
    auto minMax = [minLuminance, maxLuminance](float value)
    {
        return  ( 100.f * ( value - minLuminance) ) / (maxLuminance - minLuminance) ;
    };


    itk::ImageRegionConstIterator<RGBImageT> inputIt(InputImage, InputImage->GetRequestedRegion());
    itk::ImageRegionIterator<RGBImageT> outputIt(outputImage, outputImage->GetRequestedRegion());
    itk::ImageRegionIterator<labImageT> labIt(labImage, labImage->GetRequestedRegion());

    typename RGBImageT::PixelType white;
    white.Fill(255);

    for(; !inputIt.IsAtEnd(); ++inputIt, ++labIt, ++outputIt)
    {

        auto labPixel = labIt.Get();

        if(minMax(labPixel[0]) > lThreshold && labPixel[1] < aThreshold &&  labPixel[2] < bThresold)
        {
            outputIt.Set(white);
        }
        else
        {
            outputIt.Set(inputIt.Get());
        }

    }

    if(show)
    {


        VTKViewer::visualize<RGBImageT>(outputImage, "Remove Background");
    }


    return outputImage;


}



template<typename RGBImageT>
typename PleuraDetector<RGBImageT>::FloatImageP
PleuraDetector<RGBImageT>::RayFeatures(GrayImageP grayImage, GrayImageP edges, unsigned raysSize, bool show)
{


    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(grayImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->FillBuffer(0.f);


    //Compute offsets
    using offsetT = itk::Offset<2>;

    std::vector<std::vector<offsetT>> rayOffsets(raysSize, std::vector<offsetT>(3, {{0, 0}}));

    offsetT east      = {{1 , 0}};
    offsetT north     = {{0 , 1}};
    offsetT southWest = {{-1 , -1}};

    for (auto it = rayOffsets.begin()+1; it!= rayOffsets.end(); ++it)
    {
        (*it)[0]  = (*(it-1))[0] + east;
        (*it)[1]  = (*(it-1))[1] + north;
        (*it)[2]  = (*(it-1))[2] + southWest;

        std::cout<<(*it)[2]<<std::endl;
    }


    std::cout<<"here"<<std::endl;

    const unsigned edgeValue = 0;

    GrayImageT::SizeType radius = {{raysSize, raysSize}};

    itk::ImageRegionConstIterator<GrayImageT> eIt(edges, edges->GetRequestedRegion()); //edges it
    itk::ImageRegionIterator<FloatImageT>     oIt(outputImage, outputImage->GetRequestedRegion()); //output it
    itk::NeighborhoodIterator<GrayImageT>     gIt(radius, edges, grayImage->GetRequestedRegion()); //gray it


    for ( ;!eIt.IsAtEnd(); ++eIt, ++gIt, ++oIt)
    {
        auto pixel = eIt.Get();
        itk::Offset<2> centerOffset = {{0, 0}};


        oIt.Set(gIt.GetPixel(centerOffset));

        if(pixel == edgeValue)
        {



        }




    }


    if(show)
    {


        VTKViewer::visualize<FloatImageT>(outputImage, "Ray features");
    }


    return outputImage;

}



template<typename RGBImageT>
typename PleuraDetector<RGBImageT>::GrayImageP
PleuraDetector<RGBImageT>::GrayToBinary(GrayImageP grayImage, bool show)
{


    auto binaryImage = GrayImageT::New();
    binaryImage->SetRegions(grayImage->GetRequestedRegion());
    binaryImage->Allocate();


    itk::ImageRegionConstIterator<GrayImageT> gIt(grayImage, grayImage->GetRequestedRegion()); //edges it
    itk::ImageRegionIterator<GrayImageT>      bIt(binaryImage, binaryImage->GetRequestedRegion()); //output it


    for(; !gIt.IsAtEnd(); ++gIt, ++bIt)
    {

        bIt.Set( (gIt.Get() == 255 )? 0 : 255 );

    }


    if(show)
    {

        VTKViewer::visualize<GrayImageT>(binaryImage, "Gray to Binary");
    }


    return binaryImage;


}


template<typename InputImageT>
typename PleuraDetector<InputImageT>::FeatureVector*
PleuraDetector<InputImageT>::ComputeTexture(GrayImageP grayImage, GrayImageP edges , unsigned radius)
{





    dlib::array2d<dlib::rgb_pixel> dlibGrayImage;
    ifc::ITKToDlib<typename InputImageT::PixelType, dlib::rgb_pixel>(InputImage, dlibGrayImage);


   // dlib::array2d<unsigned char> lbpImage;
   // dlib::make_uniform_lbp_image(dlibGrayImage, lbpImage);


    dlib::image_window my_window(dlibGrayImage, "LBP");

    my_window.wait_until_closed();



    typename InputImageT::Pointer itkImage;
    ifc::DlibToITK<dlib::rgb_pixel, typename InputImageT::PixelType>(dlibGrayImage, itkImage);
    VTKViewer::visualize<InputImageT>(itkImage, "Gray to Binary");






}

template<typename InputImageT>
void PleuraDetector<InputImageT>::Detect()
{

    auto rgbImage = RemoveBackground(90, 5,5, false);




    //rgb to gray
    using rgbToGrayFilterType = itk::RGBToLuminanceImageFilter<InputImageT, GrayImageT>;
    auto rgbToGrayFilter = rgbToGrayFilterType::New();
    rgbToGrayFilter->SetInput(rgbImage);
    rgbToGrayFilter->Update();
    auto grayImage = rgbToGrayFilter->GetOutput();

    auto eqGrayImage = HistogramEqualization(grayImage, 1, 1, 5, false);

     io::WriteImage<GrayImageT>(eqGrayImage, "/home/oscar/gray.png");


    //auto binaryImage = GrayToBinary(eqGrayImage, true);


    auto edges       = EdgeDetectionCanny(eqGrayImage, false);
    //auto components  = ConnectedComponets(edges, 50,  true);

    auto textureVector = ComputeTexture(grayImage, edges,  5);



    //RayFeatures(eqGrayImage, edges, 5, true);

    //auto fractalDim = ComputeFractalDimension(components, 1.1 ,true);

    //ComputeRoundness(components, 0.15,  true);


    io::printOK("Detecting pleura");

}
