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
//    this->_selectedDirectory = QCoreApplication::applicationDirPath() + "/Recordings";
    this->_selectedDirectory = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/oqni/Recordings";
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
	this->_expProtocolsPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/oqni/exp_protocols";
    QStringList *items = this->findExpProtocols(this->_expProtocolsPath);
    this->_protocol2->addItems(*items);
    delete items;
    this->_protocol3 = new QLabel("Limb:", this);
    this->_protocol4 = new QComboBox(this);
    this->_protocol4->addItems({"left leg", "right leg"});
    this->readExpProtocol();

    this->_timer1 = new QLabel("Duration (seconds):", this);
    
    this->_lineEdit = new QLineEdit(this);
    this->_finishMsgLabel = new QLabel("", this);
    
    this->_showChart = new QCheckBox("Display chart:     ", this);
    
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
    delete _showChart;
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
    if (this->_durationTimerValue == 0)
    {
		this->_buttonStart->setEnabled(false);
        this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
    }
    connect(this->_buttonStart, &QPushButton::clicked, this,
		[=](void)
		{
            if (this->_durationTimerValue == 0)
                return ;
            
			this->setMinimumSize(1200, 700);
			this->setMaximumSize(1200, 700);
			this->_buttonClose->setEnabled(false);
            this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
			this->_buttonStart->setEnabled(false);
            this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
			this->_buttonStop->setEnabled(true);
            this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
			this->_lineEdit->setEnabled(false);
            this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            
            this->_showSelectedDir2->setEnabled(false);
            this->_showSelectedDir2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
			this->_showSelectedDir2->setCursorPosition(0);
            
            this->_recordingFolder2->setEnabled(false);
            this->_recordingFolder2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            this->_recordingFolder3->setEnabled(false);
            this->_recordingFolder3->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            
            this->_placement2->setEnabled(false);
            this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            this->_placement4->setEnabled(false);
            this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            
            this->_protocol2->setEnabled(false);
            this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            this->_protocol4->setEnabled(false);
            this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            
			this->_closeEventFlag = false;
            
            this->_threadDisplayTimer = new ThreadDisplayTimer(this->_durationTimerValue, this, this->_expProtocolsPath, this->_expProtocol); // this thread starts in TreadReader thread
            this->_threadReader = new ThreadReader(_selectedComPort, _threadDisplayTimer);
            this->_threadReader->start();
            
			this->_recordingFolder4->setText(_threadReader->getFileCreationDate());
			this->_recordingFolder5->setText(_threadReader->getFileCreationTime());
            
            this->_finishMsgLabel->hide();
			connect(this->_threadDisplayTimer, &ThreadDisplayTimer::finishedSignal, this, &WindowNext::onThreadDisplayTimerFinished);
    
		////////////////////////////////////////////////////////////////////////////
//			QChart *chart = new QChart();
//			chart->setTitle("Dynamic Line Chart");
//			chart->legend()->hide();
		
//			QLineSeries *series = new QLineSeries();
//			series->setColor(Qt::red);
//			chart->addSeries(series);
		
//			QValueAxis *axisX = new QValueAxis();
//			axisX->setTitleText("X Axis");
//			axisX->setRange(0, 10000);
//			chart->addAxis(axisX, Qt::AlignBottom);
//			series->attachAxis(axisX);
		
//			QValueAxis *axisY = new QValueAxis();
//			axisY->setTitleText("Y Axis");
//			axisY->setRange(-2500000000, 2500000000);
//			chart->addAxis(axisY, Qt::AlignLeft);
//			series->attachAxis(axisY);
            
			QDialog *dialog = new QDialog();
            
            connect(_threadReader, &ThreadReader::stringAdded, dialog,
            	[=](){
                static int ii = -1;
                ++ii;
//                qDebug() << "Time is:" << qFromLittleEndian<qint64>(_threadReader->_dataRead.mid((ii * 29) + 0,8).constData());
//                qDebug() << "Preamble is:" << qFromBigEndian<unsigned int>(_threadReader->_dataRead.mid((ii * 29) + 8,4).constData());
//                qDebug() << "ID is:" << qFromBigEndian<unsigned char>(_threadReader->_dataRead.mid((ii * 29) + 12,1).constData());
//                qDebug() << "Counter is:" << qFromBigEndian<unsigned char>(_threadReader->_dataRead.mid((ii * 29) + 13,1).constData());
//                qDebug() << "Channel num is:" << qFromBigEndian<unsigned char>(_threadReader->_dataRead.mid((ii * 29) + 14,1).constData());
//                qDebug() << "Channel size is:" << qFromBigEndian<unsigned char>(_threadReader->_dataRead.mid((ii * 29) + 15,1).constData());
//                qDebug() << "Data1 is:" << qFromLittleEndian<unsigned int>(_threadReader->_dataRead.mid((ii * 29) + 16,4).constData());
//                qDebug() << "Data2 is:" << qFromLittleEndian<unsigned int>(_threadReader->_dataRead.mid((ii * 29) + 20,4).constData());
//                qDebug() << "Data3 is:" << qFromLittleEndian<unsigned int>(_threadReader->_dataRead.mid((ii * 29) + 24,4).constData());
//                qDebug() << "Labvel is:" << qFromLittleEndian<unsigned char>(_threadReader->_dataRead.mid((ii * 29) + 28,1).constData());
//					dialog->raise();
//					dialog->show();
//                	QStringList data = _threadReader->_data[0][_threadReader->_data[0].size() - 1].split(",");
//                    if (series->count() > 90)
//                        series->remove(0);
//                	series->append(data[0].toInt(), data[1].toInt());
            	});
		
//			QChartView *chartView = new QChartView(chart);
//			chartView->setRenderHint(QPainter::Antialiasing);
		
//			dialog->setLayout(new QVBoxLayout);
//			dialog->layout()->addWidget(chartView);
//			dialog->resize(1000, 6000);
//			dialog->raise();
//            dialog->show();
//			dialog->exec();
		////////////////////////////////////////////////////////////////////////////
		});
}

