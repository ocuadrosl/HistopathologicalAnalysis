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
                                                 std::vector<GrayImageT::IndexType>& centers,
                                                 GrayImageP grayImage)
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
                                                                 bool show)
{
    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(InputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->FillBuffer(255.f);


    const auto extractNeighborhood = util::ExtractNeighborhoodITK<GrayImageT>;

    for(auto it = centers.begin(); it != centers.end(); ++it) //fails here...
    {

        std::cout<<*it<<std::endl;

        GrayImageP neighborhood;
        extractNeighborhood(boundaries, *it, neigborhoodSize, neighborhood);
/*

        auto fractalDimensionFilter = std::make_unique<FractalDimensionFilter<GrayImageT>>();
        fractalDimensionFilter->SetInputImage(neighborhood);
        fractalDimensionFilter->PrintWarningsOff();
        fractalDimensionFilter->Compute();
        float dimension = fractalDimensionFilter->GetDimension();


        if(std::isnan(dimension))
        {

            // VTKViewer::visualize<GrayImageT>(neighborhood, "Fractal dimension");
        }


        outputImage->SetPixel(*it, dimension);
        std::cout<<neighborhood->GetRequestedRegion().GetSize()<<std::endl;
        std::cout<<dimension<<" dim"<<std::endl;

*/

    }


    std::cout<<"exit"<<std::endl;
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


    //this is temporary, delete it
    io::WriteImage<GrayImageT>(thinBoundaries, "/home/oscar/data/biopsy/tiff/dataset_1/"+ImageName+"_boundaries.tiff");


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
void PleuraDetector<InputImageT>::Detect()
{


    //pre-processing
    auto rgbImage = CleanBackground(86, 5,5, false); //90

    //rgb to gray
    using rgbToGrayFilterType = itk::RGBToLuminanceImageFilter<InputImageT, GrayImageT>;
    auto rgbToGrayFilter = rgbToGrayFilterType::New();
    rgbToGrayFilter->SetInput(rgbImage);
    rgbToGrayFilter->Update();

    auto grayImage = rgbToGrayFilter->GetOutput();

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


    return;

    unsigned neighborhoodSize = 101;
    std::vector<GrayImageT::IndexType> centers;
    ComputeCenters(thinBoundaries, neighborhoodSize, centers, grayImage);


    ComputeFractalDimensionCenters(thinBoundaries, neighborhoodSize, centers, true);



    return;

    LBPHistogramsT lbpHistograms;
    ComputeLBP(grayImage, thinBoundaries, centers, neighborhoodSize, lbpHistograms);

    std::vector<unsigned long> assignments;
    SpectralClustering(lbpHistograms, assignments);

    ShowAssignments(assignments, centers);



    io::printOK("Detecting pleura");

}
