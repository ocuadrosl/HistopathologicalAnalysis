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

int main(/*int argc, char **argv*/)
{

    std::string inFileName = "/home/oscar/data/biopsy/Dataset\\ 1/B\\ 2009\\ 8854/B\\ 2009\\ 8854\\ A.vsi";
    std::string inputNoisyFile = "/home/oscar/data/biopsy/Dataset\\ 1/B\\ 2017\\ 5479/FolderB\\ 2017\\ 5479\\ F/B\\ 2017\\ 5479\\ F.vsi";
    //std::string inFileName ="/home/oscar/data/biopsy/B2046-18\\ B20181107/Image01B2046-18\\ B.vsi";
    //std::string inFileName ="/home/oscar/data/biopsy/B2046-18\\ B20181107/Image01B2046-18\\ B.vsi";
    std::string outFileName = "/home/oscar/src/HistopathologicalAnalysis/output/tmpImage.tiff";
    //short magnification= 5;


    //usings


    //read image

    std::unique_ptr<ImageReader<>> reader(new ImageReader<>());

    //reader->readVSI(inputNoisyFile, outFileName, 5);
    reader->read(outFileName);

    auto image = reader->getRGBImage();

    //VTKViewer<>::visualizeRGB(image, "Input Image RGB");


    //H&E color normalization

    std::unique_ptr<HEStainFilter<>> stainFilter(new HEStainFilter<>());
    stainFilter->setImage(image);
    stainFilter->denoise();


    //ROI extraction
    std::unique_ptr<ROIExtractor<>> roiExtractor(new ROIExtractor<>());
    roiExtractor->setImage(stainFilter->getOutput());
    roiExtractor->setDensityThreshold(70);
    roiExtractor->computeDensity();
    roiExtractor->densityToColorMap();
    roiExtractor->blendColorMap(true);
    roiExtractor->computeConnectedComponents();

    //writing ROIs
    std::unique_ptr<LabelMapToMultipleGrayImagesFilter<>> labelMapToImagesFilter(new LabelMapToMultipleGrayImagesFilter<>());
    labelMapToImagesFilter->setLabelMap(roiExtractor->getConnectedComponents());
    labelMapToImagesFilter->setGrayImage(roiExtractor->getGrayImage());
    labelMapToImagesFilter->extractSubImages();
    labelMapToImagesFilter->resizeSubImages(2);
    labelMapToImagesFilter->writeSubImages("/home/oscar/roi", "roi");

    return 0;


    /*
    QApplication app (argc, argv);

    MainWindow window;
    window.show();

    return app.exec();*/
}
