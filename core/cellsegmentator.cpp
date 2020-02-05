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

    BImage = extractChannelFilter->getOutputImage();

    //visualizing
    if(show)
    {

        using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
        rescaleFilterType2::Pointer rescaleFilter2 = rescaleFilterType2::New();
        rescaleFilter2->SetInput(extractChannelFilter->getOutputImage());
        rescaleFilter2->SetOutputMinimum(0);
        rescaleFilter2->SetOutputMaximum(255);
        rescaleFilter2->Update();

        VTKViewer::visualize<grayImageT>(rescaleFilter2->GetOutput() ,"b channel Image");
    }


}

template<typename rgbImageT>
void CellSegmentator<rgbImageT>::computeDistances(bool show)
{

    distanceMap =  floatImageT::New();
    distanceMap->SetRegions(BImage->GetRequestedRegion());
    distanceMap->Allocate();



    using neighborhoodIteratorT = itk::NeighborhoodIterator<floatImageT>;
    neighborhoodIteratorT::RadiusType radius;
    radius.Fill(5);

    itk::NeighborhoodIterator<floatImageT> it(radius , BImage, BImage->GetRequestedRegion());
    itk::ImageRegionIterator<floatImageT> dIt(distanceMap, distanceMap->GetRequestedRegion());


     while(!it.IsAtEnd())
     {
         float distance =0.f;

         for(unsigned i=0; i< it.Size(); ++i)
         {
             distance += std::abs(it.GetCenterPixel() - it.GetPixel(i));
         }

         dIt.Set(distance/it.Size() + (10*it.GetCenterPixel()));

         ++it;
         ++dIt;
     }




     if(show)
     {

         using rescaleFilterType2= itk::RescaleIntensityImageFilter<floatImageT, grayImageT>;
         rescaleFilterType2::Pointer rescaleFilter = rescaleFilterType2::New();
         rescaleFilter->SetInput(distanceMap);
         rescaleFilter->SetOutputMinimum(0);
         rescaleFilter->SetOutputMaximum(255);
         rescaleFilter->Update();

         VTKViewer::visualize<grayImageT>(rescaleFilter->GetOutput() ,"distance map");
     }





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

    itk::ImageRegionConstIterator<floatImageT> BIt(BImage, BImage->GetRequestedRegion());
    itk::ImageRegionIterator<grayImageT> labelIt(labelMap, labelMap->GetRequestedRegion());


    while(!BIt.IsAtEnd())
    {

        labelIt.Set( (BIt.Get() <= threshold)? 1 : 255 );

        ++BIt;
        ++labelIt;

    }

    VTKViewer::visualize<grayImageT>(labelMap ,"Label Map");




}



template<typename rgbImageT>
void CellSegmentator<rgbImageT>::findCells()
{

    CreateImageB(true);
    computeDistances(true);
    //LabelRoughly();



}


template<typename imageT>
void CellSegmentator<imageT>::superPixels()
{



}





