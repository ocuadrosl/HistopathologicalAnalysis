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


        VTKViewer::visualize<GrayImageT>(rgbToGrayFilter->GetOutput(), "Connected components gray-scale");

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
PleuraDetector<InputImageT>::ComputeFractalDimension(LabelMapP components, bool show)
{



    auto outputImage = FloatImageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();
    outputImage->Allocate(Background);
    outputImage->FillBuffer(0.f);


    using FilterType = itk::RegionOfInterestImageFilter<GrayImageT, GrayImageT>;



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

        if(dimension < 1.1f)
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
void PleuraDetector<InputImageT>::Detect()
{

    //rgb to gray
    using rgbToGrayFilterType = itk::RGBToLuminanceImageFilter<InputImageT, GrayImageT>;
    typename rgbToGrayFilterType::Pointer rgbToGrayFilter = rgbToGrayFilterType::New();
    rgbToGrayFilter->SetInput(inputImage);
    rgbToGrayFilter->Update();
    auto grayImage = rgbToGrayFilter->GetOutput();


    //VTKViewer::visualize<GrayImageT>(grayImage);

    //GeodesicActiveCountour(grayImage, true);
    auto edges = EdgeDetectionCanny(grayImage, false);
    auto components = ConnectedComponets(edges, 50,  false);

    auto fractalDim = ComputeFractalDimension(components, true);

/*
    auto fractalDimensionFilter = std::make_unique<FractalDimensionFilter<GrayImageT>>();
    fractalDimensionFilter->SetInputImage(edges);
    fractalDimensionFilter->SetUnitTileLenght(100);
    fractalDimensionFilter->Compute();
    std::cout<<fractalDimensionFilter->GetDimension()<<std::endl;
*/


    io::printOK("Detecting pleura");





}
