#include "mainwindow.h"
#include "windownext.hpp"
#include "debugger.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _portCount(0)
    , _selectedComPort(nullptr)
{
    DEBUGGER();
    
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
    connect(this->_buttonAbout, &QToolButton::clicked, this, [=](void) { DEBUGGER(); this->buttonAboutAction(); });
    
    DEBUGGER();
}

MainWindow::~MainWindow()
{
    DEBUGGER();
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
    DEBUGGER();
}

void    MainWindow::putWindowOnScreen(int windowWidth, int windowHeight)
{
    DEBUGGER();
    
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
    
    DEBUGGER();
}

QPushButton    *MainWindow::createButton(const QString &name, int x, int y, int width, \
                                         int height, std::function<void(void)> onPressAction, QWidget *box)
{
    DEBUGGER();
    QPushButton *button;
   
    /* ------------------- Button design ------------------ */
    button = new QPushButton(name, box);
    button->setGeometry(x, y, width, height);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
    connect(button, &QPushButton::released, button,
        [=](void)
        {
            DEBUGGER();
            button->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
            DEBUGGER();
        });
    connect(button, &QPushButton::clicked, button,
        [=](void)
        {
            DEBUGGER();
            button->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            DEBUGGER();
        });
    connect(button, &QPushButton::pressed, button,
        [=](void)
        {
            DEBUGGER();
            button->setStyleSheet(MY_DEFINED_PRESSED_BUTTON);
            if (onPressAction != nullptr)
                onPressAction();
            DEBUGGER();
        });

    DEBUGGER();
    return (button);
}
    
void    MainWindow::addLoadingAnimation(QPushButton *button, int x, int y, int width, int height)
{
    DEBUGGER();
    
    /* ---------------- Button functional ----------------- */
    this->_gifLabel = new QLabel(this);
    this->_gifLabel->setGeometry(x, y, width, height);
    this->_gifLabel->stackUnder(button);
    this->_gifLabel->hide();
    this->_gifMovie = new QMovie(":/Imgs/loading.gif");
    this->_gifMovie->setScaledSize(this->_gifLabel->size());
    this->_gifLabel->setMovie(this->_gifMovie);
    this->_gifLabel->setStyleSheet("background: #e6e6e6;");
    
    DEBUGGER();
}

void    MainWindow::createGroupBox(int x, int y, int width, int height)
{
    DEBUGGER();

    /* ---------------- adding GroupBox ------------------- */
    this->_groupBox = new QGroupBox("Connected COM ports:", this);
    this->_groupBox->setGeometry(x, y, width, height);
    this->_groupBox->stackUnder(this->_gifLabel);
    this->_groupBox->setStyleSheet("color: black; border: 1px solid gray; background: #e6e6e6;");
    
    DEBUGGER();
}

void    MainWindow::createLiftVertical(int x, int y, int width, int height)
{
    DEBUGGER();
    
    /* ----------- adding Vertical ScrollBar -------------- */
    this->_liftVertical = new QScrollBar(Qt::Vertical, this);
    this->_liftVertical->setGeometry(x, y, width, height);
    this->_liftVertical->hide();
    connect(this->_liftVertical, &QScrollBar::valueChanged, this->_groupBox,
        [=](void)
        {
            DEBUGGER();
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
            DEBUGGER();
        });
    
    DEBUGGER();
}

void    MainWindow::buttonCheckAction(void)
{
    DEBUGGER();
    
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
                        DEBUGGER();
                        
                        this->_buttonNext->setEnabled(true);
                        this->_buttonNext->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                        (*it)->getToolButton()->raise();
                        (*it)->getToolButton()->show();
                        if (this->_previewsRadioButton && this->_previewsRadioButton != *it)
                            this->_previewsRadioButton->getToolButton()->hide();
                        this->_previewsRadioButton = *it;
                        
                        DEBUGGER();
                    });
                if ((*it)->getCheckBox()->isChecked() == false)
                    (*it)->getToolButton()->hide();
                
                connect((*it)->getToolButton(), &QToolButton::clicked, this,
					[=](void)
					{
                        DEBUGGER();
                        this->buttonToolAction(*it);
                        DEBUGGER();
					});
            }
        });
    
    DEBUGGER();
}

void    MainWindow::buttonNextAction()
{
    DEBUGGER();
    
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
        DEBUGGER();
        return ;
    }

    try
    {
		this->_windowNext = new WindowNext(this);
		this->_windowNext->setButtonBrowse(createButton("Browse", 490, 50, 100, 30, nullptr, this->_windowNext));
        this->_windowNext->setButtonStart(createButton("Start", 140, 340, 100, 30, nullptr, this->_windowNext));
        this->_windowNext->setButtonStop(createButton("Stop", 250, 340, 100, 30, nullptr, this->_windowNext));
        this->_windowNext->setButtonClose(createButton("Close", 360, 340, 100, 30, nullptr, this->_windowNext));
		
		this->_windowNext->exec();
    }
    catch (int ret) {    }
    
	this->_buttonNext->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
    delete this->_windowNext;
    
    DEBUGGER();
}

