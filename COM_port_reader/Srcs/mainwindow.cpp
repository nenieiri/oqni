#include "mainwindow.h"
    
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
    
    /* ------------- background, icon, and title ---------- */
    
    this->setWindowTitle("OQNI: COM port reader");
    
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    
    QString background = ":/Imgs/background.png";
    this->setStyleSheet("background-image: url(" + background + ");");
    
    /* ---------------------------------------------------- */
    
    /* ------------------- Button design ------------------ */
    
    this->_buttonCheck = new QPushButton("Check connected ports", this);
    this->_buttonCheck->setGeometry(20, 30, 180, 30);
    this->_buttonCheck->setCursor(Qt::PointingHandCursor);
    this->_buttonCheck->setStyleSheet("QPushButton {border-radius: 6px; \
                                      color: black; \
                                      border: 1px solid #0078D4; \
                                      background: #6FD5CA;} \
                                      QPushButton:hover {border-radius: 6px; \
                                      color: black; \
                                      border: 1px solid #0078D4; \
                                      background: #B9E8E2;}");

    connect(this->_buttonCheck, &QPushButton::pressed, this->_buttonCheck, [=]() {
        this->_buttonCheck->setStyleSheet("border-radius: 6px; \
                                          color: blue; \
                                          border: 1px solid #0078D4; \
                                          background: white;");
    });
    connect(this->_buttonCheck, &QPushButton::released, this->_buttonCheck, [=]() {
        this->_buttonCheck->setStyleSheet("border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid #0078D4; \
                                          background: #6FD5CA;");
    });
    connect(this->_buttonCheck, &QPushButton::clicked, this->_buttonCheck, [=]() {
        this->_buttonCheck->setStyleSheet("QPushButton {border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid #0078D4; \
                                          background: #6FD5CA;} \
                                          QPushButton:hover {border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid #0078D4; \
                                          background: #B9E8E2;}");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

