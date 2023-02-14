#include "mainwindow.h"
#include "../ui_mainwindow.h"

#include <QApplication>
#include <QScreen>
    
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    /* ------ put window to the center of the screen ------ */
    
    QScreen *screen = QApplication::primaryScreen();
    QSize screenSize = screen->size();
    int screenWidth = screenSize.width();
    int screenHeight = screenSize.height();
    int windownWidth = 500;
    int windownHeight = 440;
    this->setGeometry((screenWidth - windownWidth) / 2, \
                      (screenHeight - windownHeight) / 2, \
                      windownWidth, windownHeight);
    
    /* ---------------------------------------------------- */
    
    this->setWindowTitle("OQNI: COM port reader");
    
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    
    QString background = ":/Imgs/background.png";
    this->setStyleSheet("background-color: #CCD6DD; background-image: url(" + background + "); background-repeat: no-repeat; background-position: center;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

