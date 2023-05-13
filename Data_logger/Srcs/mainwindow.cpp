#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _portCount(0)
    , _selectedComPort(nullptr)
{
    ERROR_LOGGER();
    
    ui->setupUi(this);
    
    this->putWindowOnScreen(700, 616);
    this->_buttonCheck = this->createButton("Check connected ports", 20, 30, 380, 30, std::bind(&MainWindow::buttonCheckAction, this), this);
    this->addLoadingAnimation(this->_buttonCheck, 21, 150, 370, 370);
    this->createGroupBox(20, 70, 380, 515);
    this->createLiftVertical(379, 71, 20, 513);
    this->_buttonChart = this->createButton("Chart", 433, 555, 100, 30, std::bind(&MainWindow::buttonChartAction, this), this);
    this->_buttonNext = this->createButton("Next", 566, 555, 100, 30, std::bind(&MainWindow::buttonNextAction, this), this);
    this->_buttonNext->setEnabled(false);
    this->_buttonNext->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);

    this->_filesList = nullptr;
    this->_windowChart = nullptr;
    this->_windowNext = nullptr;
    
    _baudRateItems = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};
    _dataBitsItems = {"5", "6", "7", "8"};
    _parityItems = {"None", "Even", "Odd", "Space", "Mark"};
    _stopBitsItems = {"1", "1.5", "2"};
    _flowControlItems = {"None", "Hardware", "Xon / Xoff"};

    this->_buttonAbout = new QToolButton(this);
    this->_buttonAbout->setIcon(QIcon(":/Imgs/about.png"));
    this->_buttonAbout->setIconSize(QSize(25, 25));
    this->_buttonAbout->setCursor(Qt::PointingHandCursor);
    this->_buttonAbout->setToolTip("About the program.");
    this->_buttonAbout->setStyleSheet("QToolButton { border-radius: 20px; border: 2px solid #006699; background: #B9E8E2;} \
                                      QToolButton:hover { border-radius: 20px; border: 3px solid #006699; background: white;}");
    this->_buttonAbout->setGeometry(626, 25, 40, 40);
    this->_buttonAbout->show();
    connect(this->_buttonAbout, &QToolButton::clicked, this, [=](void) { ERROR_LOGGER(); this->buttonAboutAction(); });
    
    ERROR_LOGGER();
}

MainWindow::~MainWindow()
{
    ERROR_LOGGER();
    for (QVector<ComPort *>::iterator it = _comPorts.begin(); it < _comPorts.end(); ++it)
        delete *it;
    this->_comPorts.clear();
    delete _buttonCheck;
    delete _buttonNext;
    delete _buttonChart;
    delete _buttonAbout;
    delete _gifLabel;
    delete _gifMovie;
    delete _liftVertical;
    delete _groupBox;
    delete ui;
    ERROR_LOGGER();
}

void    MainWindow::putWindowOnScreen(int windowWidth, int windowHeight)
{
    ERROR_LOGGER();
    
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
    this->setWindowTitle("OQNI: Data logger");
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    this->setStyleSheet("background-image: url(:/Imgs/background.png); font-size: 20px");
    
    ERROR_LOGGER();
}

QPushButton    *MainWindow::createButton(const QString &name, int x, int y, int width, \
                                         int height, std::function<void(void)> onPressAction, QWidget *box)
{
    ERROR_LOGGER();
    QPushButton *button;
   
    /* ------------------- Button design ------------------ */
    button = new QPushButton(name, box);
    button->setGeometry(x, y, width, height);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
    connect(button, &QPushButton::released, button,
        [=](void)
        {
            ERROR_LOGGER();
            button->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
            ERROR_LOGGER();
        });
    connect(button, &QPushButton::clicked, button,
        [=](void)
        {
            ERROR_LOGGER();
            button->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            ERROR_LOGGER();
        });
    connect(button, &QPushButton::pressed, button,
        [=](void)
        {
            ERROR_LOGGER();
            button->setStyleSheet(MY_DEFINED_PRESSED_BUTTON);
            if (onPressAction != nullptr)
                onPressAction();
            ERROR_LOGGER();
        });

    ERROR_LOGGER();
    return (button);
}
    
