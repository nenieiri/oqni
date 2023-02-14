#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("OQNI: COM port reader");
    this->setWindowIcon();
}

MainWindow::~MainWindow()
{
    delete ui;
}

