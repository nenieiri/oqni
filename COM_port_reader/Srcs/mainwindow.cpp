#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _portCount(0)
    , _selectedComPort(nullptr)
{
    ui->setupUi(this);
    
    this->putWindowOnScreen(700, 616);
    this->_buttonCheck = this->createButton("Check connected ports", 20, 30, 380, 30, std::bind(&MainWindow::buttonCheckAction, this), this);
    this->addLoadingAnimation(this->_buttonCheck, 21, 150, 370, 370);
    this->createGroupBox(20, 70, 380, 515);
    this->createLiftVertical(379, 71, 20, 513);
    this->_buttonNext = this->createButton("Next", 560, 555, 100, 30, std::bind(&MainWindow::buttonNextAction, this), this);
    
    _baudRateItems = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    _dataBitsItems = {"5", "6", "7", "8"};
    _parityItems = {"None", "Even", "Odd", "Space", "Mark"};
    _stopBitsItems = {"1", "1.5", "2"};
    _flowControlItems = {"None", "Hardware", "Xon / Xoff"};
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
    this->setMinimumSize(windowWidth, windowHeight);
    this->setMaximumSize(windowWidth, windowHeight);

    /* ------------- background, icon, and title ---------- */
    this->setWindowTitle("OQNI: COM port reader");
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    this->setStyleSheet("background-image: url(:/Imgs/background.png); font-size: 20px");
}

QPushButton    *MainWindow::createButton(const QString &name, int x, int y, int width, \
                                         int height, std::function<void(void)> onPressAction, QWidget *box)
{
    QPushButton *button;
   
    /* ------------------- Button design ------------------ */
    button = new QPushButton(name, box);
    button->setGeometry(x, y, width, height);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
    connect(button, &QPushButton::released, button,
        [=](void)
        {
            button->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        });
    connect(button, &QPushButton::clicked, button,
        [=](void)
        {
            button->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
        });
    connect(button, &QPushButton::pressed, button,
        [=](void)
        {
            button->setStyleSheet(MY_DEFINED_PRESSED_BUTTON);
            if (onPressAction != nullptr)
                onPressAction();
        });

    return (button);
}
    
void    MainWindow::addLoadingAnimation(QPushButton *button, int x, int y, int width, int height)
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

void    MainWindow::createGroupBox(int x, int y, int width, int height)
{
    /* ---------------- adding GroupBox ------------------- */
    this->_groupBox = new QGroupBox("Connected COM ports:", this);
    this->_groupBox->setGeometry(x, y, width, height);
    this->_groupBox->stackUnder(this->_gifLabel);
    this->_groupBox->setStyleSheet("border: 1px solid gray; background: #e6e6e6;");
}

