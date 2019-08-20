#include <iostream>
#include <QApplication>

#include "mainwindow.h"
#include "core/imagereader.h"

using namespace std;

int main(/*int argc, char **argv*/)
{

    std::string inFileName = "/home/oscar/data/biopsy/Dataset\\ 1/B\\ 2009\\ 8854/B\\ 2009\\ 8854\\ A.vsi";
    std::string outFileName = "/home/oscar/image.tiff";
    short magnification= 5;

    ImageReader reader;

    reader.readVSI(inFileName, outFileName, magnification);




    return 0;
    /*
    QApplication app (argc, argv);

    MainWindow window;
    window.show();

    return app.exec();*/
}
