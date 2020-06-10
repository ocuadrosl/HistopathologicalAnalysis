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




}