void    MainWindow::createLiftVertical(int x, int y, int width, int height)
{
    /* ----------- adding Vertical ScrollBar -------------- */
    this->_liftVertical = new QScrollBar(Qt::Vertical, this);
    this->_liftVertical->setGeometry(x, y, width, height);
    this->_liftVertical->hide();
    connect(this->_liftVertical, &QScrollBar::valueChanged, this->_groupBox,
        [=](void)
        {
            int liftRatio;
        
            for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
            {
                liftRatio = 40 * (1 + (it - _comPorts.begin()) - this->_liftVertical->value());
                (*it)->getCheckBox()->setGeometry(40, liftRatio, 310, 24);
                (*it)->getCheckBox()->raise();

                (*it)->getToolButton()->setGeometry(5, liftRatio - 5, 30, 30);
                (*it)->getToolButton()->raise();
                if (liftRatio >= 40)
                {
                    (*it)->getCheckBox()->show();
                    if ((*it)->getCheckBox()->isChecked() == true )
                    (*it)->getToolButton()->show();
                }
                else
                {
                    (*it)->getCheckBox()->hide();
                    (*it)->getToolButton()->hide();
                }
                (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");
            }
        });
}

void    MainWindow::buttonCheckAction(void)
{
    /* ----------- show animation and update checkboxes' list -------------- */

    this->_previewsCheckBox = nullptr;
    this->_gifLabel->show();
    this->_gifMovie->start();
    this->_liftVertical->hide();
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
    {
        (*it)->getCheckBox()->hide();
        (*it)->getToolButton()->hide();
    }
    QTimer::singleShot(1000, this->_gifLabel, &QLabel::hide);
    QTimer::singleShot(1000, this,
        [=](void)
        {
            for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
                delete (*it);
            this->_comPorts.clear();
                
            QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
            for (const QSerialPortInfo& port : portList)
                this->_comPorts.push_back(new ComPort(port, this->_groupBox));

            this->_portCount = this->_comPorts.size();
            if (this->_portCount > 12)
            {
                this->_liftVertical->setValue(0);
                this->_liftVertical->show();
                this->_liftVertical->setMinimum(0);
                this->_liftVertical->setMaximum(this->_portCount - 12);
            }
            for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
            {
                (*it)->getCheckBox()->setGeometry(40, 40 * (1 + (it - _comPorts.begin())), 310, 24);
                (*it)->getCheckBox()->raise();
                (*it)->getCheckBox()->show();
                (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");

                (*it)->getToolButton()->setGeometry(5, 40 * (1 + (it - _comPorts.begin())) - 5, 30, 30);

                connect((*it)->getCheckBox(), &QRadioButton::clicked, (*it)->getToolButton(),
                    [=](void)
                    {
                        (*it)->getToolButton()->raise();
                        (*it)->getToolButton()->show();
                        if (this->_previewsCheckBox && this->_previewsCheckBox != *it)
                            this->_previewsCheckBox->getToolButton()->hide();
                        this->_previewsCheckBox = *it;
                    });
                if ((*it)->getCheckBox()->isChecked() == false)
                    (*it)->getToolButton()->hide();
                
                connect((*it)->getToolButton(), &QToolButton::clicked, this,
					[=](void)
					{
						this->buttonToolAction(*it);
					});
            }
        });
}

void    MainWindow::buttonNextAction()
{
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it != _comPorts.end(); ++it)
    {
        if ((*it)->getCheckBox()->isChecked() == true )
        {
            this->_selectedComPort = *it;
            break ;
        }
    }
    if (this->_selectedComPort == nullptr)
    {
        delete this->_windowNext;
        return ;
    }
    
    this->_windowNext = new WindowNext(this);
    
    this->_windowNext->setButtonBrowse(createButton("Browse", 490, 50, 100, 30, nullptr, this->_windowNext));
    this->_windowNext->setButtonStart(createButton("Start", 140, 260, 100, 30, nullptr, this->_windowNext));
    this->_windowNext->setButtonStop(createButton("Stop", 250, 260, 100, 30, nullptr, this->_windowNext));
    this->_windowNext->setButtonClose(createButton("Close", 360, 260, 100, 30, nullptr, this->_windowNext));
    
    this->_windowNext->exec();
    this->_buttonNext->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
    delete this->_windowNext;
}

void    MainWindow::buttonToolAction(ComPort *comPort)
{
    comPort->_windowProperty = new QDialog(this);
    comPort->_windowProperty->setModal(true);

    comPort->_windowProperty->setMinimumSize(360, 300);
    comPort->_windowProperty->setMaximumSize(360, 300);
    comPort->_windowProperty->setWindowTitle("Properties");
    comPort->_windowProperty->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    comPort->_windowProperty->setWindowFilePath(":/Imgs/oqni.ico");
    comPort->_windowProperty->setStyleSheet("background: #e6e6e6;");
    
    QLabel *portName = new QLabel("Port name:         " + comPort->getPortName(), comPort->_windowProperty);
    portName->setGeometry(10, 10, 430, 30);
    QLabel *baudRate = new QLabel("Baud Rate:" , comPort->_windowProperty);
    baudRate->setGeometry(10, 50, 130, 30);
    QLabel *dataBits = new QLabel("Data Bits:", comPort->_windowProperty);
    dataBits->setGeometry(10, 90, 130, 30);
    QLabel *parity = new QLabel("Parity:", comPort->_windowProperty);
    parity->setGeometry(10, 130, 130, 30);
    QLabel *stopBits = new QLabel("Stop Bits:", comPort->_windowProperty);
    stopBits->setGeometry(10, 170, 130, 30);
    QLabel *flowControl = new QLabel("Flow Control:", comPort->_windowProperty);
    flowControl->setGeometry(10, 210, 130, 30);
    
    QComboBox *baudComboBox = new QComboBox(comPort->_windowProperty);
    baudComboBox->addItems(this->_baudRateItems);
    baudComboBox->setGeometry(150, 50, 200, 30);
    
    QComboBox *dataComboBox = new QComboBox(comPort->_windowProperty);
    dataComboBox->addItems(this->_dataBitsItems);
    dataComboBox->setGeometry(150, 90, 200, 30);
    
    QComboBox *parityComboBox = new QComboBox(comPort->_windowProperty);
    parityComboBox->addItems(this->_parityItems);
    parityComboBox->setGeometry(150, 130, 200, 30);
    
    QComboBox *stopComboBox = new QComboBox(comPort->_windowProperty);
    stopComboBox->addItems(this->_stopBitsItems);
    stopComboBox->setGeometry(150, 170, 200, 30);
    
    QComboBox *flowComboBox = new QComboBox(comPort->_windowProperty);
    flowComboBox->addItems(this->_flowControlItems);
    flowComboBox->setGeometry(150, 210, 200, 30);
    
    comPort->_cancelProperties = this->createButton("Cancel", 10, 255, 100, 30, nullptr, comPort->_windowProperty);
    comPort->_setDefaultProperties = this->createButton("Default", 130, 255, 100, 30, nullptr, comPort->_windowProperty);
    comPort->_saveProperies = this->createButton("Save", 250, 255, 100, 30, nullptr, comPort->_windowProperty);

    baudComboBox->setCurrentIndex(comPort->getBaudRateIndex());
    dataComboBox->setCurrentIndex(comPort->getDataBitsIndex());
    parityComboBox->setCurrentIndex(comPort->getParityIndex());
    stopComboBox->setCurrentIndex(comPort->getStopBitsIndex());
    flowComboBox->setCurrentIndex(comPort->getFlowControlIndex());
    
    connect(comPort->_cancelProperties, &QPushButton::clicked, comPort->_windowProperty,
		[=](void)
		{
            comPort->_windowProperty->close();
		});
    connect(comPort->_setDefaultProperties, &QPushButton::clicked, comPort->_windowProperty,
		[=](void)
		{
            baudComboBox->setCurrentIndex(7);
            dataComboBox->setCurrentIndex(3);
            parityComboBox->setCurrentIndex(0);
            stopComboBox->setCurrentIndex(0);
            flowComboBox->setCurrentIndex(0);
		});
    connect(comPort->_saveProperies, &QPushButton::clicked, comPort->_windowProperty,
        [=](void)
        {
            comPort->setBaudRate(baudComboBox->currentText(), this->_baudRateItems);
            comPort->setDataBits(dataComboBox->currentText(), this->_dataBitsItems);
            comPort->setParity(parityComboBox->currentText(), this->_parityItems);
            comPort->setStopBits(stopComboBox->currentText(), this->_stopBitsItems);
            comPort->setFlowControl(flowComboBox->currentText(), this->_flowControlItems);
            comPort->_windowProperty->close();
        });

    comPort->_windowProperty->exec();
    delete portName;
    delete baudRate;
    delete dataBits;
    delete parity;
    delete stopBits;
    delete flowControl;
    delete baudComboBox;
    delete dataComboBox;
    delete parityComboBox;
    delete stopComboBox;
    delete flowComboBox;
    delete comPort->_windowProperty;
}

