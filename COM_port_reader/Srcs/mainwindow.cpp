#include "mainwindow.h"
#include "../ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("OQNI: COM port reader");
    
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
}

MainWindow::~MainWindow()
{
    delete ui;
}

