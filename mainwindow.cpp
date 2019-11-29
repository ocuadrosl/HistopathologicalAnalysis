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



void MainWindow::cellSegmentation()
{


    //rgb to grayscale



    using cellSegmentatorT = CellSegmentator<rgbImageT>;
    std::unique_ptr<cellSegmentatorT>  cellSegmentator(new cellSegmentatorT());
    cellSegmentator->setImage(inputImage);
    //cellSegmentator->computeGradients();

    cellSegmentator->findCellNuclei();
    cellSegmentator->superPixels();



    //using vectorImageT =  itk::Image<itk::CovariantVector<float, 2>, 2>;
    //VTKViewer::visualizeVectorImage<rgbImageT, vectorImageT>(inputImage, cellSegmentator->getGradients());


}



void MainWindow::readImage(std::string fileName)
{

    using imageReaderT = ImageReader<rgbImageT>;
    std::unique_ptr<imageReaderT> reader(new imageReaderT());

    auto it = std::find(fileName.begin(), fileName.end(), '.');
    std::string fileType("", static_cast<uint>(fileName.end() - it));
    std::copy(it, fileName.end(), fileType.begin());


    if(fileType==".vsi")
    {

        //Replace white spaces with shell-style white spaces "\\ "
        fileName = std::regex_replace(fileName, std::regex("\\s+"), "\\ ");

        //TODO replace this directory for a local project dir
        std::string tmpFileName = "/home/oscar/src/HistopathologicalAnalysis/tmp/tmpImage.tiff";

        reader->readVSI(fileName, tmpFileName, 1);



    }
    else
    {
        reader->read(fileName);
    }

    inputImage = reader->getRGBImage();

    //TODO delete this
    //VTKViewer::visualize<rgbImageT>(inputImage);


    cellSegmentation();


}
