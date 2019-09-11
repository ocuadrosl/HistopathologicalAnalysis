#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "itkImage.h"
#include "QuickView.h"

//local includes
#include "core/imagereader.h"
#include "core/roiextractor.h"
#include "util/vtkviewer.h"
#include "core/hestainfilter.h"
#include "util/labelmaptomultipleimagesfilter.h"

int main(/*int argc, char **argv*/)
{

    std::string inputFileName = "/home/oscar/data/biopsy/Dataset\\ 2/B\\ 2002\\ 381/FolderB\\ 2002\\ 381\\ A/FMRP_B\\ 2002\\ 381\\ A01.vsi";
    //std::string inputNoisyFile = "//home/oscar/data/Dataset\\ 1/B\\ 2017\\ 5479/FolderB\\ 2017\\ 5479\\ F/B\\ 2017\\ 5479\\ F.vsi";
    //std::string inFileName ="/home/oscar/data/biopsy/B2046-18\\ B20181107/Image01B2046-18\\ B.vsi";
    //std::string inFileName ="/home/oscar/data/biopsy/B2046-18\\ B20181107/Image01B2046-18\\ B.vsi";
    std::string outFileName = "/home/oscar/src/HistopathologicalAnalysis/output/tmpImage.tiff";
    //short magnification= 5;


    //usings


    //read image

    std::unique_ptr<ImageReader<>> reader(new ImageReader<>());

    reader->readVSI(inputFileName, outFileName, 5);
    //reader->read(outFileName);

    auto image = reader->getRGBImage();

    //VTKViewer<>::visualizeRGB(image, "Input Image RGB");


    //H&E color normalization
    std::unique_ptr<HEStainFilter> stainFilter(new HEStainFilter());
    stainFilter->setImage(image);
    stainFilter->colorEnhancement();


    //ROI extraction
    std::unique_ptr<ROIExtractor> roiExtractor(new ROIExtractor());

    roiExtractor->setImage(stainFilter->getOutput());
    roiExtractor->setDensityThreshold(70);
    roiExtractor->computeDensity();
    roiExtractor->densityToColorMap();
    roiExtractor->blendColorMap();
    roiExtractor->computeConnectedComponents();


    //writing ROIs
    using LabelMapToMultipleImagesFilterT = LabelMapToMultipleImagesFilter<ROIExtractor::rgbImageType, ROIExtractor::labelMapT>;

    std::unique_ptr<LabelMapToMultipleImagesFilterT> labelMapToImagesFilter(new LabelMapToMultipleImagesFilterT());
    labelMapToImagesFilter->setLabelMap(roiExtractor->getConnectedComponents());
    labelMapToImagesFilter->setImage(stainFilter->getOutput());
    labelMapToImagesFilter->extractImages();
    labelMapToImagesFilter->resizeImages(2);
    labelMapToImagesFilter->writeImages("/home/oscar/roi", "roi");

    return 0;


    /*
    QApplication app (argc, argv);

    MainWindow window;
    window.show();

    return app.exec();*/
}
