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
void LoGFilter<inputImageT, outputImageT>::setSigma(float sigma)
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
void LoGFilter<inputImageT, outputImageT>::compute(bool show, bool echo)
{

    createKernel(1);

    using castInputImageT = itk::CastImageFilter<inputImageT, imageFloatT>;
    typename castInputImageT::Pointer castInputImageFilter = castInputImageT::New();
    castInputImageFilter->SetInput(inputImage);
    castInputImageFilter->Update();


    using convolutionFilterT = itk::ConvolutionImageFilter<imageFloatT>;
    typename convolutionFilterT::Pointer convolutionFilter = convolutionFilterT::New();
    convolutionFilter->SetInput(castInputImageFilter->GetOutput());
    convolutionFilter->SetKernelImage(kernel);
    convolutionFilter->Update();

    //TODO if constexpr to avoid unnecessary image cast

    using castOutputImageT = itk::CastImageFilter<imageFloatT, outputImageT>;
    typename castOutputImageT::Pointer castOutputImageFilter = castOutputImageT::New();
    castOutputImageFilter->SetInput(convolutionFilter->GetOutput());
    castOutputImageFilter->Update();


    outputImage = castOutputImageFilter->GetOutput();

    if(show)
    {
        VTKViewer::visualize<outputImageT>(outputImage, "Laplacian of Gaussian");
    }

    if(echo)
    {
        IO::printOK("LoG filter");
    }



}


template<typename inputImageT, typename outputImageT>
void LoGFilter<inputImageT, outputImageT>::setKernelSize(unsigned kernelSize)
{

    this->kernelSize = kernelSize;

}

template<typename inputImageT, typename outputImageT>
void LoGFilter<inputImageT, outputImageT>::createKernel(bool show)
{

    using regionT =  imageFloatT::RegionType;
    regionT::SizeType size;
    size.Fill(kernelSize);
    regionT::IndexType index;
    index.Fill(0);
    regionT kernelRegion(index, size);


    kernel = imageFloatT::New();
    kernel->SetRegions(kernelRegion);
    kernel->Allocate();
    kernel->FillBuffer(0);

    index.Fill(0);

    float logVal;
    float nLimit = -static_cast<float>(kernelSize/2);
    float pLimit =  static_cast<float>(kernelSize/2);

    for(float x = nLimit ; x <= pLimit; ++x)
    {

        for(float y = nLimit ; y <= pLimit; ++y)
        {
            //std::cout<<x<<" "<<y<<std::endl;
            logVal = Math::LoG<float>(x, y, sigma);
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
        VTKViewer::visualize<imageFloatT>(kernel, "Kernel");
    }


}
