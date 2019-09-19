#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <itkImage.h>
#include <itkRGBPixel.h>

//local includes
#include "core/imagereader.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


    using pixelCompT = unsigned int;

    // RGB type  alias
    using rgbPixelT = itk::RGBPixel<pixelCompT   >;
    using rgbImageT = itk::Image   <rgbPixelT, 2 >;
    using rgbImageP = typename rgbImageT::Pointer;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpenImage_triggered();

private:

    Ui::MainWindow *ui;
    rgbImageP inputImage;

    void readImage(std::string fileName);




};

#endif // MAINWINDOW_H
