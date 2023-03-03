#include "windownext.hpp"

WindowNext::WindowNext(MainWindow *parent)
    : QDialog(parent)
{
    QScreen     *screen = QApplication::primaryScreen();
    QSize       screenSize = screen->size();
    int         screenWidth = screenSize.width();
    int         screenHeight = screenSize.height();
    int         windowWidth = 600;
    int         windowHeight = 350;    

    this->_buttonBrowse = nullptr;
    this->_buttonStart = nullptr;
    this->_buttonStop = nullptr;
    this->_buttonClose = nullptr;
    
    this->_closeEventFlag = true;
    
    this->_selectedComPort = parent->getSelectedComPort();
    
    this->_showReadingPort1 = new QLabel("Read from:", this);
	this->_showReadingPort2 = new QLabel(this->_selectedComPort->getPortName(), this);
    
    this->_showSelectedDir1 = new QLabel("Save to:", this);
    this->_showSelectedDir2 = new QLineEdit(this);
    this->_selectedDirectory = QCoreApplication::applicationDirPath() + "/Recordings/";
    this->_showSelectedDir2->setText(_selectedDirectory);
    
    this->_recordingFolder1 = new QLabel("Recording Folder:", this);
    this->_recordingFolder2 = new QLineEdit(this);
    this->_recordingFolder3 = new QLineEdit(this);
    this->_recordingFolder4 = new QLineEdit(this);
    this->_recordingFolder5 = new QLineEdit(this);

    this->_placement1 = new QLabel("OPT Placement:", this);
    this->_placement2 = new QComboBox(this);
    this->_placement2->addItems({"1.1, 1.2", "3.1, 3.2", "4.1, 4.2"});
    this->_placement3 = new QLabel("IMU Placement:", this);
    this->_placement4 = new QComboBox(this);
    this->_placement4->addItems({"above knee", "below knee"});

    this->_protocol1 = new QLabel("Recording Protocol:", this);
    this->_protocol2 = new QComboBox(this);
	this->_expProtocolsPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    this->_expProtocolsPath += "/oqni/exp_protocols";
    QStringList *items = this->findExpProtocols(this->_expProtocolsPath);
    this->_protocol2->addItems(*items);
    delete items;
    this->_protocol3 = new QLabel("Limb:", this);
    this->_protocol4 = new QComboBox(this);
    this->_protocol4->addItems({"left leg", "right leg"});

    this->_timer1 = new QLabel("Duration (seconds):", this);
    
    this->_lineEdit = new QLineEdit(this);
    this->_finishMsgLabel = new QLabel("", this);
    
    this->setModal(true);
    
    this->setGeometry((screenWidth - windowWidth) / 2 - 300, \
    						(screenHeight - windowHeight) / 2 - 200, \
                    		windowWidth, windowHeight);
    this->setMinimumSize(windowWidth, windowHeight);
    this->setMaximumSize(windowWidth, windowHeight);
    this->setWindowTitle("OQNI: Drawer");
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    this->setStyleSheet("background: #e6e6e6;");
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);

    this->setParametersDesign();
}

WindowNext::~WindowNext()
{
    delete _buttonBrowse;
    delete _buttonClose;
    delete _buttonStart;
    delete _buttonStop;
    delete _showReadingPort1;
    delete _showReadingPort2;
    delete _showSelectedDir1;
    delete _showSelectedDir2;
    delete _recordingFolder1;
    delete _recordingFolder2;
    delete _recordingFolder3;
    delete _recordingFolder4;
    delete _recordingFolder5;
    delete _placement1;
    delete _placement2;
    delete _placement3;
    delete _placement4;
    delete _protocol1;
    delete _protocol2;
    delete _protocol3;
    delete _protocol4;
    delete _timer1;
    delete _lineEdit;
    delete _finishMsgLabel;
}

void    WindowNext::closeEvent(QCloseEvent *event)
{
    if (_closeEventFlag == true)
        event->accept();
    else
        event->ignore();
}

/* -------------------------------- Setters --------------------------------- */

