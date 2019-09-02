#ifndef ROIEXTRACTOR_H
#define ROIEXTRACTOR_H

//itk includes
#include "itkImage.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkColormapFunction.h"
#include "itkRGBToLuminanceImageFilter.h"
#include <itkLabelObject.h>
#include <itkLabelMap.h>
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapOverlayImageFilter.h"
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelToRGBImageFilter.h>
#include <itkLabelImageToLabelMapFilter.h>


//local includes
#include "../util/math.h"
#include "../util/overlayrgbimagefilter.h"
#include "../util/labelmaptomultiplegrayimagesfilter.h"
#include "../util/customprint.h"


//for testing
#include "../util/vtkviewer.h"

/*
*/

class ROIExtractor
{
public:

    //suffix T = Type
    //syffix P = Pointer

    using pixelComponentT = unsigned int;

    // RGB type  alias
    using rgbPixelType = itk::RGBPixel<pixelComponentT>;
    using rgbImageType = itk::Image< rgbPixelType, 2 >;
    using rgbImageP  =   typename  rgbImageType::Pointer;

    //gray scale type alias
    using grayImageType = itk::Image< pixelComponentT, 2>;
    using grayImageP = typename grayImageType::Pointer;

    //label map type alias
    using labelObjectT = itk::LabelObject<pixelComponentT, 2>;
    using labelMapT    = itk::LabelMap<labelObjectT>;
    using labelMapP    = typename labelMapT::Pointer;

    //const values for otsu
    const pixelComponentT BACKGROUND=255;
    const pixelComponentT FOREGROUND=0;

    //setters
    void setImage(rgbImageP inputImage);
    void setKernelSize(short kernelSize);
    void setDensityThreshold(pixelComponentT threshold);

    labelMapP  getConnectedComponents() const;
    rgbImageP  getColorMap ()           const;
    grayImageP getGrayImage()           const;

    void computeDensity     (bool showResult = false);
    void densityToColorMap  (bool showResult = false);
    void blendColorMap      (bool showResult = false);
    void computeConnectedComponents(bool showResult = false);

    ROIExtractor();

private:

    rgbImageP  inputImage;
    rgbImageP  colorMapImage;
    grayImageP densityImage;
    grayImageP grayImage;
    labelMapP  connectedComponents;

    short      kernelSize;
    pixelComponentT densityThreshold;

    //local functionality
    grayImageP otsuThreshold();
    void divideDensityIntoHighAndLow(grayImageP &highDensity, grayImageP &lowDensity);
    void rgbToGrayImage();

};


#endif // ROIEXTRACTOR_H
