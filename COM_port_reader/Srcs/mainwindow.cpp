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
                                      border: 1px solid gray; \
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
        this->_gifLabel->show();
        this->_gifMovie->start();
        QTimer::singleShot(3000, this->_gifLabel, &QLabel::hide);
    });
    connect(this->_buttonCheck, &QPushButton::released, this->_buttonCheck, [=]() {
        this->_buttonCheck->setStyleSheet("QPushButton {border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid #0078D4; \
                                          background: #6FD5CA;} \
                                          QPushButton:hover {border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid #0078D4; \
                                          background: #B9E8E2;}");
    });
    connect(this->_buttonCheck, &QPushButton::clicked, this->_buttonCheck, [=]() {
        this->_buttonCheck->setStyleSheet("QPushButton {border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid gray; \
                                          background: #6FD5CA;} \
                                          QPushButton:hover {border-radius: 6px; \
                                          color: black; \
                                          border: 1px solid #0078D4; \
                                          background: #B9E8E2;}");
    });
    
    /* ---------------------------------------------------- */
    
    /* ---------------- Button functional ----------------- */
    
    this->_gifLabel = new QLabel(this);
    this->_gifLabel->setGeometry(21, 90, 170, 170);
    this->_gifLabel->stackUnder(this->_buttonCheck);
    this->_gifLabel->hide();
    
    this->_gifMovie = new QMovie(":/Imgs/loading.gif");
    this->_gifMovie->setScaledSize(this->_gifLabel->size());
    
    this->_gifLabel->setMovie(this->_gifMovie);
    this->_gifLabel->setStyleSheet("background: #e6e6e6;");

    /* ---------------------------------------------------- */

    /* ---------------- adding GroupBox ------------------- */

    this->_groupBox = new QGroupBox("Connected COM ports", this);
    this->_groupBox->setGeometry(20, 70, 180, 300);
    this->_groupBox->stackUnder(this->_gifLabel);
    this->_groupBox->setStyleSheet("border: 1px solid gray; background: #e6e6e6;");
    
}

MainWindow::~MainWindow()
{
    delete _buttonCheck;
    delete _gifLabel;
    delete _gifMovie;
    delete ui;
}

