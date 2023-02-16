#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _portCount(0)
{
    ui->setupUi(this);
    
    this->putWindowOnScreen(500, 440);
    this->_buttonCheck = this->createButton("Check connected ports", 20, 30, 180, 30, std::bind(&MainWindow::updateCheckbox, this));
    this->addAnimation(this->_buttonCheck, 21, 90, 170, 170);
    this->createGroupBox();


    /* ----------- adding Vertical ScrollBar -------------- */
    
    this->_liftVertical = new QScrollBar(Qt::Vertical, this);
    this->_liftVertical->setGeometry(179, 71, 20, 298);
    this->_liftVertical->hide();
    
    connect(this->_liftVertical, &QScrollBar::valueChanged, this->_groupBox, [=]() {
        int liftRatio;

        for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        {
            liftRatio = 30 * (1 + (it - _comPorts.begin()) - this->_liftVertical->value());
            (*it)->getCheckBox()->setGeometry(10, liftRatio, 155, 20);
            (*it)->getCheckBox()->raise();
            if (liftRatio >= 30)
                (*it)->getCheckBox()->show();
            else
                (*it)->getCheckBox()->hide();
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

void    MainWindow::putWindowOnScreen(int windowWidth, int windowHeight)
{
    /* ------ put window to the center of the screen ------ */
    QScreen *screen = QApplication::primaryScreen();
    QSize screenSize = screen->size();
    int screenWidth = screenSize.width();
    int screenHeight = screenSize.height();
    this->setGeometry((screenWidth - windowWidth) / 2, \
                      (screenHeight - windowHeight) / 2, \
                      windowWidth, windowHeight);

    /* ------------- background, icon, and title ---------- */
    this->setWindowTitle("OQNI: COM port reader");
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    QString background = ":/Imgs/background.png";
    this->setStyleSheet("background-image: url(" + background + ");");
}

QPushButton    *MainWindow::createButton(const QString &name, int x, int y, int width, int height, std::function<void(void)> onPressAction)
{
    QPushButton *button;
   
    /* ------------------- Button design ------------------ */
    button = new QPushButton(name, this);
    button->setGeometry(x, y, width, height);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet("QPushButton {border-radius: 6px; \
                              color: black; \
                              border: 1px solid gray; \
                              background: #6FD5CA;} \
                              QPushButton:hover {border-radius: 6px; \
                              color: black; \
                              border: 1px solid #0078D4; \
                              background: #B9E8E2;}");
    connect(button, &QPushButton::released, button,
        [=](void)
        {
            button->setStyleSheet("QPushButton {border-radius: 6px; \
                                      color: black; \
                                      border: 1px solid #0078D4; \
                                      background: #6FD5CA;} \
                                      QPushButton:hover {border-radius: 6px; \
                                      color: black; \
                                      border: 1px solid #0078D4; \
                                      background: #B9E8E2;}");
        });
    connect(button, &QPushButton::clicked, button,
        [=](void)
        {
            button->setStyleSheet("QPushButton {border-radius: 6px; \
                                      color: black; \
                                      border: 1px solid gray; \
                                      background: #6FD5CA;} \
                                      QPushButton:hover {border-radius: 6px; \
                                      color: black; \
                                      border: 1px solid #0078D4; \
                                      background: #B9E8E2;}");
        });
    connect(button, &QPushButton::pressed, button,
        [=](void)
        {
            button->setStyleSheet("border-radius: 6px; \
                                      color: blue; \
                                      border: 1px solid #0078D4; \
                                      background: white;");
            onPressAction();
        });

    return (button);
}
    
void    MainWindow::updateCheckbox(void)
{
    this->_gifLabel->show();
    this->_gifMovie->start();
    this->_liftVertical->hide();
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        (*it)->getCheckBox()->hide();
    QTimer::singleShot(3000, this->_gifLabel, &QLabel::hide);
    this->_portCount = 30;
    QTimer::singleShot(3000, this, &MainWindow::createCheckBox);
}
    
void    MainWindow::addAnimation(QPushButton *button, int x, int y, int width, int height)
{
    /* ---------------- Button functional ----------------- */
    this->_gifLabel = new QLabel(this);
    this->_gifLabel->setGeometry(x, y, width, height);
    this->_gifLabel->stackUnder(button);
    this->_gifLabel->hide();
    this->_gifMovie = new QMovie(":/Imgs/loading.gif");
    this->_gifMovie->setScaledSize(this->_gifLabel->size());
    this->_gifLabel->setMovie(this->_gifMovie);
    this->_gifLabel->setStyleSheet("background: #e6e6e6;");
}

void    MainWindow::createGroupBox()
{
    /* ---------------- adding GroupBox ------------------- */
    this->_groupBox = new QGroupBox("Connected COM ports", this);
    this->_groupBox->setGeometry(20, 70, 180, 300);
    this->_groupBox->stackUnder(this->_gifLabel);
    this->_groupBox->setStyleSheet("border: 1px solid gray; background: #e6e6e6;");
}

void    MainWindow::createCheckBox()
{
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        delete (*it);
    this->_comPorts.clear();
        
    for (int i = 0; i < this->_portCount; ++i)
        this->_comPorts.push_back(new ComPort("COM" + QString::number(i + 1), this->_groupBox));
    if (this->_portCount > 9)
    {
        this->_liftVertical->show();
        this->_liftVertical->setMinimum(0);
        this->_liftVertical->setMaximum(this->_portCount - 9);
    }
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
    {
        (*it)->getCheckBox()->setGeometry(10, 30 + 30 * (it - _comPorts.begin()), 155, 20);
        (*it)->getCheckBox()->raise();
        (*it)->getCheckBox()->show();
        (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");
    }
}