void		WindowNext::setButtonStop(QPushButton *buttonStop)
{
    this->_buttonStop = buttonStop;
    this->_buttonStop->setEnabled(false);
    this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
    connect(this->_buttonStop, &QPushButton::clicked, this,
		[=](void)
		{
			saveDataToFile("000");
        
			this->_closeEventFlag = true;
        
            this->setMinimumSize(600, 350);
            this->setMaximumSize(600, 350);
            this->_buttonClose->setEnabled(true);
            this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            this->_buttonStart->setEnabled(true);
            this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            this->_buttonStop->setEnabled(false);
            this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            this->_lineEdit->setEnabled(true);
            this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
        
            this->_showSelectedDir2->setEnabled(true);
            this->_showSelectedDir2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
        
            this->_recordingFolder2->setEnabled(true);
            this->_recordingFolder2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
            this->_recordingFolder3->setEnabled(true);
            this->_recordingFolder3->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
        
            this->_placement2->setEnabled(true);
            this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
            this->_placement4->setEnabled(true);
            this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
        
            this->_protocol2->setEnabled(true);
            this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
            this->_protocol4->setEnabled(true);
            this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
            
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
			this->_finishMsgLabel->setStyleSheet("font-size: 28px; color: #B22222; font-weight: bold;");
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
    this->_showSelectedDir2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_showSelectedDir2->setToolTip(_selectedDirectory);
    
    this->_recordingFolder1->setGeometry(10, 90, 160, 30);
    this->_recordingFolder1->setStyleSheet("font-size: 18px;");
    this->_recordingFolder2->setGeometry(180, 90, 105, 30);
    this->_recordingFolder2->setAlignment(Qt::AlignCenter);
    this->_recordingFolder2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_recordingFolder2->setText("BL-003");
    this->_recordingFolder3->setGeometry(300, 90, 128, 30);
    this->_recordingFolder3->setMaxLength(3);
    this->_recordingFolder3->setAlignment(Qt::AlignCenter);
    this->_recordingFolder3->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_recordingFolder3->setText("000");
    this->_recordingFolder4->setGeometry(438, 90, 70, 30);
    this->_recordingFolder4->setEnabled(false);
    this->_recordingFolder4->setMaxLength(6);
    this->_recordingFolder4->setAlignment(Qt::AlignCenter);
    this->_recordingFolder4->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT2);
    this->_recordingFolder4->setText(QDate::currentDate().toString("yyMMdd"));
    this->_recordingFolder5->setGeometry(518, 90, 70, 30);
    this->_recordingFolder5->setEnabled(false);
    this->_recordingFolder5->setMaxLength(6);
    this->_recordingFolder5->setAlignment(Qt::AlignCenter);
    this->_recordingFolder5->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT2);
    this->_recordingFolder5->setText(QTime::currentTime().toString("hhmmss"));

    this->_placement1->setGeometry(10, 130, 160, 30);
    this->_placement1->setStyleSheet("font-size: 18px;");
    this->_placement2->setGeometry(180, 130, 105, 30);
    this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_placement3->setGeometry(300, 130, 160, 30);
    this->_placement3->setStyleSheet("font-size: 18px;");
    this->_placement4->setGeometry(438, 130, 150, 30);
    this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_protocol1->setGeometry(10, 170, 160, 30);
    this->_protocol1->setStyleSheet("font-size: 18px;");
    this->_protocol2->setGeometry(180, 170, 105, 30);
    this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_protocol3->setGeometry(300, 170, 160, 30);
    this->_protocol3->setStyleSheet("font-size: 18px;");
    this->_protocol4->setGeometry(438, 170, 150, 30);
    this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_timer1->setGeometry(10, 210, 160, 30);
    this->_timer1->setStyleSheet("font-size: 18px;");

    this->_lineEdit->setPlaceholderText("enter here");
    this->_lineEdit->setGeometry(180, 210, 105, 30);
    this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_lineEdit->setToolTip("Please enter only numeric values.");
    this->_lineEdit->setMaxLength(3);
    this->_lineEdit->setAlignment(Qt::AlignCenter);
    
    this->_finishMsgLabel->setGeometry(220, 248, 160, 40);
    this->_finishMsgLabel->setAlignment(Qt::AlignCenter);
    
    this->_showChart->setGeometry(288, 210, 166, 30);
    this->_showChart->setStyleSheet("font-size: 18px;");
    this->_showChart->setLayoutDirection(Qt::RightToLeft);
    this->_showChart->setChecked(false);
    
    /* --- If the text contains a non-numeric character, show warrnig msg --- */
    this->_lineEdit->setText(QString::number(this->_durationMax));
    connect(this->_lineEdit, &QLineEdit::textChanged, this,
        [=](void)
        {
			this->_durationTimerValue = 0;
			this->_buttonStart->setEnabled(false);
            this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
        	if (this->_lineEdit->text().length() == 0)
            {
                this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
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
                    QMessageBox::warning(this, tr("Invalid Input"),
                                        tr("Please enter a numeric value."), QMessageBox::Ok);
                    break ;
                }
            }
            if (hasOnlyDigits == true)
            {
                if (this->_lineEdit->text().toInt() > this->_durationMax)
                {
                    this->_lineEdit->setStyleSheet("background-color: red; padding: 0 5px; color: blue;");
                    QString msg = "Duration can't be greater<br>than protocol time (";
                    msg += QString::number(this->_durationMax) + " sec).";
                    QMessageBox::warning(this, tr("Invalid Input"), msg, QMessageBox::Ok);
                }
                else
                {
                    this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
					this->_durationTimerValue = text.toInt();
					this->_buttonStart->setEnabled(true);
                    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                }
            }
        });
    
    /* --- If the SaveTo (Browse) lineEdit text changed --- */
    connect(this->_showSelectedDir2, &QLineEdit::textChanged, this,
        [=](void)
        {
            if (this->_showSelectedDir2->text().length() == 0)
            {
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            }
            else
            {
                this->_buttonStart->setEnabled(true);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            }
            this->_selectedDirectory = this->_showSelectedDir2->text();
        });

    /* --- When recordingFolder2 text changed --- */
    connect(this->_recordingFolder2, &QLineEdit::textChanged, this,
        [=](void)
    	{
            if (this->_recordingFolder2->text().length() == 0)
            {
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            }
            else
            {
                this->_buttonStart->setEnabled(true);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            }
    	});

    /* --- When recordingFolder3 text changed --- */
    connect(this->_recordingFolder3, &QLineEdit::textChanged, this,
        [=](void)
    	{
            if (this->_recordingFolder3->text().length() == 0)
            {
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            }
            else
            {
                this->_buttonStart->setEnabled(true);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
            }
    	});
    
    /* --- When _placement2 value changed --- */
    connect(this->_placement2, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
        });

    /* --- When _placement4 value changed --- */
    connect(this->_placement4, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
        });

    /* --- When expProtocol combo box value changed --- */
    connect(this->_protocol2, &QComboBox::currentTextChanged, this,
        [=](void)
    	{
            this->readExpProtocol();
			this->_lineEdit->setText(QString::number(this->_durationMax));
            this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
    	});

    /* --- When _protocol4 value changed --- */
    connect(this->_protocol4, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
        });
    connect(this->_showChart, &QCheckBox::stateChanged, this,
        [=](void)
        {
        		if (this->_showChart->isChecked() == true)
            {
                qDebug() << "true";
            }
            else
            {
                qDebug() << "false";
            }
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
    this->_expProtocol.clear();
    this->_durationMax = 0;
    QFile file(protocol);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return (1);
    QTextStream in(&file);
    int i = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue ;
        this->_expProtocol.push_back(line.split(","));
        this->_durationMax += this->_expProtocol.back()[2].toInt();
    }
    file.close();
    this->_expProtocol.pop_front();
    this->_durationTimerValue = this->_durationMax;
    return (0);
}

