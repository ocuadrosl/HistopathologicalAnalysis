#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include <itkImage.h>
#include <itkRGBPixel.h>




//local includes
#include "core/imagereader.h"
#include "core/roiextractor.h"
#include "util/vtkviewer.h"
#include "core/hestainfilter.h"
#include "util/labelmaptomultipleimagesfilter.h"
#include "core/cellsegmentator.h"

#include <QSurfaceFormat>
//#include <QVTKOpenGLNativeWidget.h>


int main(int argc, char **argv)
{

    QApplication app (argc, argv);

    MainWindow window;
    window.show();


     return app.exec();


    //at work
    //std::string inputFileName = "/home/oscar/data/biopsy/Dataset\\ 1/B\\ 2009\\ 8854/B\\ 2009\\ 8854\\ A.vsi";

    //at home
    std::string inputFileName ="/home/oscar/data/biopsy/B526-18\\ \\ B\\ 20181107/Image01B526-18\\ \\ B\\ .vsi";

    std::string outFileName = "/home/oscar/src/HistopathologicalAnalysis/tmp/tmpImage.tiff";


    using rgbImageT = itk::Image<itk::RGBPixel<unsigned>, 2>;

    //reading image
    using imageReaderT = ImageReader<rgbImageT>;
    std::unique_ptr<imageReaderT> reader(new imageReaderT());

    //reader->readVSI(inputFileName, outFileName, 1);
    reader->read(outFileName);

    auto image = reader->getRGBImage();

    //VTKViewer<>::visualize(image , "Input Image");

    //H&E color normalization
    std::unique_ptr<HEStainFilter> stainFilter(new HEStainFilter());
    stainFilter->setImage(image);
    stainFilter->colorEnhancement();


    using grayImageT = itk::Image<unsigned,2>;
    using cellSegmentatorT = CellSegmentator<grayImageT>;
    std::unique_ptr<cellSegmentatorT>  cellSegmentator(new cellSegmentatorT());
    cellSegmentator->setImage(reader->getGrayScaleImage());
    cellSegmentator->computeGradients();



   // return 0;


    //ROI extraction
    std::unique_ptr<ROIExtractor> roiExtractor(new ROIExtractor());

    roiExtractor->setImage(stainFilter->getOutput());
    roiExtractor->setDensityThreshold(70);
    roiExtractor->computeDensity();
    roiExtractor->densityToColorMap();
    roiExtractor->blendColorMap();
    roiExtractor->computeConnectedComponents();

/*



    return 0;

    //writing ROIs
    using LabelMapToMultipleImagesFilterT = LabelMapToMultipleImagesFilter<ROIExtractor::rgbImageType, ROIExtractor::labelMapT>;

    std::unique_ptr<LabelMapToMultipleImagesFilterT> labelMapToImagesFilter(new LabelMapToMultipleImagesFilterT());
    labelMapToImagesFilter->setLabelMap(roiExtractor->getConnectedComponents());
    labelMapToImagesFilter->setImage(stainFilter->getOutput());
    labelMapToImagesFilter->extractImages();
    labelMapToImagesFilter->resizeImages(2);
    labelMapToImagesFilter->writeImages("/home/oscar/src/HistopathologicalAnalysis/output/roi", "roi");

    return 0;
*/





}
