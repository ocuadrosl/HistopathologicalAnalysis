#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <itkImage.h>
#include <itkRGBPixel.h>

#include <itkCovariantVector.h>


//local includes
#include "core/imagereader.h"
#include "core/cellsegmentator.h"
#include "util/vtkviewer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


    using pixelCompT = uint;

    // RGB type  alias
    using rgbPixelT = itk::RGBPixel<uint>;
    using rgbImageT = itk::Image   <rgbPixelT, 2 >;
    using rgbImageP = typename rgbImageT::Pointer;

    using grayImageT = itk::Image<uint, 2>;
    using graImageP = typename grayImageT::Pointer;


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpenImage_triggered();

private:

    Ui::MainWindow *ui;
    rgbImageP inputImage;


    void readImage( std::string imageName, const std::string& dirPath, const std::string& fileName);
    void cellSegmentation(const std::string& dirPath, const std::string& fileName);

    void readDirectory(const QStringList& fileNames);




};

#endif // MAINWINDOW_H
