#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "core/imagereader.h"
#include "itkImage.h"
#include "QuickView.h"


int main(/*int argc, char **argv*/)
{

    //std::string inFileName = "/home/oscar/data/biopsy/Dataset\\ 1/B\\ 2009\\ 8854/B\\ 2009\\ 8854\\ A.vsi";
    std::string inFileName ="/home/oscar/data/biopsy/B2046-18\\ B20181107/Image01B2046-18\\ B.vsi";
    std::string outFileName = "tmpImage.tiff";
    short magnification= 5;

    ImageReader reader;

    using  imageType = ImageReader::imageType;
    imageType::Pointer image;

    //image = reader.readVSI(inFileName, outFileName, magnification);

    image = reader.read(outFileName);

    QuickView viewer;
    viewer.AddImage(image.GetPointer());
    viewer.Visualize();


    return 0;
    /*
    QApplication app (argc, argv);

    MainWindow window;
    window.show();

    return app.exec();*/
}