void    MainWindow::buttonChartAction()
{
    DEBUGGER();
    
    QString		selectedFile;
    QString		line;
	
//  QCoreApplication::applicationDirPath() + "/Recordings";
    selectedFile = QFileDialog::getOpenFileName(this, "Select a file", \
                            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)  + "/oqni/Recordings", \
                            "All Files (*.csv)");
	if (selectedFile == "")
    {
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        DEBUGGER();
        return ;
    }
    QFile file(selectedFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file";
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        DEBUGGER();
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
        _filesList[i].setStyleSheet("color: black; font-size: 18px;");
    }
    
    _isRejected = true;
    connect(buttonOk, &QPushButton::clicked, &choosingFiles,
		[&](void)
		{
            DEBUGGER();
        
            choosingFiles.close();
        	_isRejected = false;
            
            DEBUGGER();
		});
    
    choosingFiles.exec();
    disconnect(buttonOk, &QPushButton::clicked, &choosingFiles, nullptr);
    delete buttonOk;
    if (_isRejected == true)
    {
		delete [] _filesList;
		_filesList = nullptr;
		this->_buttonChart->setStyleSheet(MY_DEFINED_RELEASED_BUTTON);
        DEBUGGER();
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
        DEBUGGER();
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
    
    DEBUGGER();
}

void    MainWindow::buttonToolAction(ComPort *comPort)
{
    DEBUGGER();
    
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
    portName->setStyleSheet("color: black;");
    QLabel *baudRate = new QLabel("Baud Rate:" , comPort->_windowProperty);
    baudRate->setGeometry(10, 50, 130, 30);
    baudRate->setStyleSheet("color: black;");
    QLabel *dataBits = new QLabel("Data Bits:", comPort->_windowProperty);
    dataBits->setGeometry(10, 90, 130, 30);
    dataBits->setStyleSheet("color: black;");
    QLabel *parity = new QLabel("Parity:", comPort->_windowProperty);
    parity->setGeometry(10, 130, 130, 30);
    parity->setStyleSheet("color: black;");
    QLabel *stopBits = new QLabel("Stop Bits:", comPort->_windowProperty);
    stopBits->setGeometry(10, 170, 130, 30);
    stopBits->setStyleSheet("color: black;");
    QLabel *flowControl = new QLabel("Flow Control:", comPort->_windowProperty);
    flowControl->setGeometry(10, 210, 130, 30);
    flowControl->setStyleSheet("color: black;");
    
    QComboBox *baudComboBox = new QComboBox(comPort->_windowProperty);
    baudComboBox->addItems(this->_baudRateItems);
    baudComboBox->setGeometry(150, 50, 200, 30);
    baudComboBox->setStyleSheet("color: black;");
    
    QComboBox *dataComboBox = new QComboBox(comPort->_windowProperty);
    dataComboBox->addItems(this->_dataBitsItems);
    dataComboBox->setGeometry(150, 90, 200, 30);
    dataComboBox->setStyleSheet("color: black;");
    
    QComboBox *parityComboBox = new QComboBox(comPort->_windowProperty);
    parityComboBox->addItems(this->_parityItems);
    parityComboBox->setGeometry(150, 130, 200, 30);
    parityComboBox->setStyleSheet("color: black;");
    
    QComboBox *stopComboBox = new QComboBox(comPort->_windowProperty);
    stopComboBox->addItems(this->_stopBitsItems);
    stopComboBox->setGeometry(150, 170, 200, 30);
    stopComboBox->setStyleSheet("color: black;");
    
    QComboBox *flowComboBox = new QComboBox(comPort->_windowProperty);
    flowComboBox->addItems(this->_flowControlItems);
    flowComboBox->setGeometry(150, 210, 200, 30);
    flowComboBox->setStyleSheet("color: black;");
    
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
            DEBUGGER();
            comPort->_windowProperty->close();
            DEBUGGER();
		});
    connect(comPort->_setDefaultProperties, &QPushButton::clicked, comPort->_windowProperty,
		[=](void)
		{
           DEBUGGER();
        
            baudComboBox->setCurrentIndex(7);
            dataComboBox->setCurrentIndex(3);
            parityComboBox->setCurrentIndex(0);
            stopComboBox->setCurrentIndex(0);
            flowComboBox->setCurrentIndex(0);
            
            DEBUGGER();
		});
    connect(comPort->_saveProperies, &QPushButton::clicked, comPort->_windowProperty,
        [=](void)
        {
            DEBUGGER();
            
            comPort->setBaudRate(baudComboBox->currentText(), this->_baudRateItems);
            comPort->setDataBits(dataComboBox->currentText(), this->_dataBitsItems);
            comPort->setParity(parityComboBox->currentText(), this->_parityItems);
            comPort->setStopBits(stopComboBox->currentText(), this->_stopBitsItems);
            comPort->setFlowControl(flowComboBox->currentText(), this->_flowControlItems);
            comPort->_windowProperty->close();
            
            DEBUGGER();
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
    
    DEBUGGER();
}

void    MainWindow::buttonAboutAction()
{
    DEBUGGER();
    
    QString about = "Data logger 2.2 (for ESP-32) \
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
    
    DEBUGGER();
}