void    WindowNext::setButtonStart(QPushButton *buttonStart)
{
    this->_buttonStart = buttonStart;
    this->_buttonStart->setEnabled(false);
	this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
    connect(this->_buttonStart, &QPushButton::clicked, this,
		[=](void)
		{
            if (this->_durationTimerValue == 0)
                return ;
            this->createDirectory(_selectedDirectory);
            if (this->_selectedDirectory == "")
                return ;
            if (this->readExpProtocol() == 0 || this->_expProtocol.isEmpty() == true)
                return ;
            
			this->setMinimumSize(1200, 700);
			this->setMaximumSize(1200, 700);
			this->_buttonClose->setEnabled(false);
			this->_buttonClose->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
			this->_buttonStart->setEnabled(false);
			this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
			this->_buttonStop->setEnabled(true);
			this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
			this->_lineEdit->setEnabled(false);
			this->_lineEdit->setStyleSheet("background-color: #D3D3D3; padding: 0 5px; color: blue;");
            
            this->_showSelectedDir2->setEnabled(false);
			this->_showSelectedDir2->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
			this->_showSelectedDir2->setCursorPosition(0);
            
            this->_recordingFolder2->setEnabled(false);
			this->_recordingFolder2->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
            this->_recordingFolder3->setEnabled(false);
			this->_recordingFolder3->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
            
            this->_placement2->setEnabled(false);
			this->_placement2->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
            this->_placement4->setEnabled(false);
			this->_placement4->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
            
            this->_protocol2->setEnabled(false);
			this->_protocol2->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
            this->_protocol4->setEnabled(false);
			this->_protocol4->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
            
			this->_closeEventFlag = false;
            
            this->_threadDisplayTimer = new ThreadDisplayTimer(this->_durationTimerValue, this);
            this->_threadDisplayTimer->start();
            
            this->_threadReader = new ThreadReader(_selectedComPort, _showSelectedDir2->text(), _threadDisplayTimer);
            this->_threadReader->start();
            
            this->_finishMsgLabel->hide();
			connect(this->_threadDisplayTimer, &ThreadDisplayTimer::finishedSignal, this, &WindowNext::onThreadDisplayTimerFinished);
		});
}

void		WindowNext::setButtonStop(QPushButton *buttonStop)
{
    this->_buttonStop = buttonStop;
    this->_buttonStop->setEnabled(false);
    this->_buttonStop->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
    connect(this->_buttonStop, &QPushButton::clicked, this,
		[=](void)
		{
			this->_closeEventFlag = true;
        
            this->setMinimumSize(600, 350);
            this->setMaximumSize(600, 350);
            this->_buttonClose->setEnabled(true);
            this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
            this->_buttonStart->setEnabled(true);
            this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
            this->_buttonStop->setEnabled(false);
            this->_buttonStop->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
            this->_lineEdit->setEnabled(true);
            this->_lineEdit->setStyleSheet("background-color: white; padding: 0 5px; color: blue;");
        
            this->_showSelectedDir2->setEnabled(true);
            this->_showSelectedDir2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
        
            this->_recordingFolder2->setEnabled(true);
            this->_recordingFolder2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
            this->_recordingFolder3->setEnabled(true);
            this->_recordingFolder3->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
        
            this->_placement2->setEnabled(true);
            this->_placement2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
            this->_placement4->setEnabled(true);
            this->_placement4->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
        
            this->_protocol2->setEnabled(true);
            this->_protocol2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
            this->_protocol4->setEnabled(true);
            this->_protocol4->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
            
            this->_threadDisplayTimer->requestInterruption();
            this->_threadDisplayTimer->wait();    
            delete this->_threadDisplayTimer;
            this->_threadDisplayTimer = nullptr;

            this->_threadReader->requestInterruption();    
            this->_threadReader->wait();    
            delete this->_threadReader;
            this->_threadReader = nullptr;
        
            this->_finishMsgLabel->setText("Stopped");
            this->_finishMsgLabel->show();
        
		});
}

void		WindowNext::setButtonClose(QPushButton *buttonClose)
{
    this->_buttonClose = buttonClose;
    connect(this->_buttonClose, &QPushButton::clicked, this,
        [=](void)
        {
            this->_closeEventFlag = true;
            this->close();
        });
}

