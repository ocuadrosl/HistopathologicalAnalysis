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

#include "core/pleuradetector.h"

#include <QSurfaceFormat>
//#include <QVTKOpenGLNativeWidget.h>


int main(/*int argc, char **argv*/)
{




    /* QApplication app (argc, argv);

    //uncomment this to use the graphical interface

    MainWindow window;
    window.show();

    return app.exec();
    */



    using rgbPixelT = itk::RGBPixel<uint>;
    using rgbImageT = itk::Image   <rgbPixelT, 2 >;
    using rgbImageP = typename rgbImageT::Pointer;


    std::string imageDir  = "/home/oscar/data/biopsy/tiff/test/images";
    std::string imageName = "B 2009 8854 A_1x";
    std::string imageType = ".tiff";



    using imageReaderT = ImageReader<rgbImageT>;
    std::unique_ptr<imageReaderT> imageReader(new imageReaderT());

    imageReader->read(imageDir+"/"+imageName+imageType);


    //Reading Labels
    using imageReaderT = ImageReader<rgbImageT>;
    std::unique_ptr<imageReaderT> labelReader(new imageReaderT());

    labelReader->read("/home/oscar/data/biopsy/tiff/test/labels/"+imageName+imageType);



    using pleuraDetectorT = PleuraDetector<rgbImageT>;
    std::unique_ptr<pleuraDetectorT>  pleuraDetector(new pleuraDetectorT());
    pleuraDetector->SetInputImage(imageReader->getRGBImage());
    pleuraDetector->SetImageName(imageDir, imageName);
    pleuraDetector->SetLabelImage(labelReader->getRGBImage());
    pleuraDetector->SetCSVFileName("/home/oscar/data/biopsy/tiff/test/csv/"+imageName+".csv");
    pleuraDetector->Detect();


    return 0;



}
