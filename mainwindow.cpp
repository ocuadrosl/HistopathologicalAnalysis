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



void MainWindow::readDirectory(const QStringList& fileNames)
{
    std::string fileName, dirPath;
    for(int i=0; i < fileNames.size(); ++i)
    {
        fileName = fileNames[i].split("/").back().split(".").first().toStdString();
        auto pathAux = fileNames[i].split("/");
        pathAux.pop_back();
        dirPath = pathAux.join("/").toStdString();

        readImage(fileNames[i].toStdString(), dirPath, fileName);


    }

}


void MainWindow::on_actionOpenImage_triggered()
{


    QFileDialog qFileDialog(this);

    qFileDialog.setFileMode(QFileDialog::ExistingFiles);

    qFileDialog.setNameFilter(tr("Images (*.vsi *.tiff)"));

    //auto fileName = qFileDialog.getOpenFileName().toStdString();
    auto fileNames  = qFileDialog.getOpenFileNames();

    readDirectory(fileNames);


}



void MainWindow::cellSegmentation(const std::string& dirPath, const std::string& fileName)
{


    //rgb to grayscale

    using cellSegmentatorT = CellSegmentator<rgbImageT>;
    std::unique_ptr<cellSegmentatorT>  cellSegmentator(new cellSegmentatorT());
    cellSegmentator->setImage(inputImage);
    cellSegmentator->setNames(dirPath, fileName);

    cellSegmentator->findCells();

}



void MainWindow::PleuraDetection(const std::string& dirPath, const std::string& fileName)
{


    //rgb to grayscale

    using pleuraDetectorT = PleuraDetector<rgbImageT>;
    std::unique_ptr<pleuraDetectorT>  pleuraDetector(new pleuraDetectorT());
    pleuraDetector->SetInputImage(inputImage);
    pleuraDetector->Detect();
    //pleuraDetector->setNames(dirPath, fileName);

    //pleuraDetector->findCells();

}



void MainWindow::readImage( std::string imageName, const std::string& dirPath, const std::string& fileName)
{

    using imageReaderT = ImageReader<rgbImageT>;
    std::unique_ptr<imageReaderT> reader(new imageReaderT());

    auto it = std::find(imageName.begin(), imageName.end(), '.');
    std::string fileType("", static_cast<uint>(imageName.end() - it));
    std::copy(it, imageName.end(), fileType.begin());



    if(fileType==".vsi")
    {
        //Replace white spaces with shell-style white spaces "\\ "
        //imageName = std::regex_replace(imageName, std::regex("\\s+"), "\\ ");

        //TODO replace this directory for a local project dir
        std::string tmpFileName = "/home/oscar/src/HistopathologicalAnalysis/tmp/tmpImage.tiff";

        reader->readVSI(imageName, tmpFileName, 5);



    }
    else
    {
        reader->read(imageName);
    }

    inputImage = reader->getRGBImage();

    //TODO delete this
    //VTKViewer::visualize<rgbImageT>(inputImage);


     //cellSegmentation(dirPath, fileName);
     PleuraDetection(dirPath, fileName);


}