void		WindowNext::setButtonBrowse(QPushButton *buttonBrowse)
{
    this->_buttonBrowse = buttonBrowse;
    connect(this->_buttonBrowse, &QPushButton::clicked, this,
        [=](void)
        {
			QFileDialog dialog;
			QString     selectedDirectoryTmp;
			
		    dialog.setOption(QFileDialog::ShowDirsOnly);
			selectedDirectoryTmp = dialog.getExistingDirectory(this, tr("Save to"), \
					QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
			if (selectedDirectoryTmp != "")
            {
				this->_showSelectedDir2->setText(selectedDirectoryTmp);
				this->_showSelectedDir2->setCursorPosition(0);
                this->_showSelectedDir2->setToolTip(selectedDirectoryTmp);
            }
        });
}

/* ---------------------------- Member functions ---------------------------- */

void    WindowNext::setParametersDesign(void)
{
    this->_showReadingPort1->setGeometry(10, 10, 100, 30);
    this->_showReadingPort1->setStyleSheet("font-size: 18px;");
    this->_showReadingPort2->setGeometry(120, 10, 480, 30);
    this->_showReadingPort2->setStyleSheet("font-size: 14px; color: blue;");
    
    this->_showSelectedDir1->setGeometry(10, 50, 100, 30);
    this->_showSelectedDir1->setStyleSheet("font-size: 18px;");
    this->_showSelectedDir2->setGeometry(120, 50, 360, 30);
    this->_showSelectedDir2->setCursorPosition(0);
    this->_showSelectedDir2->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_showSelectedDir2->setToolTip(_selectedDirectory);
    
    this->_recordingFolder1->setGeometry(10, 90, 160, 30);
    this->_recordingFolder1->setStyleSheet("font-size: 18px;");
    this->_recordingFolder2->setGeometry(180, 90, 105, 30);
    this->_recordingFolder2->setAlignment(Qt::AlignCenter);
    this->_recordingFolder2->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_recordingFolder2->setText("BL-003");
    this->_recordingFolder3->setGeometry(300, 90, 128, 30);
    this->_recordingFolder3->setMaxLength(3);
    this->_recordingFolder3->setAlignment(Qt::AlignCenter);
    this->_recordingFolder3->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_recordingFolder3->setText("000");
    this->_recordingFolder4->setGeometry(438, 90, 70, 30);
    this->_recordingFolder4->setEnabled(false);
    this->_recordingFolder4->setMaxLength(6);
    this->_recordingFolder4->setAlignment(Qt::AlignCenter);
    this->_recordingFolder4->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
    this->_recordingFolder5->setGeometry(518, 90, 70, 30);
    this->_recordingFolder5->setEnabled(false);
    this->_recordingFolder5->setMaxLength(1);
    this->_recordingFolder5->setAlignment(Qt::AlignCenter);
    this->_recordingFolder5->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");

    this->_placement1->setGeometry(10, 130, 160, 30);
    this->_placement1->setStyleSheet("font-size: 18px;");
    this->_placement2->setGeometry(180, 130, 105, 30);
    this->_placement2->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_placement3->setGeometry(300, 130, 160, 30);
    this->_placement3->setStyleSheet("font-size: 18px;");
    this->_placement4->setGeometry(438, 130, 150, 30);
    this->_placement4->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");

    this->_protocol1->setGeometry(10, 170, 160, 30);
    this->_protocol1->setStyleSheet("font-size: 18px;");
    this->_protocol2->setGeometry(180, 170, 105, 30);
    this->_protocol2->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_protocol3->setGeometry(300, 170, 160, 30);
    this->_protocol3->setStyleSheet("font-size: 18px;");
    this->_protocol4->setGeometry(438, 170, 150, 30);
    this->_protocol4->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");

    this->_timer1->setGeometry(10, 210, 160, 30);
    this->_timer1->setStyleSheet("font-size: 18px;");

    this->_lineEdit->setPlaceholderText("enter here");
    this->_lineEdit->setGeometry(180, 210, 105, 30);
    this->_lineEdit->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_lineEdit->setToolTip("Please enter only numeric values.");
    this->_lineEdit->setMaxLength(4);
    this->_lineEdit->setAlignment(Qt::AlignCenter);
    
    this->_finishMsgLabel->setGeometry(360, 205, 160, 40);
    this->_finishMsgLabel->setAlignment(Qt::AlignCenter);
    this->_finishMsgLabel->setStyleSheet("font-size: 24px; color: #B22222; font-weight: bold;");
    
    this->_durationTimerValue = 0;

    /* --- If the text contains a non-numeric character, show warrnig msg --- */
    connect(this->_lineEdit, &QLineEdit::textChanged, this,
        [=](void)
        {
        	if (this->_lineEdit->text().length() == 0)
            {
                this->_lineEdit->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
				this->_durationTimerValue = 0;
				this->_buttonStart->setEnabled(false);
				this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
                return ;
			}
            QString text = this->_lineEdit->text();
            bool hasOnlyDigits = true;
            for (int i = 0; i < text.length(); i++)
            {
                if (text[i].isDigit() == false)
                {
                    hasOnlyDigits = false;
                    this->_lineEdit->setStyleSheet("background-color: red; padding: 0 5px; color: blue;");
                    this->_durationTimerValue = 0;
					this->_buttonStart->setEnabled(false);
					this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
                    QMessageBox::warning(this, tr("Invalid Input"),
                                        tr("Please enter a numeric value."), QMessageBox::Ok);
                    break ;
                }
            }
            if (hasOnlyDigits == true)
            {
                this->_lineEdit->setStyleSheet("background-color: white; padding: 0 5px; color: blue;");
                this->_durationTimerValue = text.toInt();
				this->_buttonStart->setEnabled(true);
				this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
            }
        });
    
    /* --- If the SaveTo lineEdit text changed --- */
    connect(this->_showSelectedDir2, &QLineEdit::textChanged, this,
        [=](void)
        {
            if (this->_showSelectedDir2->text().length() == 0)
            {
                this->_showSelectedDir2->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
            }
            else
            {
                this->_buttonStart->setEnabled(true);
				this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
            }
            this->_selectedDirectory = this->_showSelectedDir2->text();
        });
}

void    WindowNext::createDirectory(const QString &path)
{
    QDir dir(path);
    
    if (!dir.exists())
        if(!dir.mkpath(path))
            this->_selectedDirectory = "";
}

QStringList *WindowNext::findExpProtocols(const QString &path)
{
    QStringList     *items = new QStringList();    
    QDir            directory(path);
    QFileInfoList   fileList;
    
    directory.setNameFilters(QStringList() << "*.csv");
    fileList = directory.entryInfoList();
    for (const QFileInfo& fileInfo : fileList)
        if (fileInfo.isFile())
            items->append(fileInfo.fileName().left(fileInfo.fileName().length() - 4));
    return items;
}

int	WindowNext::readExpProtocol(void)
{
    QString	protocol = this->_expProtocolsPath + "/";
    protocol += this->_protocol2->currentText() + ".csv";
    
    QFile file(protocol);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return (0);
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        this->_expProtocol.push_back(line.split(","));
    }
    file.close();
    this->_expProtocol.pop_front();
    return (1);
}

