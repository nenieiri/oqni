#include "mainwindow.h"
    
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    this->_portCount = 0;
    
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
        this->_liftVertical->hide();
        for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
            (*it)->getCheckBox()->hide();
        QTimer::singleShot(3000, this->_gifLabel, &QLabel::hide);
        this->_portCount = 30;
        QTimer::singleShot(3000, this, &MainWindow::createCheckBox);
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
    
    /* ---------------------------------------------------- */

    /* ----------- adding Vertical ScrollBar -------------- */
    
    this->_liftRatio = 0;
    this->_liftVertical = new QScrollBar(Qt::Vertical, this);
    this->_liftVertical->setGeometry(179, 71, 20, 298);
    this->_liftVertical->hide();
    
    connect(this->_liftVertical, &QScrollBar::valueChanged, this->_groupBox, [=]() {
        this->_liftRatio = 30 * this->_liftVertical->value();
        for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        {
            (*it)->getCheckBox()->setGeometry(10, 30 + 30 * (it - _comPorts.begin()) - _liftRatio, 155, 20);
            (*it)->getCheckBox()->raise();
            (*it)->getCheckBox()->show();
            (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");
        }
    });
}

MainWindow::~MainWindow()
{
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        delete *it;
    this->_comPorts.clear();
    delete _buttonCheck;
    delete _gifLabel;
    delete _gifMovie;
    delete _liftVertical;
    delete ui;
}

void    MainWindow::createCheckBox()
{
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        delete (*it);
    this->_comPorts.clear();
        
    for (int i = 0; i < this->_portCount; ++i)
        this->_comPorts.push_back(new ComPort("COM" + QString::number(i), this->_groupBox));
    if (this->_portCount > 9)
    {
        this->_liftVertical->show();
        this->_liftVertical->setMinimum(0);
        this->_liftVertical->setMaximum(this->_portCount - 9);
    }
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
    {
        (*it)->getCheckBox()->setGeometry(10, 30 + 30 * (it - _comPorts.begin()) - _liftRatio, 155, 20);
        (*it)->getCheckBox()->raise();
        (*it)->getCheckBox()->show();
        (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");
    }
}