void	WindowNext::saveDataToFile(const QString &subject)
{
	QFile  			myFile[2];
	QTextStream		out[2];
    char            id;
    unsigned char   counter;
    unsigned int    data;
    unsigned char   oldCounter[2];
    
    const char      bytesPA = _threadReader->getBytesPA();
    const char      bytesID = _threadReader->getBytesID();
    const char      bytesCO = _threadReader->getBytesCO();
    const char      bytesCH = _threadReader->getBytesCH();
    const char      bytesOCH = _threadReader->getBytesOCH();
    const char      numOfCH = _threadReader->getNumOfCH();
    const char      sizeOfCH = _threadReader->getSizeOfCH();
    QByteArray		dataRead = _threadReader->getDataRead();
    qint64			startTime = _threadReader->getStartTime();
    int				totalBytes = bytesPA + bytesID + bytesCO + bytesCH + bytesOCH + numOfCH * sizeOfCH +
                                8 + 1; // 8 - sizeof time; 1 - sizeof label;
    
	this->_fullSavingPath = _selectedDirectory + "/";
    this->_fullSavingPath += _recordingFolder2->text() + "/";
    this->_fullSavingPath += _recordingFolder2->text() + "_";
    this->_fullSavingPath += subject + "_";
    this->_fullSavingPath += _threadReader->getFileCreationDate();
	
	const QString	fileNamePrefix = _fullSavingPath + _threadReader->getFileNamePrefix();
	
	this->createDirectory(_fullSavingPath);
	if (this->_selectedDirectory == "")
		return ;
	
	myFile[0].setFileName(fileNamePrefix + "1.csv");
	if (!myFile[0].open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Failed to open file for writing:" << myFile[0].fileName();
		return ;
	}
	
	myFile[1].setFileName(fileNamePrefix + "2.csv");
	if (!myFile[1].open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Failed to open file for writing:" << myFile[1].fileName();
		myFile[0].close();
        myFile[0].remove();
		return ;
	}
    
	out[0].setDevice(&myFile[0]);
	out[1].setDevice(&myFile[1]);
    
    out[0] << "time_millisec,led11,led12,led13,label\n";
    out[1] << "time_millisec,led21,led22,led23,label\n";
    
	id = qFromBigEndian<unsigned char>(dataRead.mid(bytesPA, bytesID).constData());
    oldCounter[id - 1] = qFromBigEndian<unsigned char>(dataRead.mid(bytesPA + bytesID, bytesCO).constData()) - 1;
    
	id = qFromBigEndian<unsigned char>(dataRead.mid(totalBytes + bytesPA, bytesID).constData());
    oldCounter[id - 1] = qFromBigEndian<unsigned char>(dataRead.mid(totalBytes + bytesPA + bytesID, bytesCO).constData()) - 1;
    
	myFile[0].close();
	myFile[1].close();
}

