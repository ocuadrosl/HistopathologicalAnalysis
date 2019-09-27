#include "logfilter.h"
template<typename imageT>
LoGFilter<imageT>::LoGFilter()
{

}

template<typename imageT>
void LoGFilter<imageT>::setImage(imageP inputImage)
{
    this->inputImage = inputImage;
}

template<typename imageT>
void LoGFilter<imageT>::compute()
{

    outputImage = imageT::New();
    outputImage->SetRegions(inputImage->GetRequestedRegion());
    outputImage->Allocate();


    createKernel();

    IO::printOK("LoG filter");



}

template<typename imageT>
void LoGFilter<imageT>::createKernel()
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
            std::cout<<logVal<<" ";
            index[1]+=1;
         }
        std::cout<<std::endl;
        index[0]+=1;
        index[1] =0;

    }



    //VTKViewer::visualize<doubleImageT>(kernel, "Kernel");



}
