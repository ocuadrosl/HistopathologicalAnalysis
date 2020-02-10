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
         distance = mean1 = mean2 = size1 = size2 = 0.f;

         for(unsigned i=0; i< it.Size(); ++i)
         {

             if (it.GetPixel(i) < 0) //cells
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
void CellSegmentator<rgbImageT>::findCells()
{

    CreateImageB(true);


    //LabelRoughly();
    auto map = computeDistances(BImage, true);
    map = computeDistances(map, true);
    //map = computeDistances(map, true);
    //map = computeDistances(map, true);
    //map = computeDistances(map, true);
   // map = computeDistances(map, true);


}


template<typename imageT>
void CellSegmentator<imageT>::superPixels()
{



}





