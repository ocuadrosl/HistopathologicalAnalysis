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
void PleuraDetector<InputImageT>::SetLabelImage(RGBImageP labelImage)
{

    this->LabelImage = labelImage;

}


template<typename InputImageT>
typename PleuraDetector<InputImageT>::GrayImageP
PleuraDetector<InputImageT>::EdgeDetectionCanny(GrayImageP grayImage, float variance, bool show)
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
    filter->SetVariance(variance);
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
typename PleuraDetector<InputImageT>::LabelMapP PleuraDetector<InputImageT>::ConnectedComponets(GrayImageP edgesImage, unsigned threhold, unsigned background, bool show)
{


    using ConnectedComponentImageFilterType = itk::ConnectedComponentImageFilter<GrayImageT, GrayImageT>;
    ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New();
    connected->SetInput(edgesImage);
    connected->FullyConnectedOn();
    connected->SetBackgroundValue(background); //black
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
        std::vector<ShapeLabelObjectType::Pointer> labelsToRemove;
        for(unsigned i=1; i < labelMap->GetNumberOfLabelObjects(); ++i) //it starts in 1 because 0 is the background label
        {



            if(labelMap->GetLabelObject(i)->Size() <  threhold)
            {
                labelsToRemove.push_back(labelMap->GetLabelObject(i));
                //labelsToRemove.push_back(i);
            }

        }

        for(auto i : labelsToRemove)
        {
            //labelMap->RemoveLabel(i);
            labelMap->RemoveLabelObject(i);
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
        //io::WriteImage<rgbImageT>(labelMapToRGBFilter->GetOutput(), "/home/oscar/components-rgb.tiff");
        //io::WriteImage<GrayImageT>(rgbToGrayFilter->GetOutput(), "/home/oscar/components-gray.tiff");


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
        rescaler->SetOutputMaximum(Foreground);
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
PleuraDetector<RGBImageT>::CleanBackground(float lThreshold, float aThreshold, float bThresold, bool show)
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
    //Extracting L channel
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
    white.Fill(Foreground);

    for(; !inputIt.IsAtEnd(); ++inputIt, ++labIt, ++outputIt)
    {

        auto labPixel = labIt.Get();

        if(minMax(labPixel[0]) > lThreshold  &&  labPixel[1] < aThreshold &&  labPixel[2] < bThresold)
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

        VTKViewer::visualize<RGBImageT>(outputImage, "Clean Background");
    }


    return outputImage;


}



template<typename RGBImageT>
typename PleuraDetector<RGBImageT>::FloatImageP
PleuraDetector<RGBImageT>::RayFeatures(GrayImageP edges, unsigned raysSize, bool show)
{


    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(edges->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->FillBuffer(0.f);


    //Compute offsets
    using offsetT = itk::Offset<2>;

    std::vector<std::vector<offsetT>> rayOffsets(raysSize, std::vector<offsetT>(3, {0, 0}));

    offsetT east      = {1 , 0};
    offsetT north     = {0 , 1};
    offsetT southWest = {-1 , -1};

    for (auto it = rayOffsets.begin()+1; it!= rayOffsets.end(); ++it)
    {
        (*it)[0]  = (*(it-1))[0] + east;
        (*it)[1]  = (*(it-1))[1] + north;
        (*it)[2]  = (*(it-1))[2] + southWest;

        //std::cout<<(*it)[2]<<std::endl;
    }


    GrayImageT::SizeType radius = {{raysSize, raysSize}};

    itk::ImageRegionConstIterator<GrayImageT> eIt(edges, edges->GetRequestedRegion()); //edges it
    itk::ImageRegionIterator<FloatImageT>     oIt(outputImage, outputImage->GetRequestedRegion()); //output it
    itk::NeighborhoodIterator<GrayImageT>     gIt(radius, edges, edges->GetRequestedRegion()); //gray it


    for ( ;!eIt.IsAtEnd(); ++eIt, ++gIt, ++oIt)
    {

        if(eIt.Get() == Foreground)
        {
            std::vector<unsigned> distance(3,raysSize);
            std::vector<bool> distanceFlags(3, false);
            for (unsigned  i=1;  i < rayOffsets.size(); ++i)
            {


                if(gIt.GetPixel(rayOffsets[i][0]) == Foreground && distanceFlags[0] == false)
                {
                    distance[0] = i;
                    distanceFlags[0] = true;
                }

                if(gIt.GetPixel(rayOffsets[i][1]) == Foreground && distanceFlags[1] == false)
                {
                    distance[1] = i;
                    distanceFlags[1] = true;
                }

                if(gIt.GetPixel(rayOffsets[i][2]) == Foreground && distanceFlags[2] == false)
                {
                    distance[2] = i;
                    distanceFlags[2] = true;
                }


            }

            if(*std::max_element(distance.begin(), distance.end())>=raysSize)
            {
                oIt.Set(raysSize);
            }

        }

    }


    if(show)
    {

        using RescaleType = itk::RescaleIntensityImageFilter<FloatImageT, GrayImageT>;
        RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(outputImage);
        rescaler->SetOutputMinimum(Background);
        rescaler->SetOutputMaximum(Foreground);
        rescaler->Update();


        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput(), "Ray features");
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

        bIt.Set( (gIt.Get() == Foreground )? Background : Foreground );

    }


    if(show)
    {

        VTKViewer::visualize<GrayImageT>(binaryImage, "Gray to Binary");
    }


    io::printOK("Simple gray to binary");

    return binaryImage;


}


template<typename InputImageT>
void PleuraDetector<InputImageT>::ComputeLBP(GrayImageP grayImage, GrayImageP edges,
                                             const std::vector<GrayImageT::IndexType>& centers,
                                             const unsigned neighborhoodSize,
                                             LBPHistogramsT& lbpHistograms)
{


    using grayPixeT = GrayImageT::PixelType;
    using dlibGrayImageT = dlib::array2d<grayPixeT>;

    //ITK to Dlib image
    dlibGrayImageT dlibGrayImage;
    ifc::ITKToDlib<grayPixeT, grayPixeT>(grayImage, dlibGrayImage);

    //compute uniform LBP
    using lbpImageT = dlib::array2d<unsigned char>;
    lbpImageT lbpImage;
    dlib::make_uniform_lbp_image(dlibGrayImage, lbpImage);

    //
    // dlib::image_window my_window(lbpImage, "LBP");
    // my_window.wait_until_closed();


    dlib::array2d<float> outputImage;
    outputImage.set_size(lbpImage.nr(), lbpImage.nc());


    itk::ImageRegionConstIteratorWithIndex<GrayImageT> eIt(edges, edges->GetRequestedRegion());



    dlib::array2d<unsigned char> neighborhood;


    const auto ExtractNeighborhood = util::ExtractNeighborhood<lbpImageT, GrayImageT::IndexType>;


    dlib::matrix<unsigned long> lbpHistogramLocal;
    LBPHistogramT lbpHistogram;



    //ofstream file ("/home/oscar/lbp.txt");


    //for (;!eIt.IsAtEnd();  ++eIt)
    for(auto it = centers.begin(); it != centers.end(); ++it)
    {


        //if(eIt.Get() == Foreground)
        {
            //auto index = eIt.GetIndex();
            auto index = (*it);
            ExtractNeighborhood(lbpImage, index, neighborhoodSize, neighborhood);

            //dlib::image_window my_window2(neighborhood, "LBP");
            //my_window2.wait_until_closed();

            dlib::get_histogram(neighborhood, lbpHistogramLocal, 59);


            auto it = lbpHistogramLocal.begin()+1;
            auto it2 = lbpHistogram.begin();
            for(; it != lbpHistogramLocal.end()-2; ++it, ++it2)
            {

                (*it2) = std::sqrt(*it);
                //          file<<std::sqrt(*it)<<",";
            }
            (*it2) = std::sqrt(*it);
            //    file<<std::sqrt(*it)<<std::endl;

            //dlib::remove_row(lbpHistogram, 0);

            lbpHistograms.push_back(lbpHistogram);

            //auto hist = dlib::remove_row(lbpHistogram, 0);

            //std::cout<<lbpHistogram.size()<<std::endl;
            //std::cout<<std::endl;


            //VTKViewer::PlotBar<std::vector<long>>(std::vector<long>(hist.begin(), hist.end()), 58);

        }



    }



    io::printOK("Local Binary Pattern");


    // dlib::image_window my_window2(outputImage, "LBP");
    // my_window2.wait_until_closed();

    /*std::cout<<histograms.size()<<std::endl;
    VTKViewer::PlotBar<std::vector<int>>( histograms, histograms.size());
*/

}


template<typename InputImageT>
typename PleuraDetector<InputImageT>::GrayImageP PleuraDetector<InputImageT>::ExtractBoundaries(GrayImageP binaryImage, bool show)
{

    //Connecting background
    ConnectBackground(binaryImage);

    //Detecting background
    using ConnectedFilterType = itk::ConnectedThresholdImageFilter<GrayImageT, GrayImageT>;
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
    connectedThreshold->SetInput(binaryImage);
    connectedThreshold->SetLower(Background);
    connectedThreshold->SetUpper(Background);
    connectedThreshold->SetSeed({0,0}); //TODO define it
    connectedThreshold->SetReplaceValue(Foreground);

    connectedThreshold->Update();



    using binaryContourImageFilterType = itk::BinaryContourImageFilter<GrayImageT, GrayImageT>;

    binaryContourImageFilterType::Pointer binaryContourFilter = binaryContourImageFilterType::New();
    binaryContourFilter->SetInput(connectedThreshold->GetOutput());
    binaryContourFilter->SetBackgroundValue(Background);
    binaryContourFilter->SetForegroundValue(Foreground);
    binaryContourFilter->FullyConnectedOn();
    binaryContourFilter->Update();



    if(show)
    {

        VTKViewer::visualize<GrayImageT>(binaryContourFilter->GetOutput(), "Boundaries");
    }


    io::printOK("Extract boundaries");


    return binaryContourFilter->GetOutput();





}

template<typename InputImageT>
void PleuraDetector<InputImageT>:: ConnectBackground(GrayImageP& grayImage)
{

    const auto upperIndex = grayImage->GetRequestedRegion().GetUpperIndex();

    for (unsigned col=0; col <= upperIndex[0]; ++col)
    {
        grayImage->SetPixel({col,0}, Background);
        grayImage->SetPixel({col,upperIndex[1]}, Background);
    }


    for (unsigned row=0; row <= upperIndex[1]; ++row)
    {
        grayImage->SetPixel({0,row}, Background);
        grayImage->SetPixel({upperIndex[0], row}, Background);
    }


}


template<typename InputImageT>
void PleuraDetector<InputImageT>::SpectralClustering(LBPHistogramsT& lbpHistograms, std::vector<unsigned long>& assignments)
{

    using kernelT = dlib::radial_basis_kernel<LBPHistogramT>;

    using kernelT = dlib::radial_basis_kernel<LBPHistogramT>;

    dlib::kcentroid<kernelT> kc(kernelT(0.1), 0.01, 8);

    dlib::kkmeans<kernelT> test(kc);

    std::vector<LBPHistogramT> initial_centers;

    LBPHistogramT m;
    dlib::rand rnd;

    test.set_number_of_centers(2);

    dlib::pick_initial_centers(2, initial_centers, lbpHistograms, test.get_kernel());

    test.train(lbpHistograms,initial_centers);


    assignments = dlib::spectral_cluster(kernelT(0.1), lbpHistograms, 2);
    //cout <<dlib::mat(assignments) << endl;


    io::printOK("Spectral Clustering");

}


template<typename InputImageT>
void PleuraDetector<InputImageT>::ComputeCenters(GrayImageP boundaries,
                                                 unsigned neigborhoodSize,
                                                 std::vector<GrayImageT::IndexType>& centers)
{

    auto tmpImage = GrayImageT::New();
    tmpImage->SetRegions(boundaries->GetRequestedRegion());
    tmpImage->Allocate();
    tmpImage->FillBuffer(100);

    itk::NeighborhoodIterator<GrayImageT>::RadiusType radius;
    radius.Fill(neigborhoodSize/2);

    itk::NeighborhoodIterator<GrayImageT> it(radius, boundaries, boundaries->GetRequestedRegion());

    for (;!it.IsAtEnd(); ++it)
    {

        if(it.GetCenterPixel() == Foreground)
        {

            centers.push_back(it.GetIndex());
            for (unsigned i=0; i < it.Size(); ++i)
            {
                if(it.InBounds())
                {
                    //drawing windows
                    /*for (unsigned i=0;i<it.Size();++i)
                    {

                        const auto& index = it.GetIndex(i);
                        tmpImage->SetPixel(index, grayImage->GetPixel(index));

                    }
                    */
                    it.SetPixel(i, Background);
                }

            }

        }


    }

    //  VTKViewer::visualize<GrayImageT>(tmpImage, "Centers");
    io::printOK("Compute Centers");

}



template<typename InputImageT>
void PleuraDetector<InputImageT>:: ShowAssignments(const SCAssignments& assignments, const std::vector<GrayImageT::IndexType>& centers)
{


    auto outputImage = GrayImageT::New();
    outputImage->SetRegions(InputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->FillBuffer(255/2);


    for (unsigned i = 0;  i <  centers.size(); ++i)
    {

        outputImage->SetPixel(centers[i],  assignments[i]*255);

    }

    VTKViewer::visualize<GrayImageT>(outputImage, "Assignments");



}


template<typename InputImageT>
void PleuraDetector<InputImageT>::ComputeFractalDimensionCenters(GrayImageP boundaries,
                                                                 unsigned neigborhoodSize,
                                                                 const IndexVector& centers,
                                                                 std::vector<float>& output,
                                                                 bool show)
{


    output.resize(centers.size());

    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(InputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->FillBuffer(255.f);


    const auto extractNeighborhood = util::ExtractNeighborhoodITK<GrayImageT>;

    auto outIt = output.begin();
    for(auto it = centers.begin(); it != centers.end(); ++it, ++outIt) //fails here...
    {


        GrayImageP neighborhood;
        extractNeighborhood(boundaries, *it, neigborhoodSize, neighborhood);


        auto fractalDimensionFilter = std::make_unique<FractalDimensionFilter<GrayImageT>>();
        fractalDimensionFilter->SetInputImage(neighborhood);
        fractalDimensionFilter->PrintWarningsOff();
        fractalDimensionFilter->Compute();
        (*outIt) = fractalDimensionFilter->GetDimension();


        /*
        if(std::isnan(dimension))
        {

            std::cerr<<"nan"<<std::endl;
            // VTKViewer::visualize<GrayImageT>(neighborhood, "Fractal dimension");
        }
*/

        outputImage->SetPixel(*it,  (*outIt));
        //std::cout<<neighborhood->GetRequestedRegion().GetSize()<<std::endl;



    }


    if(show)
    {

        using RescaleType = itk::RescaleIntensityImageFilter<FloatImageT, GrayImageT>;
        RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(outputImage);
        rescaler->SetOutputMinimum(Background);
        rescaler->SetOutputMaximum(Foreground);
        rescaler->Update();

        VTKViewer::visualize<GrayImageT>(rescaler->GetOutput(), "Fractal dimension");
    }


    io::printOK("Computing Fractal Dimension");


}
template<typename InputImageT>
typename PleuraDetector<InputImageT>::GrayImageP PleuraDetector<InputImageT>::ThinningBoundaries(GrayImageP boundaries, bool show)
{

    auto componentsEdges  = ConnectedComponets(boundaries, 50, Background, false);


    auto binaryBoundaries = util::LabelMapToBinaryImage<LabelMapType, GrayImageT>(componentsEdges, 0, 1, false);

    using BinaryThinningImageFilterType = itk::BinaryThinningImageFilter<GrayImageT, GrayImageT>;
    BinaryThinningImageFilterType::Pointer binaryThinningImageFilter = BinaryThinningImageFilterType::New();
    binaryThinningImageFilter->SetInput(binaryBoundaries);
    binaryThinningImageFilter->Update();

    // Rescale the image so that it can be seen (the output is 0 and 1, we want 0 and 255)
    using RescaleType = itk::RescaleIntensityImageFilter<GrayImageT, GrayImageT>;
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(binaryThinningImageFilter->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    auto thinBoundaries = rescaler->GetOutput();

    if(show)
    {
        VTKViewer::visualize<GrayImageT>(thinBoundaries, "Thin boundaries");

    }


    //this is temporary, delete it, I use this during the ground truth creation...
    io::WriteImage<GrayImageT>(thinBoundaries, "/home/oscar/data/biopsy/tiff/test/boundaries/"+ImageName+"_boundaries.tiff");


    io::printOK("Thinning Boundaries");


    return thinBoundaries;




}

template<typename InputImageT>
void PleuraDetector<InputImageT>::SetImageName(const std::string& dirPath, const std::string& fileName)
{

    this->ImageDirPath = dirPath;
    this->ImageName = fileName;


}

template<typename InputImageT>
void PleuraDetector<InputImageT>:: WriteCSVFile(const std::string& fileName,
                                                const IndexVector& centers,
                                                unsigned neighborhoodSize,
                                                const std::vector<float>& fractalDimension,
                                                const LBPHistogramsT& LBPHistograms,
                                                const CooccurrenceFeatures& cooccurrenceFeatures,
                                                const std::vector<float>& labels,
                                                bool writeHeader)
{


    std::ofstream csvFile(fileName);

    if(writeHeader)
    {
        //writting header

        csvFile<<"c,r,ns,fd,";
        unsigned i=1;
        for (auto it = (*LBPHistograms.begin()).begin() ; it != (*LBPHistograms.begin()).end()  ; ++it)
        {

            csvFile<<"blp_"+ std::to_string(i++)+",";


        }

        i=1;
        for (auto it = (*cooccurrenceFeatures.begin()).begin() ; it != (*cooccurrenceFeatures.begin()).end()  ; ++it)
        {

            csvFile<<"coo_"+ std::to_string(i++)+",";


        }

        csvFile<<"label";

        csvFile<<std::endl;


    }
    auto fdIt  = fractalDimension.begin();
    auto lbpIt = LBPHistograms.begin();
    auto cIt   = centers.begin();
    auto cooIt = cooccurrenceFeatures.begin();
    auto lIt = labels.begin();


    for (;fdIt != fractalDimension.end(); ++fdIt, ++lbpIt, ++cIt, ++cooIt, ++lIt)
    {

        csvFile<< (*cIt)[0] <<","<<(*cIt)[1]<<","<<neighborhoodSize<<","<<*fdIt<<",";
        for (auto it = (*lbpIt).begin() ; it != (*lbpIt).end(); ++it)
        {

            csvFile<<(*it)<<",";

        }
        for (auto it = (*cooIt).begin() ; it != (*cooIt).end(); ++it)
        {

            csvFile<<(*it)<<",";

        }

        csvFile<<*lIt;
        csvFile<<std::endl;


    }


    io::printOK("Writing CSV file");


}



template<typename InputImageT>
void PleuraDetector<InputImageT>::ReadAssignmentsFile(const std::string& fileName, std::vector<unsigned>& assignments)
{

    std::ifstream classFile(fileName);


    std::string line;
    if (classFile.is_open())
    {
        while ( getline (classFile,line) )
        {
            assignments.push_back(std::stoi(line));
            //std::cout<<std::stoi(line)<<std::endl;
        }

        classFile.close();
    }


}


template<typename InputImageT>
void PleuraDetector<InputImageT>::ReadCSVFile(const std::string& fileName,
                                              std::vector<SampleT>& samples,
                                              unsigned samplesIndexBegin,
                                              unsigned samplesIndexEnd,
                                              std::vector<float>& labels,
                                              unsigned labelsIndex)
{


    std::ifstream csvFile(fileName);


    std::string line;
    std::string value;

    unsigned sampleSize = samplesIndexEnd - samplesIndexBegin + 1;

    if (csvFile.is_open())
    {
        std::getline (csvFile, line); // header

        while ( std::getline (csvFile, line))
        {
            std::istringstream istLine(line);

            //std::cout<<line<<std::endl;
            for (unsigned index=0; std::getline(istLine, value, ','); ++index)
            {

                //verify label
                if(index == labelsIndex)
                {
                    labels.push_back(std::stof(value));
                    //std::cout<<std::stof(value)<<std::endl;
                }

                if(index == samplesIndexBegin)
                {
                    SampleT  sample(sampleSize,1);
                    sample(0) = std::stof(value); // first index already got
                    for (unsigned sIndex=1; sIndex < sampleSize ; ++sIndex)
                    {
                        std::getline(istLine, value, ',');
                        sample(sIndex) = std::stof(value);
                        ++index;
                    }

                    samples.push_back(sample);
                    //std::cout<<sample<<std::endl;
                }
            }

        }

        csvFile.close();
    }



    io::printOK("Read CSV samples");
}


template<typename InputImageT>
void PleuraDetector<InputImageT>:: DrawAssignments(IndexVector& centers, unsigned neighborhoodSize, std::vector<unsigned>& assignments)
{


    using DuplicatorType = itk::ImageDuplicator<InputImageT>;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(InputImage);
    duplicator->Update();

    RGBImageP outputImage  =  duplicator->GetOutput();;

    using AddImageFilterType = itk::AddImageFilter<InputImageT, InputImageT>;

    const auto extractNeighborhood = util::ExtractNeighborhoodITK<InputImageT>;

    typename InputImageT::PixelType color;

    auto cIt = centers.begin();
    auto assIt  =  assignments.begin();
    auto imgRegion = InputImage->GetRequestedRegion();
    for (; cIt != centers.end(); ++cIt, ++assIt)
    {

        RGBImageP neighborhood;

        extractNeighborhood(InputImage, *cIt, neighborhoodSize, neighborhood);

        auto region = neighborhood->GetRequestedRegion();

        auto index = *cIt;
        index[0] = ((*cIt)[0] - neighborhoodSize/2 < 0)? 0 : (*cIt)[0] - neighborhoodSize/2;
        index[1] = ((*cIt)[1] - neighborhoodSize/2 < 0)? 0 : (*cIt)[1] - neighborhoodSize/2;

        region.SetIndex(index);



        if(*assIt==0)
        {
            color.Set(0,0,0);
        }
        else if(*assIt==1)
        {
            color.Set(255,0,0);
        }
        else if(*assIt==2)
        {
            color.Set(0,255,0);
        }
        /*else if(*assIt==3)
        {
            color.Set(0,0,255);
        }
        else
        {
            color.Set(255,255,255);
        }*/


        util::PaintRegion<InputImageT>(outputImage, region, color);



    }



    if(1)
    {
        VTKViewer::visualize<InputImageT>(outputImage, "Assignments");

    }


}



template<typename InputImageT>
void PleuraDetector<InputImageT>::ComputeCooccurrenceMatrices(GrayImageP image,
                                                              unsigned neigborhoodSize,
                                                              const IndexVector& centers,
                                                              std::vector<std::vector<float>>& features)
{

    /*
        //See
        //https://itk.org/Doxygen/html/classitk_1_1Statistics_1_1HistogramToTextureFeaturesFilter.html
        //https://itk.org/Doxygen/html/classitk_1_1Statistics_1_1ScalarImageToCooccurrenceMatrixFilter.html#ae1c2899a24c24cd8e02e999d7f6d2e0d

    */
    /*
    using floatImageT = itk::Image<float, 2>;
    using castFilterType = itk::CastImageFilter<GrayImageT,floatImageT>;
    castFilterType::Pointer castFilter = castFilterType::New();
    castFilter->SetInput(image);
    castFilter->Update();
    auto floatImage = castFilter->GetOutput();
*/

    features.resize(centers.size());

    using ScalarImageToCooccurrenceMatrixFilter =  itk::Statistics::ScalarImageToCooccurrenceMatrixFilter<GrayImageT>;


    using histogramT =  ScalarImageToCooccurrenceMatrixFilter::HistogramType;
    using offsetVectorT = itk::VectorContainer<unsigned char, ScalarImageToCooccurrenceMatrixFilter::OffsetType>;

    //using offsetT = GrayImageT::OffsetType;
    //offsetT offset = {-1,1};

    offsetVectorT::Pointer offsets = offsetVectorT::New();
    // offsets->reserve(2);

    offsets->push_back({-1,1});
    offsets->push_back({1,1});
    offsets->push_back({1,-1});




    using  histToFeaturesT = itk::Statistics::HistogramToTextureFeaturesFilter<histogramT>;


    const auto extractNeighborhood = util::ExtractNeighborhoodITK<GrayImageT>;

    auto fIt = features.begin();
    for(auto it = centers.begin(); it != centers.end(); ++it, ++fIt)
    {

        GrayImageT::Pointer neighborhood;
        extractNeighborhood(image, *it, neigborhoodSize, neighborhood);


        ScalarImageToCooccurrenceMatrixFilter::Pointer scalarImageToCooccurrenceMatrixFilter = ScalarImageToCooccurrenceMatrixFilter::New();

        scalarImageToCooccurrenceMatrixFilter->SetInput(neighborhood);
        scalarImageToCooccurrenceMatrixFilter->SetOffsets(offsets);
        scalarImageToCooccurrenceMatrixFilter->SetNumberOfBinsPerAxis(16);
        scalarImageToCooccurrenceMatrixFilter->SetPixelValueMinMax(0,255);


        scalarImageToCooccurrenceMatrixFilter->Update();


        histToFeaturesT::Pointer features =  histToFeaturesT::New();
        features->SetInput(scalarImageToCooccurrenceMatrixFilter->GetOutput());
        features->Update();

        (*fIt).push_back(features->GetEnergy());
        (*fIt).push_back(features->GetEntropy());
        (*fIt).push_back(features->GetCorrelation());
        (*fIt).push_back(features->GetInertia());


    }


    io::printOK("Co-occurrence Matrix features");


}



template<typename InputImageT>
void PleuraDetector<InputImageT>::MatchCentersWithLabels(const IndexVector& centers, std::vector<float>& labels)
{


    //1 = pleura
    //2 = not pleura
    labels = std::vector<float>(centers.size(), 0);


    typename InputImageT::PixelType greenColor;
    greenColor.Set(0,255,0);

    auto lIt = labels.begin();
    for (auto cIt = centers.begin(); cIt != centers.end() ; ++cIt, ++lIt)
    {

        (*lIt) = ( LabelImage->GetPixel(*cIt) == greenColor)? +1 : -1;

    }


    io::printOK("Match Centers with Labels");


}


template<typename InputImageT>
void PleuraDetector<InputImageT>::SetCSVFileName(std::string csvFileName)
{


    this->CSVFilename = csvFileName;

}


/*
Add all features in a DLib matrix

*/

template<typename InputImageT>
void PleuraDetector<InputImageT>::FeaturesToDLibMatrix(const std::vector<float>& fractalDimension,
                                                       const LBPHistogramsT& LBPHistograms,
                                                       const CooccurrenceFeatures& cooccurrenceFeatures,
                                                       std::vector<SampleT>& samples)
{


    samples = std::vector<SampleT>(fractalDimension.size(), SampleT(4,1));

    auto fIt = samples.begin();
    auto fdIt  = fractalDimension.begin();
    auto lbpIt = LBPHistograms.begin();
    auto cooIt = cooccurrenceFeatures.begin();



    for (;fdIt != fractalDimension.end(); ++fdIt, ++lbpIt, ++cooIt, ++fIt)
    {

        //auto sIt = fIt->begin();

        //(*sIt) = *fdIt; //+1, +59

        //std::copy( lbpIt->begin(), lbpIt->end(), sIt);

        std::copy( cooIt->begin(), cooIt->end(), fIt->begin());

       // std::cout<<*fIt<<std::endl;


    }



    io::printOK("Features to DLib Matrix");

}





template<typename InputImageT>
void PleuraDetector<InputImageT>::KRRTrainer( std::vector<SampleT>& samples, const std::vector<double>& labels)
{


    using kernelT = dlib::radial_basis_kernel<SampleT>;

    dlib::vector_normalizer<SampleT> normalizer;
    normalizer.train(samples);


    for ( auto it = samples.begin(); it != samples.end(); ++it)
    {
        (*it) = normalizer(*it);

    }



    dlib::krr_trainer<kernelT> trainer;

    trainer.use_classification_loss_for_loo_cv();


    for (double gamma = 0.000001; gamma <= 1; gamma *= 5)
    {
        // tell the trainer the parameters we want to use
        trainer.set_kernel(kernelT(gamma));

        // loo_values will contain the LOO predictions for each sample.  In the case
        // of perfect prediction it will end up being a copy of labels.
        std::vector<double> loo_values;
        trainer.train(samples, labels, loo_values);

        // Print gamma and the fraction of samples correctly classified during LOO cross-validation.
        const double classification_accuracy = dlib::mean_sign_agreement(labels, loo_values);
        cout << "gamma: " << gamma << "     LOO accuracy: " << classification_accuracy << endl;
    }



    trainer.set_kernel(kernelT(0.390625));
    using decFunctT = dlib::decision_function<kernelT>;
    using functT = dlib::normalized_function<decFunctT>;

    functT learned_function;
    learned_function.normalizer = normalizer;  // save normalization information
    learned_function.function = trainer.train(samples, labels); // perform the actual training and save the results


    // print out the number of basis vectors in the resulting decision function
    //std::cout << "\nnumber of basis vectors in our learned_function is " << learned_function.function.basis_vectors.size() << std::endl;


    dlib::serialize("/home/oscar/function.dat")<<learned_function;


    functT learned_function2;
    dlib::deserialize("/home/oscar/function.dat")>>learned_function2;

    std::cout << "\nnumber of basis vectors in our learned_function is " << learned_function2.function.basis_vectors.size() << std::endl;


    io::printOK("Kernel Ridge Classification");





}

template<typename InputImageT>
void PleuraDetector<InputImageT>::Detect()
{



/*


    //tmp
    std::vector<unsigned> assignments;
    //ReadAssignmentsFile("/home/oscar/tmp/output_3.csv", assignments);
    std::vector<float> labels2;
    std::vector<SampleT> samplesTmp;
    ReadCSVFile(CSVFilename, samplesTmp, 62, 65, labels2, 66);




    //DrawAssignments(centers2, 101, assignments);



    return;
*/


    //pre-processing
    auto rgbImage = CleanBackground(86, 5,5, false); //90

    //rgb to gray
    using rgbToGrayFilterType = itk::RGBToLuminanceImageFilter<InputImageT, GrayImageT>;
    auto rgbToGrayFilter = rgbToGrayFilterType::New();
    rgbToGrayFilter->SetInput(rgbImage);
    rgbToGrayFilter->Update();




    using RescaleType = itk::RescaleIntensityImageFilter<GrayImageT, GrayImageT>;
    RescaleType::Pointer rescaler = RescaleType::New();
    rescaler->SetInput(rgbToGrayFilter->GetOutput());
    rescaler->SetOutputMinimum(Background);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();
    auto grayImage = rescaler->GetOutput();


    auto eqGrayImage = HistogramEqualization(grayImage, 1, 1, 5, false);

    //binary
    auto binaryImage = GrayToBinary(eqGrayImage, false);
    //boundaries
    auto boundaries = ExtractBoundaries(binaryImage, false);

    //io::WriteImage<GrayImageT>(eqGrayImage, "/home/oscar/gray.png");


    //auto binaryImage = GrayToBinary(eqGrayImage, true);
    //auto background  = ConnectedComponets(binaryImage, 0, Foreground,  true);


    /*
    using moothFilterType = itk::SmoothingRecursiveGaussianImageFilter<GrayImageT, GrayImageT>;
    moothFilterType::Pointer smoothFilter = moothFilterType::New();
    smoothFilter->SetInput(binaryImage);
    smoothFilter->SetSigma(1);
    smoothFilter->Update();
    auto smoothImage = smoothFilter->GetOutput();

   // VTKViewer::visualize<GrayImageT>(smoothImage, "Gray to Binary");

  // auto components  = ConnectedComponets(smoothImage, 0,  false);




    ExtractBoundaries(smoothImage, true);
*/
    //auto edges = EdgeDetectionCanny(eqGrayImage, 5, false);

    auto thinBoundaries = ThinningBoundaries(boundaries, false);



    //Computing features from here

    unsigned neighborhoodSize = 51;
    std::vector<GrayImageT::IndexType> centers;
    ComputeCenters(thinBoundaries, neighborhoodSize, centers);


    std::vector<float> labels;
    MatchCentersWithLabels(centers, labels);


    std::vector<float> fractalDimentions;
    ComputeFractalDimensionCenters(thinBoundaries, neighborhoodSize, centers, fractalDimentions, false);


    // return;

    //Co-occurrence matrix.
    std::vector<std::vector<float>> cooFeatures;
    ComputeCooccurrenceMatrices(grayImage, neighborhoodSize, centers, cooFeatures);



    LBPHistogramsT lbpHistograms;
    ComputeLBP(grayImage, thinBoundaries, centers, neighborhoodSize, lbpHistograms);


    WriteCSVFile(CSVFilename, centers, neighborhoodSize ,fractalDimentions, lbpHistograms, cooFeatures, labels, false);

    std::vector<SampleT> samples;
    FeaturesToDLibMatrix(fractalDimentions,lbpHistograms, cooFeatures, samples);


    std::vector<double> labelsDouble(labels.begin(), labels.end());
    KRRTrainer(samples, labelsDouble);



    /*
    std::vector<unsigned long> assignments;
    SpectralClustering(lbpHistograms, assignments);

    ShowAssignments(assignments, centers);
    */


    io::printOK("Detecting pleura");

}

