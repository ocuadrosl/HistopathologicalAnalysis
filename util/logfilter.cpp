#include "logfilter.h"
template<typename inputImageT, typename outputImageT>
LoGFilter<inputImageT, outputImageT>::LoGFilter()
{

}

template<typename inputImageT, typename outputImageT>
void LoGFilter<inputImageT, outputImageT>::setImage(inputImageP inputImage)
{
    this->inputImage = inputImage;
}

template<typename inputImageT, typename outputImageT>
void LoGFilter<inputImageT, outputImageT>::setSigma(double sigma)
{

    this->sigma = sigma;
}

template<typename inputImageT, typename outputImageT>
typename LoGFilter<inputImageT, outputImageT>::outputImageP
LoGFilter<inputImageT, outputImageT>::getOutput()
{
    return outputImage;
}

template<typename inputImageT, typename outputImageT>
void LoGFilter<inputImageT, outputImageT>::compute(bool show)
{

    createKernel();

    using castInputImageT = itk::CastImageFilter<inputImageT, doubleImageT>;
    typename castInputImageT::Pointer castInputImageFilter = castInputImageT::New();
    castInputImageFilter->SetInput(inputImage);


    using convolutionFilterT = itk::ConvolutionImageFilter<doubleImageT>;
    typename convolutionFilterT::Pointer convolutionFilter = convolutionFilterT::New();
    convolutionFilter->SetInput(castInputImageFilter->GetOutput());
    convolutionFilter->SetKernelImage(kernel);
    convolutionFilter->Update();

    //TODO if constexpr to avoid unnecessary image cast

    using castOutputImageT = itk::CastImageFilter<doubleImageT, outputImageT>;
    typename castOutputImageT::Pointer castOutputImageFilter = castOutputImageT::New();
    castOutputImageFilter->SetInput(convolutionFilter->GetOutput());


    outputImage = castOutputImageFilter->GetOutput();

    if(show)
    {
        VTKViewer::visualize<outputImageT>(outputImage, "Laplacian of Gaussian");
    }



    IO::printOK("LoG filter");



}

template<typename inputImageT, typename outputImageT>
void LoGFilter<inputImageT, outputImageT>::createKernel(bool show)
{

    using regionT =  doubleImageT::RegionType;
    regionT::SizeType size;
    size.Fill(kernelSize);
    regionT::IndexType index;
    index.Fill(0);
    regionT kernelRegion(index, size);


    kernel = doubleImageT::New();
    kernel->SetRegions(kernelRegion);
    kernel->Allocate();
    kernel->FillBuffer(0);

    index.Fill(0);

    double logVal;
    double nLimit = -static_cast<double>(kernelSize/2);
    double pLimit =  static_cast<double>(kernelSize/2);

    for(double x = nLimit ; x <= pLimit; ++x)
    {

        for(double y = nLimit ; y <= pLimit; ++y)
        {
            //std::cout<<x<<" "<<y<<std::endl;
            logVal = Math::LoG(x, y, sigma);
            kernel->SetPixel(index, logVal);
            //std::cout<<logVal<<" ";
            index[1]+=1;
         }
        //std::cout<<std::endl;
        index[0]+=1;
        index[1] =0;

    }


    if(show)
    {
        VTKViewer::visualize<doubleImageT>(kernel, "Kernel");
    }


}