void   WindowNext::onThreadDisplayTimerFinished(void)
{
    if (_durationMax == _durationTimerValue)
        this->saveDataToFile(_recordingFolder3->text());
    else
        this->saveDataToFile("000");
    
    this->_closeEventFlag = true;
    
	this->setMinimumSize(600, 350);
	this->setMaximumSize(600, 350);
	this->_buttonClose->setEnabled(true);
    this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
	this->_buttonStart->setEnabled(true);
    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
	this->_buttonStop->setEnabled(false);
    this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
	this->_lineEdit->setEnabled(true);
    this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    
    this->_showSelectedDir2->setEnabled(true);
    this->_showSelectedDir2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    
    this->_recordingFolder2->setEnabled(true);
    this->_recordingFolder2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_recordingFolder3->setEnabled(true);
    this->_recordingFolder3->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    
    this->_placement2->setEnabled(true);
    this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_placement4->setEnabled(true);
    this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    
    this->_protocol2->setEnabled(true);
    this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_protocol4->setEnabled(true);
    this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_threadDisplayTimer->wait();
	delete this->_threadDisplayTimer;
    this->_threadDisplayTimer = nullptr;

    this->_threadReader->requestInterruption();
    this->_threadReader->wait();
    delete this->_threadReader;
    this->_threadReader = nullptr;
    
    this->_finishMsgLabel->setText("Finished");
    this->_finishMsgLabel->setStyleSheet("font-size: 28px; color: #B22222; font-weight: bold;");
    this->_finishMsgLabel->show();
}