void    MainWindow::addLoadingAnimation(QPushButton *button, int x, int y, int width, int height)
{
    ERROR_LOGGER();
    
    /* ---------------- Button functional ----------------- */
    this->_gifLabel = new QLabel(this);
    this->_gifLabel->setGeometry(x, y, width, height);
    this->_gifLabel->stackUnder(button);
    this->_gifLabel->hide();
    this->_gifMovie = new QMovie(":/Imgs/loading.gif");
    this->_gifMovie->setScaledSize(this->_gifLabel->size());
    this->_gifLabel->setMovie(this->_gifMovie);
    this->_gifLabel->setStyleSheet("background: #e6e6e6;");
    
    ERROR_LOGGER();
}

void    MainWindow::createGroupBox(int x, int y, int width, int height)
{
    ERROR_LOGGER();
    
    /* ---------------- adding GroupBox ------------------- */
    this->_groupBox = new QGroupBox("Connected COM ports:", this);
    this->_groupBox->setGeometry(x, y, width, height);
    this->_groupBox->stackUnder(this->_gifLabel);
    this->_groupBox->setStyleSheet("border: 1px solid gray; background: #e6e6e6;");
    
    ERROR_LOGGER();
}

void    MainWindow::createLiftVertical(int x, int y, int width, int height)
{
    ERROR_LOGGER();
    
    /* ----------- adding Vertical ScrollBar -------------- */
    this->_liftVertical = new QScrollBar(Qt::Vertical, this);
    this->_liftVertical->setGeometry(x, y, width, height);
    this->_liftVertical->hide();
    connect(this->_liftVertical, &QScrollBar::valueChanged, this->_groupBox,
        [=](void)
        {
            ERROR_LOGGER();
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
            ERROR_LOGGER();
        });
    
    ERROR_LOGGER();
}

void    MainWindow::buttonCheckAction(void)
{
    ERROR_LOGGER();
    
    /* ----------- show animation and update radiobuttons' list ------------- */
    this->_buttonNext->setEnabled(false);
    this->_buttonNext->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);

    this->_previewsRadioButton = nullptr;
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
                        ERROR_LOGGER();
                        
                        this->_buttonNext->setEnabled(true);
                        this->_buttonNext->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                        (*it)->getToolButton()->raise();
                        (*it)->getToolButton()->show();
                        if (this->_previewsRadioButton && this->_previewsRadioButton != *it)
                            this->_previewsRadioButton->getToolButton()->hide();
                        this->_previewsRadioButton = *it;
                        
                        ERROR_LOGGER();
                    });
                if ((*it)->getCheckBox()->isChecked() == false)
                    (*it)->getToolButton()->hide();
                
                connect((*it)->getToolButton(), &QToolButton::clicked, this,
					[=](void)
					{
						ERROR_LOGGER();
                        this->buttonToolAction(*it);
                        ERROR_LOGGER();
					});
            }
        });
    
    ERROR_LOGGER();
}

void    MainWindow::buttonNextAction()
{
    ERROR_LOGGER();
    
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
        ERROR_LOGGER();
        return ;
    }
    
    try
    {
		this->_windowNext = new WindowNext(this);
		this->_windowNext->setButtonBrowse(createButton("Browse", 490, 50, 100, 30, nullptr, this->_windowNext));
		this->_windowNext->setButtonStart(createButton("Start", 140, 300, 100, 30, nullptr, this->_windowNext));
		this->_windowNext->setButtonStop(createButton("Stop", 250, 300, 100, 30, nullptr, this->_windowNext));
		this->_windowNext->setButtonClose(createButton("Close", 360, 300, 100, 30, nullptr, this->_windowNext));
		
		this->_windowNext->exec();
    }
    catch (int ret) {    }
    
	this->_buttonNext->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
    delete this->_windowNext;
    
    ERROR_LOGGER();
}