void   WindowNext::onThreadDisplayTimerFinished(void)
{
	this->setMinimumSize(600, 350);
	this->setMaximumSize(600, 350);
	this->_buttonClose->setEnabled(true);
	this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
	this->_buttonStart->setEnabled(true);
	this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
	this->_buttonStop->setEnabled(false);
	this->_buttonStop->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
	this->_lineEdit->setEnabled(true);
	this->_lineEdit->setStyleSheet("background-color: white; padding: 0 5px; color: blue;");
    
    this->_showSelectedDir2->setEnabled(true);
    this->_showSelectedDir2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
    
    this->_recordingFolder2->setEnabled(true);
    this->_recordingFolder2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
    this->_recordingFolder3->setEnabled(true);
    this->_recordingFolder3->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
    
    this->_placement2->setEnabled(true);
    this->_placement2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
    this->_placement4->setEnabled(true);
    this->_placement4->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
    
    this->_protocol2->setEnabled(true);
    this->_protocol2->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");
    this->_protocol4->setEnabled(true);
    this->_protocol4->setStyleSheet("font-size: 14px; background-color: white; padding: 0 5px; color: blue;");

    this->_threadDisplayTimer->wait();
	delete this->_threadDisplayTimer;
    this->_threadDisplayTimer = nullptr;
    
    this->_threadReader->requestInterruption();
    this->_threadReader->wait();
    delete this->_threadReader;
    this->_threadReader = nullptr;
    
    this->_finishMsgLabel->setText("Finished");
    this->_finishMsgLabel->show();
    
    this->_closeEventFlag = true;
}
