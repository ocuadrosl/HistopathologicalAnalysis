#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "core/imagereader.h"
#include "itkImage.h"
#include "QuickView.h"
#include "core/roiextractor.h"
#include "util/vtkviewer.h"

int main(/*int argc, char **argv*/)
{

    std::string inFileName = "/home/oscar/data/biopsy/Dataset\\ 1/B\\ 2009\\ 8854/B\\ 2009\\ 8854\\ A.vsi";
    //std::string inFileName ="/home/oscar/data/biopsy/B2046-18\\ B20181107/Image01B2046-18\\ B.vsi";
    std::string outFileName = "tmpImage.tiff";
    short magnification= 5;


    //usings


    //read image

    std::unique_ptr<ImageReader<>> reader(new ImageReader<>());

    //auto image = reader->readVSI(inFileName, outFileName, magnification);

    reader->read(outFileName);
    auto image = reader->getRGBImage();

   VTKViewer<>::visualizeRGB(image, "Input Image RGB");

    //ROI extraction
    std::unique_ptr<ROIExtractor<>> roiExtractor(new ROIExtractor<>());
    roiExtractor->setImage(image);
    roiExtractor->extract();



    return 0;
    /*
    QApplication app (argc, argv);

    MainWindow window;
    window.show();

    return app.exec();*/
}
