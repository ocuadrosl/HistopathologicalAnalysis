#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);




}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_actionOpenImage_triggered()
{


    QFileDialog qFileDialog(this);

    qFileDialog.setNameFilter(tr("Images (*.vsi *.tiff)"));

    auto fileName = qFileDialog.getOpenFileName().toStdString();
    //QMessageBox::information(this, "title", fileName);
    readImage(fileName);

}


void MainWindow::readImage(std::string fileName)
{

    //TODO verify images types and null strings...

    using imageReaderT = ImageReader<rgbImageT>;
    std::unique_ptr<imageReaderT> reader(new imageReaderT());

    //Replace white spaces with shell-style white spaces "\\ "
    fileName = std::regex_replace(fileName, std::regex("\\s+"), "\\ ");

    //TODO replace this directory for a local project dir
    std::string tmpFileName = "/home/oscar/src/HistopathologicalAnalysis/tmp/tmpImage.tiff";

    reader->readVSI(fileName, tmpFileName, 1);
    inputImage = reader->getRGBImage();


}
