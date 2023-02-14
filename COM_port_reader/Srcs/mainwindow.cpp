#include "mainwindow.h"
#include "../ui_mainwindow.h"

#include <QApplication>
#include <QScreen>
    
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    QScreen *screen = QApplication::primaryScreen();
    QSize screenSize = screen->size();
    int screenWidth = screenSize.width();
    int screenHeight = screenSize.height();
    int windownWidth = 400;
    int windownHeight = 400;
    this->setGeometry((screenWidth - windownWidth) / 2, \
                      (screenHeight - windownHeight) / 2, \
                      windownWidth, windownHeight);
    
    this->setWindowTitle("OQNI: COM port reader");
    
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
}

MainWindow::~MainWindow()
{
    delete ui;
}

