#include "mainwindow.h"

int reader_win(const std::string &name, unsigned int BaudRate, const std::string &pathFileName);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _portCount(0)
{
    ui->setupUi(this);
    
    this->putWindowOnScreen(700, 616);
    this->_buttonCheck = this->createButton("Check connected ports", 20, 30, 320, 30, std::bind(&MainWindow::buttonCheckAction, this), this);
    this->addLoadingAnimation(this->_buttonCheck, 21, 150, 310, 310);
    this->createGroupBox(20, 70, 320, 515);
    this->createLiftVertical(319, 71, 20, 513);
    this->_buttonNext = this->createButton("Next", 560, 555, 100, 30, std::bind(&MainWindow::buttonNextAction, this), this);
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

QPushButton    *MainWindow::createButton(const QString &name, int x, int y, int width, int height, std::function<void(void)> onPressAction, QWidget *box)
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
                (*it)->getCheckBox()->setGeometry(10, liftRatio, 285, 20);
                (*it)->getCheckBox()->raise();
                if (liftRatio >= 40)
                    (*it)->getCheckBox()->show();
                else
                    (*it)->getCheckBox()->hide();
                (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");
            }
        });
}

void    MainWindow::buttonCheckAction(void)
{
    /* ----------- show animation and update checkboxes' list -------------- */
    this->_gifLabel->show();
    this->_gifMovie->start();
    this->_liftVertical->hide();
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        (*it)->getCheckBox()->hide();
    QTimer::singleShot(3000, this->_gifLabel, &QLabel::hide);
    QTimer::singleShot(3000, this,
        [=](void)
        {
            for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
                delete (*it);
            this->_comPorts.clear();
                
            QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
            for (const QSerialPortInfo& port : portList)
                this->_comPorts.push_back(new ComPort(port.portName(), this->_groupBox));
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
                (*it)->getCheckBox()->setGeometry(10, 40 * (1 + (it - _comPorts.begin())), 285, 20);
                (*it)->getCheckBox()->raise();
                (*it)->getCheckBox()->show();
                (*it)->getCheckBox()->setStyleSheet("border: 0px solid gray;");
            }
        });
}

void    MainWindow::buttonNextAction()
{
    ComPort	*target;
    
    target = nullptr;
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it != _comPorts.end(); ++it)
    {
         
        if ((*it)->getCheckBox()->isChecked() == true )
        {
            target = *it;
            break ;
        }
    }
    if (target == nullptr)
        return ;
    
    target->_propertyWindow = new QDialog(this);
    target->_propertyWindow->setModal(true);

    target->_propertyWindow->setMinimumSize(360, 300);
    target->_propertyWindow->setMaximumSize(360, 300);
    target->_propertyWindow->setWindowTitle("Properties");
    target->_propertyWindow->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    target->_propertyWindow->setWindowFilePath(":/Imgs/oqni.ico");
    target->_propertyWindow->setStyleSheet("background: #e6e6e6;");
    
    QLabel *portName = new QLabel("Port name:         " + target->getPortName(), target->_propertyWindow);
    portName->setGeometry(10, 10, 430, 30);
    QLabel *baudRate = new QLabel("Baud Rate:" , target->_propertyWindow);
    baudRate->setGeometry(10, 50, 130, 30);
    QLabel *dataBits = new QLabel("Data Bits:", target->_propertyWindow);
    dataBits->setGeometry(10, 90, 130, 30);
    QLabel *parity = new QLabel("Parity:", target->_propertyWindow);
    parity->setGeometry(10, 130, 130, 30);
    QLabel *stopBits = new QLabel("Stop Bits:", target->_propertyWindow);
    stopBits->setGeometry(10, 170, 130, 30);
    QLabel *flowControl = new QLabel("Flow Control:", target->_propertyWindow);
    flowControl->setGeometry(10, 210, 130, 30);
    
    
    QStringList items;
    QComboBox *baudComboBox = new QComboBox(target->_propertyWindow);
    items = {"110", "300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"};
    baudComboBox->addItems(items);
    baudComboBox->setGeometry(150, 50, 200, 30);
    
    QComboBox *dataComboBox = new QComboBox(target->_propertyWindow);
    items = {"5", "6", "7", "8"};
    dataComboBox->addItems(items);
    dataComboBox->setGeometry(150, 90, 200, 30);
    
    QComboBox *parityComboBox = new QComboBox(target->_propertyWindow);
    items = {"Even", "Odd", "None", "Mark", "Space"};
    parityComboBox->addItems(items);
    parityComboBox->setGeometry(150, 130, 200, 30);
    
    QComboBox *stopComboBox = new QComboBox(target->_propertyWindow);
    items = {"1", "1.5", "2"};
    stopComboBox->addItems(items);
    stopComboBox->setGeometry(150, 170, 200, 30);
    
    QComboBox *flowComboBox = new QComboBox(target->_propertyWindow);
    items = {"Xon / Xoff", "Hardware", "None"};
    flowComboBox->addItems(items);
    flowComboBox->setGeometry(150, 210, 200, 30);
    
   	target->_cancel = this->createButton("Cancel", 10, 255, 100, 30, nullptr, target->_propertyWindow);
   	target->_setDefault = this->createButton("Default", 130, 255, 100, 30, nullptr, target->_propertyWindow);
   	target->_start = this->createButton("Start", 250, 255, 100, 30, nullptr, target->_propertyWindow);
    
    connect(target->_cancel, &QPushButton::clicked, target->_propertyWindow,
		[=](void)
		{
			target->_propertyWindow->close();
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
			delete target->_propertyWindow;
            this->_buttonNext->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
		});
    
	connect(target->_setDefault, &QPushButton::clicked, target->_propertyWindow,
		[=](void)
		{
        		baudComboBox->setCurrentIndex(5);
        		dataComboBox->setCurrentIndex(3);
        		parityComboBox->setCurrentIndex(2);
        		stopComboBox->setCurrentIndex(0);
        		flowComboBox->setCurrentIndex(1);
		});
    
	connect(target->_start, &QPushButton::clicked, target->_propertyWindow,
		[=](void)
		{
            QFileDialog dialog;
            dialog.setOption(QFileDialog::ShowDirsOnly);
            
            QString selectedDirectory = dialog.getExistingDirectory(
                this,
                "Select directory to save file",
                QDir::homePath()
            );
            
            selectedDirectory += ("/" + createFileName(target->getPortName()));
            std::string fileName = selectedDirectory.toStdString();
            
            target->setBaudRate(baudComboBox->currentText().toUInt());
//			target->setDataBits(dataComboBox->currentText().toUShort());
//        	target->setParity(parityComboBox->currentText());
//        	target->setStopBits(stopComboBox->currentText().toFloat());
//        	target->setFlowControl(flowComboBox->currentText());
            
//          reader_win(target->getPortName().toStdString(), target->getBaudRate(), fileName);
		});

    target->_propertyWindow->exec();
}

const QString   MainWindow::createFileName(const QString &portName)
{
    QDateTime         currentDateTime = QDateTime::currentDateTime();
    const QString     formattedDateTime = currentDateTime.toString("yyyy-MM-dd_hh-mm-ss");
    const QString     fileName = portName + "_" + formattedDateTime + ".csv";

    return fileName;
}