void    MainWindow::buttonChartAction()
{
	ERROR_LOGGER();
    
    QString		selectedFile;
    QString		line;
	
//  QCoreApplication::applicationDirPath() + "/Recordings";
    selectedFile = QFileDialog::getOpenFileName(this, "Select a file", \
                            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)  + "/oqni/Recordings", \
                            "All Files (*.csv)");
	if (selectedFile == "")
    {
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        ERROR_LOGGER();
        return ;
    }
    QFile file(selectedFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file";
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        ERROR_LOGGER();
        return ;
    }
    
    int		lastSlashIndex = selectedFile.lastIndexOf('/');
    QString	pathToFiles = selectedFile.left(lastSlashIndex + 1);
    QString	fileNamePrefix = selectedFile.mid(lastSlashIndex + 1, 13) + "*.csv";
    
    QDir	directory(pathToFiles);
    QStringList files = directory.entryList(QStringList() << fileNamePrefix, QDir::Files);
    _filesList = new QCheckBox[files.count()];
    
    QDialog	choosingFiles = QDialog(this);
    choosingFiles.setMinimumSize(300, 10 + (files.count() + 1) * 40);
    choosingFiles.setMaximumSize(300, 10 + (files.count() + 1) * 40);
    choosingFiles.setWindowTitle("Please select files");
    choosingFiles.setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    choosingFiles.setWindowFilePath(":/Imgs/oqni.ico");
    choosingFiles.setStyleSheet("background: #e6e6e6;");

    QPushButton	*buttonOk = createButton("OK", 100, 10 + files.count() * 40, 100, 30, nullptr, &choosingFiles);
    
    for (int i = 0; i < files.count(); ++i)
    {
		_filesList[i].setParent(&choosingFiles);
		_filesList[i].setGeometry(20, 10 + i * 40, 260, 30);
		_filesList[i].setChecked(true);
		_filesList[i].setText(files[i]);
		_filesList[i].setStyleSheet("font-size: 18px;");
    }
    
    _isRejected = true;
    connect(buttonOk, &QPushButton::clicked, &choosingFiles,
		[&](void)
		{
            ERROR_LOGGER();
        
            choosingFiles.close();
        	_isRejected = false;
            
            ERROR_LOGGER();
		});
    
    choosingFiles.exec();
    disconnect(buttonOk, &QPushButton::clicked, &choosingFiles, nullptr);
    delete buttonOk;
    if (_isRejected == true)
    {
		delete [] _filesList;
		_filesList = nullptr;
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        ERROR_LOGGER();
        return ;
    }

    QTextStream in(&file);
	line = in.readLine();
	file.close();
	if (line.startsWith("time_millisec,led") == false)
    {
		delete [] _filesList;
		_filesList = nullptr;
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        ERROR_LOGGER();
		return ;
    }

    bool atLeastOneChecked = false;
    for (int i = 0; i < files.count(); i++)
    {
        if (_filesList[i].isChecked())
        {
            atLeastOneChecked = true;
            break;
        }
    }

    if (atLeastOneChecked == true)
    {
        this->_windowChart = new WindowChart(this, pathToFiles, _filesList, files.count());
        this->_windowChart->exec();
    }
    this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
    delete _windowChart;
    _windowChart = nullptr;
    delete [] _filesList;
    _filesList = nullptr;
    
    ERROR_LOGGER();
}

void    MainWindow::buttonToolAction(ComPort *comPort)
{
    ERROR_LOGGER();
    
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
            ERROR_LOGGER();
            comPort->_windowProperty->close();
            ERROR_LOGGER();
		});
    connect(comPort->_setDefaultProperties, &QPushButton::clicked, comPort->_windowProperty,
		[=](void)
		{
           ERROR_LOGGER(); 
        
            baudComboBox->setCurrentIndex(7);
            dataComboBox->setCurrentIndex(3);
            parityComboBox->setCurrentIndex(0);
            stopComboBox->setCurrentIndex(0);
            flowComboBox->setCurrentIndex(0);
            
            ERROR_LOGGER();
		});
    connect(comPort->_saveProperies, &QPushButton::clicked, comPort->_windowProperty,
        [=](void)
        {
            ERROR_LOGGER();
            
            comPort->setBaudRate(baudComboBox->currentText(), this->_baudRateItems);
            comPort->setDataBits(dataComboBox->currentText(), this->_dataBitsItems);
            comPort->setParity(parityComboBox->currentText(), this->_parityItems);
            comPort->setStopBits(stopComboBox->currentText(), this->_stopBitsItems);
            comPort->setFlowControl(flowComboBox->currentText(), this->_flowControlItems);
            comPort->_windowProperty->close();
            
            ERROR_LOGGER();
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
    
    ERROR_LOGGER();
}

void    MainWindow::buttonAboutAction()
{
    ERROR_LOGGER();
    
    QString about = "Data logger 1.0.0 \
                    <br> <br> This program was created in collaboration \
                    <br> between «Tumo Labs» and «OQNI». \
                    <br> <br>Authors: \u00A0\u00A0\u00A0\u00A0Volodya Ismailyan \
                    <br> \u00A0\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0 \
                         \u00A0\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0Vladimir Khlghatyan \
                    <br> <br> ©2023. All rights reserved.";

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Information"));
    msgBox.setText(about);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    msgBox.exec();
    
    ERROR_LOGGER();
}
