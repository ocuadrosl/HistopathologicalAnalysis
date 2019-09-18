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

    qFileDialog.setNameFilter(tr("Imnages (*.vsi)"));

    QStringList fileName;
    if(qFileDialog.exec())
    {
        fileName = qFileDialog.selectedFiles();
    }

    //auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files (*.vsi)"));

    QMessageBox::information(this, "title", fileName[0]);

}
