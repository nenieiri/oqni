#include "windownext.hpp"
#include "debugger.hpp"

WindowNext::WindowNext(MainWindow *parent)
    : QDialog(parent)
{
    DEBUGGER();

    QScreen     *screen = QApplication::primaryScreen();
    QSize       screenSize = screen->size();
    int         screenWidth = screenSize.width();
    int         screenHeight = screenSize.height();
    int         windowWidth = 600;
    int         windowHeight = 350;

    this->_chartDuration = 6 * 1000;
    this->_chartUpdateRatio = 3;

    this->_buttonBrowse = nullptr;
    this->_buttonStart = nullptr;
    this->_buttonStop = nullptr;
    this->_buttonClose = nullptr;

    this->_closeEventFlag = true;

    this->_selectedComPort = parent->getSelectedComPort();

    this->_showReadingPort1 = new QLabel("Read from:", this);
    this->_showReadingPort2 = new QLabel(this->_selectedComPort->getPortName(), this);

    this->_showSelectedDir1 = new QLabel("DB path:", this);
    this->_showSelectedDir2 = new QLineEdit(this);
//    this->_selectedDirectory = QCoreApplication::applicationDirPath() + "/Recordings";
    this->_selectedDirectory = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/oqni/Recordings";
    this->_showSelectedDir2->setText(_selectedDirectory);

//    When editing the line below, don't forget to do the same in the "retryToSaveMetaData()" function.
    this->_metaDataFilePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/oqni/Recordings/metadata.xlsx";
    QFile metaDataFile(_metaDataFilePath);
    if(!metaDataFile.exists())
    {
        this->_metaDataFilePath = "";
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText("metadata.xlsx not found.<br>Continue?");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.addButton(QMessageBox::Ok);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));

        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel)
        {
            DEBUGGER();
            throw	-1;
        }
    }

    this->_recordingFolder1 = new QLabel("Recording Folder:", this);
    this->_recordingFolder2 = new QLineEdit(this);
    this->_recordingFolder3 = new QLineEdit(this);
    this->_recordingFolder4 = new QLineEdit(this);

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

    this->_display = new QLabel("Display:", this);
    this->_showChart = new QCheckBox("chart", this);
    this->_showPic = new QCheckBox("pic", this);
    this->_saveCheckBox = new QCheckBox("save and update MD", this);
    this->_saveCheckBox->setChecked(true);

    this->_chartDialog = nullptr;
    this->_chart = nullptr;
    this->_chartView = nullptr;
    this->_axisX = nullptr;
    this->_axisY = nullptr;
    this->_series = nullptr;
    this->_hBoxLayout = nullptr;
    this->_gridLayout = nullptr;
    this->_gridLayoutPic = nullptr;
    this->_displayTimerPic = nullptr;
    this->_imageLabel = nullptr;
    this->_imageSecondsLabel = nullptr;

    this->setModal(true);

    this->setGeometry((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2, \
                            windowWidth, windowHeight);
    this->setMinimumSize(windowWidth, windowHeight);
    this->setMaximumSize(windowWidth, windowHeight);
    this->setWindowTitle("OQNI: Drawer");
    this->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->setWindowFilePath(":/Imgs/oqni.ico");
    this->setStyleSheet("background: #e6e6e6;");
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);

    this->setParametersDesign();

    DEBUGGER();
}

WindowNext::~WindowNext()
{
    DEBUGGER();

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
    delete _display;
    delete _showChart;
    delete _showPic;
    delete _saveCheckBox;

    DEBUGGER();
}

void    WindowNext::closeEvent(QCloseEvent *event)
{
    DEBUGGER();

    if (_closeEventFlag == true)
        event->accept();
    else
        event->ignore();

    DEBUGGER();
}

/* -------------------------------- Setters --------------------------------- */

void    WindowNext::setButtonStart(QPushButton *buttonStart)
{
    DEBUGGER();

    this->_buttonStart = buttonStart;
    if (this->_durationTimerValue == 0)
    {
        this->_buttonStart->setEnabled(false);
        this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
    }
    connect(this->_buttonStart, &QPushButton::clicked, this,
        [=](void)
        {
            DEBUGGER();

            if (this->_durationTimerValue == 0)
            {
                DEBUGGER();
                return ;
            }

            while (this->saveMetaData("subjects", this->_recordingFolder3->text()) == false)
            {
                if (_metaDataSavingFailMsg == "cancel")
                {
                    _metaDataSavingFailMsg = "";
                    DEBUGGER();
                    return;
                };
            }

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
            this->_recordingFolder4->setEnabled(false);
            this->_recordingFolder4->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);

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
            this->_threadReader = new ThreadReader(_durationTimerValue, _selectedComPort, _threadDisplayTimer, _showPic);
            this->_threadReader->start();

            this->_labelIsOk = _showPic->isChecked() ? true : false;

            this->_finishMsgLabel->hide();
            connect(this->_threadDisplayTimer, &ThreadDisplayTimer::finishedSignal, this, &WindowNext::onThreadDisplayTimerFinished);
            connect(_threadReader, &ThreadReader::protocolConfigDataIsReady, this,
                [=](void)
                {
                    DEBUGGER();

                    this->_bytesPA = _threadReader->getBytesPA();
                    this->_bytesID = _threadReader->getBytesID();
                    this->_bytesCO = _threadReader->getBytesCO();
                    this->_numOfS_OPT = _threadReader->getNumOfS_OPT();
                    this->_numOfS_IMU = _threadReader->getNumOfS_IMU();

                    this->_sampleRate_OPT = _threadReader->getSampleRate_OPT();
                    this->_sampleRate_IMU = _threadReader->getSampleRate_IMU();
                    this->_numOfCH_OPT = _threadReader->getNumOfCH_OPT();
                    this->_numOfCH_IMU = _threadReader->getNumOfCH_IMU();
                    this->_sizeOfCH_OPT = _threadReader->getSizeOfCH_OPT();
                    this->_sizeOfCH_IMU = _threadReader->getSizeOfCH_IMU();

                    this->_startTime = _threadReader->getStartTime();

                    if (this->_showChart->isChecked() == true) // starting thread for drawing chart
                    {
                        this->_chartDialog = new QDialog(this);
                        connect(this->_chartDialog, &QDialog::rejected, this,
                            [=]()
                            {
                                DEBUGGER();

                                this->_chartDialog = nullptr;
                                this->_showChart->setChecked(false);

                                DEBUGGER();
                            });
                        this->execChartDialog();
                    }
                    if (this->_showPic->isChecked() == true)
                    {
                        this->_picDialog = new QDialog(this);
                        connect(this->_picDialog, &QDialog::rejected, this,
                            [=]()
                            {
                                DEBUGGER();

                                this->_picDialog = nullptr;
                                this->_showPic->setChecked(false);

                                DEBUGGER();
                            });
                        this->execPicDialog();
                    }
                });
        });
}

void		WindowNext::setButtonStop(QPushButton *buttonStop)
{
    DEBUGGER();

    this->_buttonStop = buttonStop;
    this->_buttonStop->setEnabled(false);
    this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
    connect(this->_buttonStop, &QPushButton::clicked, this,
        [=](void)
        {
            DEBUGGER();

            QString msg;
            _metaDataSavingFailMsg = "<br><b>REASON:</b> the session was terminated (stopped).";
            msg = this->saveDataToFile("000") + _metaDataSavingFailMsg;

            this->_closeEventFlag = true;

            this->_showChart->setChecked(false);
            this->_showPic->setChecked(false);

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
            this->_recordingFolder4->setEnabled(true);
            this->_recordingFolder4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

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
            this->infoMessageBox(msg);
            this->_metaDataSavingFailMsg = "";

            DEBUGGER();
        });

    DEBUGGER();
}

void		WindowNext::setButtonClose(QPushButton *buttonClose)
{
    DEBUGGER();

    this->_buttonClose = buttonClose;
    connect(this->_buttonClose, &QPushButton::clicked, this,
        [=](void)
        {
            DEBUGGER();

            this->_closeEventFlag = true;
            this->close();
            this->_closeEventFlag = true;

            DEBUGGER();
        });

    DEBUGGER();
}

void		WindowNext::setButtonBrowse(QPushButton *buttonBrowse)
{
    DEBUGGER();

    this->_buttonBrowse = buttonBrowse;
    connect(this->_buttonBrowse, &QPushButton::clicked, this,
        [=](void)
        {
            DEBUGGER();

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

            DEBUGGER();
        });

    DEBUGGER();
}

/* ---------------------------- Member functions ---------------------------- */

void    WindowNext::setParametersDesign(void)
{
    DEBUGGER();

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

    this->_recordingFolder3->setGeometry(300, 90, 123, 30);
    this->_recordingFolder3->setMaxLength(3);
    this->_recordingFolder3->setAlignment(Qt::AlignCenter);
    this->_recordingFolder3->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_recordingFolder3->setText(findMaxSubjectInMetadata());

    this->_recordingFolder4->setGeometry(438, 90, 150, 30);
    this->_recordingFolder4->setAlignment(Qt::AlignCenter);
    this->_recordingFolder4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_recordingFolder4->setText(this->findSubjectInMetadata(_recordingFolder3->text(), nullptr));
    this->_recordingFolder4->setCursorPosition(0);
    this->_recordingFolder4->setToolTip(_recordingFolder4->text());

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

    this->_display->setGeometry(300, 210, 160, 30);
    this->_display->setStyleSheet("font-size: 18px;");

    this->_showChart->setGeometry(438, 210, 160, 30);
    this->_showChart->setStyleSheet("font-size: 18px;");
    this->_showChart->setChecked(false);

    this->_showPic->setGeometry(518, 210, 160, 30);
    this->_showPic->setStyleSheet("font-size: 18px;");
    this->_showPic->setChecked(true);

    this->_saveCheckBox->setGeometry(438, 240, 160, 30);
    this->_saveCheckBox->setStyleSheet("font-size: 14px;");

    /* --- If the text contains a non-numeric character, show warrnig msg --- */
    this->_lineEdit->setText(QString::number(this->_durationMax));
    connect(this->_lineEdit, &QLineEdit::textChanged, this,
        [=](void)
        {
            DEBUGGER();

            this->_durationTimerValue = 0;
            this->_buttonStart->setEnabled(false);
            this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            if (this->_lineEdit->text().length() == 0)
            {
                this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
                return ;
            }
            QString		text = this->_lineEdit->text();
            bool		hasOnlyDigits = true;
            QMessageBox	msgBox;

            msgBox.setWindowTitle(tr("Invalid Input"));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.addButton(QMessageBox::Ok);
            msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));

            for (int i = 0; i < text.length(); i++)
            {
                if (text[i].isDigit() == false)
                {
                    hasOnlyDigits = false;
                    this->_lineEdit->setStyleSheet("background-color: red; padding: 0 5px; color: blue;");
                    msgBox.setText(tr("Please enter a numeric value."));
                    msgBox.exec();
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
                    msgBox.setText(msg);
                    msgBox.exec();
                }
                else
                {
                    this->_lineEdit->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
                    this->_durationTimerValue = text.toInt();
                    this->_buttonStart->setEnabled(true);
                    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                }
            }

            DEBUGGER();
        });

    /* --- If the SaveTo (Browse) lineEdit text changed --- */
    connect(this->_showSelectedDir2, &QLineEdit::textChanged, this,
        [=](void)
        {
            DEBUGGER();

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

            DEBUGGER();
        });

    /* --- When recordingFolder2 text changed --- */
    connect(this->_recordingFolder2, &QLineEdit::textChanged, this,
        [=](void)
        {
            DEBUGGER();

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

            DEBUGGER();
        });

    /* --- When recordingFolder3 text changed --- */
    connect(this->_recordingFolder3, &QLineEdit::textChanged, this,
        [=](void)
        {
            DEBUGGER();

            if (this->_recordingFolder3->text().length() == 0)
            {
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            }
            else
            {
                this->_buttonStart->setEnabled(true);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                this->_recordingFolder4->setText(this->findSubjectInMetadata(_recordingFolder3->text(), nullptr));
                this->_recordingFolder4->setCursorPosition(0);
                this->_recordingFolder4->setToolTip(_recordingFolder4->text());
            }

            DEBUGGER();
        });

    /* --- When recordingFolder4 text changed --- */
    connect(this->_recordingFolder4, &QLineEdit::textChanged, this,
        [=](void)
        {
            DEBUGGER();

            if (this->_recordingFolder4->text().length() == 0)
            {
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
            }
            else
            {
                this->_buttonStart->setEnabled(true);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                this->_recordingFolder4->setToolTip(_recordingFolder4->text());
            }

            DEBUGGER();
        });

    /* --- When _placement2 value changed --- */
    connect(this->_placement2, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            DEBUGGER();
            this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
        });

    /* --- When _placement4 value changed --- */
    connect(this->_placement4, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            DEBUGGER();
            this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
        });

    /* --- When expProtocol combo box value changed --- */
    connect(this->_protocol2, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            DEBUGGER();

            this->readExpProtocol();
            this->_lineEdit->setText(QString::number(this->_durationMax));
            this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);

            DEBUGGER();
        });

    /* --- When _protocol4 value changed --- */
    connect(this->_protocol4, &QComboBox::currentTextChanged, this,
        [=](void)
        {
            DEBUGGER();
            this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT2);
        });
    connect(this->_showChart, &QCheckBox::stateChanged, this,
        [=](void)
        {
            DEBUGGER();

            if (this->_buttonStart->isEnabled() == true)
            {
                DEBUGGER();
                return ;
            }
            if (this->_showChart->isChecked() == true)
            {
                this->_chartDialog = new QDialog(this);
                connect(this->_chartDialog, &QDialog::rejected, this,
                    [=]()
                    {
                        this->_chartDialog = nullptr;
                        this->_showChart->setChecked(false);
                    });
                this->execChartDialog();
            }
            else
            {
                delete _axisX;
                this->_axisX = nullptr;
                delete _axisY;
                this->_axisY = nullptr;
                for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
                    this->_chart->removeSeries(&_series[i]);
                delete [] _series;
                this->_series = nullptr;
                delete this->_chart;
                this->_chart = nullptr;
                delete _chartView;
                this->_chartView = nullptr;
                delete _autoScale;
                this->_autoScale = nullptr;
                delete _sliderHorizontal;
                this->_sliderHorizontal = nullptr;
                delete _sliderHorizontalValues;
                this->_sliderHorizontalValues = nullptr;
                delete [] _checkBoxChannelsValue;
                this->_checkBoxChannelsValue = nullptr;
                delete [] _checkBoxChannels;
                this->_checkBoxChannels = nullptr;
                delete _hBoxLayout;
                this->_hBoxLayout = nullptr;
                delete _gridLayout;
                this->_gridLayout = nullptr;
                if (this->_chartDialog && this->_chartDialog->isVisible())
                    this->_chartDialog->close();
                delete this->_chartDialog;
                this->_chartDialog = nullptr;
                disconnect(this->_threadReader, &ThreadReader::lastRowOfData, this, nullptr);
            }

            DEBUGGER();
        });
    connect(this->_showPic, &QCheckBox::stateChanged, this,
        [=](void)
        {
            DEBUGGER();

            if (this->_buttonStart->isEnabled() == true)
            {
                DEBUGGER();
                return ;
            }
            DEBUGGER();
            if (this->_showPic->isChecked() == true)
            {
                this->_picDialog = new QDialog(this);
                connect(this->_picDialog, &QDialog::rejected, this,
                    [=]()
                    {
                        this->_picDialog = nullptr;
                        this->_showPic->setChecked(false);
                    });
                this->execPicDialog();
            }
            else
            {
                DEBUGGER();
                delete this->_displayTimerPic;
                this->_displayTimerPic = nullptr;
                delete this->_imageLabel;
                this->_imageLabel = nullptr;
                delete this->_imageSecondsLabel;
                this->_imageSecondsLabel = nullptr;
                delete this->_gridLayoutPic;
                this->_gridLayoutPic = nullptr;
                if (this->_picDialog && this->_picDialog->isVisible())
                    this->_picDialog->close();
                delete this->_picDialog;
                this->_picDialog = nullptr;
                disconnect(this->_threadDisplayTimer, &ThreadDisplayTimer::displayTimerText, this, nullptr);
                disconnect(this->_threadDisplayTimer, &ThreadDisplayTimer::currentSecondAndImgPath, this, nullptr);
                this->_labelIsOk = false; // save the file in "000" directory because it will have "labes" with a 0 value
            }

            DEBUGGER();
        });
}

void    WindowNext::createDirectory(const QString &path)
{
    DEBUGGER();

    QDir dir(path);

    if (!dir.exists())
        if(!dir.mkpath(path))
            this->_selectedDirectory = "";

    DEBUGGER();
}

QStringList *WindowNext::findExpProtocols(const QString &path)
{
    DEBUGGER();

    QStringList     *items = new QStringList();
    QDir            directory(path);
    QFileInfoList   fileList;

    directory.setNameFilters(QStringList() << "*.csv");
    fileList = directory.entryInfoList();
    for (const QFileInfo& fileInfo : fileList)
        if (fileInfo.isFile())
            items->append(fileInfo.fileName().left(fileInfo.fileName().length() - 4));

    DEBUGGER();
    return items;
}

int	WindowNext::readExpProtocol(void)
{
    DEBUGGER();

    QString	protocol = this->_expProtocolsPath + "/";
    protocol += this->_protocol2->currentText() + ".csv";
    this->_expProtocol.clear();
    this->_durationMax = 0;
    QFile file(protocol);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUGGER();
        return 1;
    }
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

    DEBUGGER();
    return 0;
}

QString	WindowNext::saveDataToFile(const QString &subject)
{
    DEBUGGER();

    QString msg = "Recording has not been saved.";
    if (this->_saveCheckBox->isChecked() == false)
    {
        DEBUGGER();
        return msg;
    }

    const int           maxIdPlusOne = 5; // max sensor ID + 1
    QFile               *myFile = new QFile[maxIdPlusOne];
    QTextStream         *out = new QTextStream[maxIdPlusOne];
    char                id;
    unsigned char       counter;
    unsigned char       oldCounter[maxIdPlusOne];
    bool                oldCounterExists[maxIdPlusOne] = {false, false, false, false, false};
    QVector<QByteArray> dataRead =_threadReader->getDataRead();
    int                 numOfCH[maxIdPlusOne] = {0, _numOfCH_OPT, _numOfCH_OPT, 0, _numOfCH_IMU * 3};
    int                 sizeOfCH[maxIdPlusOne] = {0, _sizeOfCH_OPT, _sizeOfCH_OPT, 0, _sizeOfCH_IMU};    

    this->_fullSavingPath = _selectedDirectory + "/";
    this->_fullSavingPath += _recordingFolder2->text() + "/";
    this->_fullSavingPath += _recordingFolder2->text() + "_";
    this->_fullSavingPath += subject + "_";
    this->_fullSavingPath += _threadReader->getFileCreationDate();

    this->createDirectory(_fullSavingPath);
    if (this->_selectedDirectory == "")
    {
        delete [] myFile;
        delete [] out;

        DEBUGGER();
        return msg;
    }

    for (int i = 1; i < maxIdPlusOne; ++i)
    {
        DEBUGGER();
        if (i == 3)
            continue ;

        QString	fileNamePrefix = _fullSavingPath + _threadReader->getFileNamePrefix(i);

        myFile[i].setFileName(fileNamePrefix + ((i == 4) ? "" : QString::number(i)) + ".csv");
        if (!myFile[i].open(QIODevice::WriteOnly | QIODevice::Text))
        {
            msg = "Permission denied: failed to open file for writing.<br>" + msg;
            for (int j = 1; j < i; ++j)
            {
                switch (j) {
                case 1:
                case 2:
                case 4:
                    myFile[j].close();
                    myFile[j].remove();
                    break;
                }
            }
            delete [] myFile;
            delete [] out;

            return msg;
        }
        out[i].setDevice(&myFile[i]);

        out[i] << "time_millisec";

        switch (i) {
        case 1:
        case 2:
            // writing first line in the .csv file of OPT sensors
            for (int j = 1; j <= _numOfCH_OPT; ++j)
                out[i] << ",led" + QString::number(i * 10 + j);
            break;
        case 4:
            // writing first line in the .csv file of IMU sensors
            for (int j = 1; j <= _numOfCH_IMU * 3; ++j)
                out[i] << ",led" + QString::number(i * 10 + j);
            break;
        }
        out[i] << ",label\n";
    }

    for (auto &data : dataRead)
    {
        id = qFromBigEndian<unsigned char>(data.mid(_bytesPA, _bytesID).constData());
        counter = qFromBigEndian<unsigned char>(data.mid(_bytesPA + _bytesID, _bytesCO).constData());

        // writing '-' character in the .csv file, if data missed
        if (oldCounterExists[id] == true)
            for (int k = 0; k < counter - oldCounter[id] - 1; ++k)
               out[id] << "-\n";

        oldCounter[id] = counter;
        oldCounterExists[id] = true;

        // writing time in the .csv file (= 'current time' - 'start time')
        // bytes --> [xxxxxx....xxxx-TTTTTTT-x]
        out[id] << qFromLittleEndian<qint64>(data.mid(data.size() - 8 - 1, 8).constData()) - _startTime << ",";

        for (int j = 0; j < numOfCH[id]; ++j)
        {
            switch (id) {
            case 1:
            case 2:
                // unsigned int for OPT sensors data (4 bytes by protocol)
                // bytes --> [xxxxxx....xxxx-DDDD-DDDD-DDDD]
                out[id] << qFromLittleEndian<unsigned int>(data.mid(_bytesPA + _bytesID + _bytesCO + j * sizeOfCH[id], sizeOfCH[id]).constData()) << ",";
                break;
            case 4:
                // int for IMU sensors data (2 bytes by protocol)
                // bytes --> [xxxxxx....xxxx-DD-DD-DD-DD-DD-DD-DD-DD-DD]
                out[id] << qFromLittleEndian<int>(data.mid(_bytesPA + _bytesID + _bytesCO + j * sizeOfCH[id], sizeOfCH[id]).constData()) << ",";
                break;
            }
        }

        // writing current label in the .csv file (last byte of data)
        out[id] << qFromLittleEndian<unsigned char>(data.right(1).constData()) << "\n";
    }

    for (int i = 1; i < maxIdPlusOne; ++i)
    {
        switch (i) {
        case 1:
        case 2:
        case 4:
            myFile[i].close();
            break;
        }
    }
    delete [] myFile;
    delete [] out;

    if (subject == "000")
        msg = "<b>Temporary</b> file created.<br>";
    else
        msg = "<b>Regular</b> file created.<br>";

    QString	fileNamePrefix = _fullSavingPath + _threadReader->getFileNamePrefix(1);
    int tmpLength = fileNamePrefix.length() - fileNamePrefix.indexOf("/Recordings");
    msg += "Recording has been saved to: <br>\u00A0\u00A0\u00A0\u00A0" + \
        fileNamePrefix.right(tmpLength).left(tmpLength - 3) + "[*].csv";

    if (subject == "000")
        msg += "<br><br>metadata.xlsx <b> has not been updated</b>.<br>";
    else
        msg += "<br><br>metadata.xlsx <b> has been updated </b> (see sheet \"DB\").<br>";

    DEBUGGER();
    return msg;
}

bool	WindowNext::saveMetaData(const QString &excelSheet, const QString &subject)
{
    DEBUGGER();

    QString		cell;
    int     	row;
    QStringList	data;

    if (_saveCheckBox->isChecked() == false)
    {
        _metaDataSavingFailMsg = "<br><b>REASON:</b> the save checkbox was not checked.";
        DEBUGGER();
        return true;
    }
    if (_metaDataFilePath == "")
    {
        QXlsx::Document	fakeXlsx("");
        this->retryToSaveMetaData(fakeXlsx, excelSheet);
        if (_metaDataFilePath == "")
            _metaDataSavingFailMsg = "<br><b>REASON:</b> metadata.xlsx not found.";
        else
            _metaDataSavingFailMsg = "";

        DEBUGGER();
        return true;
    }
    if (subject == "000")
    {
        _metaDataSavingFailMsg = "<br><b>REASON:</b> the subject was 000.";
        DEBUGGER();
        return true;
    }

    if (excelSheet == "DB")
    {
        QXlsx::Document	xlsx(_metaDataFilePath);
        xlsx.selectSheet(excelSheet);
        row = xlsx.dimension().lastRow() + 1;

        data.append(subject);
        data.append(_recordingFolder2->text());
        data.append(_recordingFolder2->text() + "_" +subject + "_" + _threadReader->getFileCreationDate());
        data.append(_threadReader->getFileCreationDate() + "_" + _threadReader->getFileCreationTime());
        data.append(_placement2->currentText());
        data.append(_placement4->currentText());
        data.append(_protocol2->currentText());
        data.append(_protocol4->currentText());

        for(int col = 1; col <= data.size(); ++col)
        {
            cell = QXlsx::CellReference(row, col).toString();
            xlsx.write(cell, data[col - 1]);
        }

        if (xlsx.save() == false)
            this->retryToSaveMetaData(xlsx, excelSheet);

        DEBUGGER();
        return true;
    }

    if (excelSheet == "subjects")
    {
        int     subjectRow;
        QString subjectName = findSubjectInMetadata(subject, &subjectRow);

        if (subject == "000" || (subjectName != "unknown" && subjectName == _recordingFolder4->text()))
        {
            DEBUGGER();
            return true;
        }
        if (subjectName == "unknown" || !subjectRow)
        {
            QXlsx::Document	xlsx(_metaDataFilePath);
            xlsx.selectSheet(excelSheet);
            subjectRow = subjectRow ? subjectRow : xlsx.dimension().lastRow() + 1;

            cell = QXlsx::CellReference(subjectRow, 1).toString();
            xlsx.write(cell, subject);
            cell = QXlsx::CellReference(subjectRow, 2).toString();
            xlsx.write(cell, _recordingFolder4->text());

            if (xlsx.save() == false)
                this->retryToSaveMetaData(xlsx, excelSheet);

            DEBUGGER();
            return true;
        }

        QString msg = "Subject <b>" + subject + "</b> exists in metadata.xlsx with name: <b>" +\
                    subjectName + "</b>. <br>Do you want to change the name to: <b>" +\
                    _recordingFolder4->text() + "</b>?";

        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Update Request"));
        msgBox.setText(msg);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
        {
            QXlsx::Document	xlsx(_metaDataFilePath);
            xlsx.selectSheet(excelSheet);

            cell = QXlsx::CellReference(subjectRow, 1).toString();
            xlsx.write(cell, subject);
            cell = QXlsx::CellReference(subjectRow, 2).toString();
            xlsx.write(cell, _recordingFolder4->text());

            if (xlsx.save() == false)
                this->retryToSaveMetaData(xlsx, excelSheet);

            DEBUGGER();
            return true;
        }
        if (ret == QMessageBox::Cancel)
        {
            _metaDataSavingFailMsg = "cancel";
            DEBUGGER();
            return false;
        }
    }

    DEBUGGER();
    return true;
}

void WindowNext::retryToSaveMetaData(QXlsx::Document &xlsx, const QString &excelSheet)
{
    DEBUGGER();

    QString msg = "<br><br> This may be the result of: \
                <br> \u00A0\u00A0\u00A0\u00A0 - The file does not exist or is open. \
                <br> \u00A0\u00A0\u00A0\u00A0 - The file has an invalid path or name. \
                <br> <br> <b> Please fix and try again! </br>";
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Cannot save the file."));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.addButton(QMessageBox::Retry);
    msgBox.addButton(QMessageBox::Ignore);
    msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));

    if (_metaDataFilePath == "")
    {
        if (excelSheet == "subjects")
            msgBox.setText("<b>Cannot</b> add new subject to <b>metadata.xlsx.</b>" + msg);
        else
            msgBox.setText("<b>Cannot</b> save <b>metadata.xlsx.</b>" + msg);

        while (_metaDataFilePath == "")
        {
            int ret = msgBox.exec();
            if (ret == QMessageBox::Ignore)
                break ;
            _metaDataFilePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/oqni/Recordings/metadata.xlsx";
            QFile metaDataFile(_metaDataFilePath);
            if(!metaDataFile.exists())
                _metaDataFilePath = "";
        }
    }
    else
    {
        msgBox.setText("<b>Cannot</b> save <b>metadata.xlsx.</b>" + msg);

        while (xlsx.save() == false)
        {
            int ret = msgBox.exec();
            if (ret == QMessageBox::Ignore)
            {
                _metaDataSavingFailMsg = "<br><b>REASON:</b> metadata.xlsx is open or unavailable.";
                break ;
            }
            _metaDataSavingFailMsg = "";
        }
    }

    DEBUGGER();
}

QString	WindowNext::findMaxSubjectInMetadata(void)
{
    DEBUGGER();

    int	max = 0;
    if (_metaDataFilePath == "")
        return (QString("000"));
    QXlsx::Document xlsx(_metaDataFilePath);
    xlsx.selectSheet("subjects");
    int	row;
    for(row = 2; row <= xlsx.dimension().lastRow(); ++row)
        if (xlsx.read(row, 1).toInt() > max)
            max = xlsx.read(row, 1).toInt();
    if (max == 999)
    {
        DEBUGGER();
        return (QString("000"));
    }
    if (xlsx.read(row - 1, 2) != "unknown")
        ++max;
    if (max < 10)
    {
        DEBUGGER();
        return ("00" + QString::number(max));
    }
    if (max < 100)
    {
        DEBUGGER();
        return ("0" + QString::number(max));
    }

    DEBUGGER();
    return (QString::number(max));
}

QString	WindowNext::findSubjectInMetadata(QString subject, int *subjectRow)
{
    DEBUGGER();

    QString unknown = "unknown";

    if (subjectRow)
        (*subjectRow) = 0;
    if (_metaDataFilePath == "")
    {
        DEBUGGER();
        return unknown;
    }
    QXlsx::Document xlsx(_metaDataFilePath);
    xlsx.selectSheet("subjects");
    for(int row = 2; row <= xlsx.dimension().lastRow(); ++row)
    {
        if (xlsx.read(row, 1).toString() == subject)
        {
            if (subjectRow)
                (*subjectRow) = row;

            DEBUGGER();
            return xlsx.read(row, 2).toString();
        }
    }

    DEBUGGER();
    return unknown;
}

void    WindowNext::execChartDialog(void)
{
    DEBUGGER();

    int screenWidth = QApplication::primaryScreen()->size().width();
    int screenHeight = QApplication::primaryScreen()->size().height();
    int windowWidth = screenWidth * 7 / 10;
    int windowHeight = screenHeight * 9 / 10;

    this->_chartDialog->setGeometry(10, 30, windowWidth, windowHeight);
    this->_chartDialog->setMinimumHeight(windowHeight / 2);
    this->_chartDialog->setMinimumWidth(windowWidth / 2);
    this->_chartDialog->show();
    this->raise();

    this->_chart = new QChart();
    _chart->setTitle("Dynamic Line Chart for OPT sensors");
    _chart->setBackgroundBrush(QBrush(QColor::fromRgb(235, 255, 255)));
    _chart->legend()->hide();

    _series = new QLineSeries[_numOfS_OPT * _numOfCH_OPT];
    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
    {
        _chart->addSeries(&_series[i]);
        if (i % _numOfCH_OPT == 0)
            _series[i].setColor(Qt::blue); // infraRed
        else if (i % _numOfCH_OPT == 1)
            _series[i].setColor(Qt::red);
        else if (i % _numOfCH_OPT == 2)
            _series[i].setColor(Qt::green);
    }

    this->_axisX = new QValueAxis();
    _axisX->setTitleText("Time (milliseconds)");
    _chart->addAxis(_axisX, Qt::AlignBottom);
    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
        _series[i].attachAxis(_axisX);

    this->_axisY = new QValueAxis();
    _axisY->setTitleText("Values");
    _chart->addAxis(_axisY, Qt::AlignLeft);
    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
        _series[i].attachAxis(_axisY);

    this->_checkBoxChannelsValue = new bool[_numOfS_OPT * _numOfCH_OPT];
    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
        this->_checkBoxChannelsValue[i] = true;

    this->_chartTimeFlag = 0;

    _seriesMinY.resize(_numOfS_OPT * _numOfCH_OPT);
    _seriesMaxY.resize(_numOfS_OPT * _numOfCH_OPT);
    _seriesMinY.fill(-1);
    _seriesMaxY.fill(0);

    connect(this->_threadReader, &ThreadReader::lastRowOfData, this,
        [=](QByteArray data)
        {
//            qDebug() << "data:" << data.toHex();

            DEBUGGER();

            if (_chartDialog == nullptr)
            {
                DEBUGGER();
                return;
            }

            unsigned int    value;
            int             ledID;
            unsigned int    minY = -1;
            unsigned int    maxY = 0;
            char            id = qFromBigEndian<unsigned char>(data.mid(_bytesPA, _bytesID).constData());
            qint64          time = qFromLittleEndian<qint64>(data.mid(data.size() - 8 - 1, 8).constData()) - _startTime;
            qint64          minX = time;

            for (int j = 0; (id == 1 || id == 2) && j < _numOfCH_OPT; ++j) // (id == 1 || id == 2) ==> to omit IMU
            {
//                qDebug() << "id:" << (int)id;
                value = qFromLittleEndian<unsigned int>(data.mid(_bytesPA + _bytesID + _bytesCO + j * _sizeOfCH_OPT, _sizeOfCH_OPT).constData());
                ledID = j + id * id - 1;

                _seriesMinY[ledID] = (value < _seriesMinY[ledID]) ? value : _seriesMinY[ledID];
                _seriesMaxY[ledID] = (value > _seriesMaxY[ledID]) ? value : _seriesMaxY[ledID];

                if (_checkBoxChannelsValue[ledID] == true)
                {
                    _series[ledID].append(time, value);
                    while (_series[ledID].count() > _chartDuration / 10)
                        _series[ledID].remove(0);
                }

                // updating axisX and axisY in interval "_chartDuration / 1000 * _chartUpdateRatio"
                if (time + _startTime - _chartTimeFlag >= _chartDuration / 1000 * _chartUpdateRatio)
                {
                    if (_autoScale->isChecked() == true)
                    {
                        for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; i++)
                        {
                            for(int j = 0; j < _series[i].count(); j++)
                            {
                                maxY = std::max(maxY, (unsigned int)_series[i].at(j).y());
                                minY = std::min(minY, (unsigned int)_series[i].at(j).y());
                            }
                        }
                    }
                    else
                        getSeriesMinMaxY(&minY, &maxY);

                    _axisY->setRange(minY, maxY);

                    for (int k = 0; k < _numOfS_OPT * _numOfCH_OPT; ++k)
                        if (_series[k].count() != 0)
                            minX = std::min(minX, (qint64)_series[k].at(0).x());

                    _axisX->setRange(minX, minX + _chartDuration);
                    _chartTimeFlag = time + _startTime;
                }
            }

            DEBUGGER();
        });

    this->_chartView = new QChartView(_chart);
    this->_chartView->setRenderHint(QPainter::Antialiasing);

    this->_sliderHorizontal = new QSlider(Qt::Horizontal, _chartDialog);
    this->_sliderHorizontal->setRange(2, 10);
    this->_sliderHorizontal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    this->_sliderHorizontal->setFixedWidth(300);
    this->_sliderHorizontal->setTickInterval(1);
    this->_sliderHorizontal->setValue(this->_chartDuration / 1000);
    this->_sliderHorizontal->setFixedHeight(15);

    this->_sliderHorizontal->setStyleSheet("QSlider::handle:horizontal {"
                                            "background: white;"
                                            "border: 1px solid black;"
                                            "width: 10px;"
                                            "height: 10px;"
                                            "margin: -8px 0;"
                                            "border-radius: 5px; }"
                                            "QSlider::handle:horizontal:hover {"
                                            "background-color: yellow;"
                                            "border: 1px solid black; }");

    connect(this->_sliderHorizontal, &QSlider::valueChanged, this,
        [=]()
        {
            DEBUGGER();
            this->_chartDuration = this->_sliderHorizontal->value() * 1000;
        });

    this->_gridLayout = new QGridLayout;

    this->_hBoxLayout = new QHBoxLayout;
    this->_checkBoxChannels = new QCheckBox[_numOfS_OPT * _numOfCH_OPT];

    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
    {
        switch (i % _numOfCH_OPT) {
        case 0:
            this->_checkBoxChannels[i].setText("infrared" + QString::number(i / _numOfCH_OPT + 1) + "  ");
            this->_checkBoxChannels[i].setStyleSheet("color: blue; font-size: 14px;");
            break;
        case 1:
            this->_checkBoxChannels[i].setText("red" + QString::number(i / _numOfCH_OPT + 1) + "  ");
            this->_checkBoxChannels[i].setStyleSheet("color: red; font-size: 14px;");
            break;
        case 2:
            this->_checkBoxChannels[i].setText("green" + QString::number(i / _numOfCH_OPT + 1) + "          ");
            this->_checkBoxChannels[i].setStyleSheet("color: green; font-size: 14px;");
            break;
        }
        this->_checkBoxChannels[i].setChecked(true);
        connect(&this->_checkBoxChannels[i], &QCheckBox::clicked, this,
            [=]()
            {
                DEBUGGER();

                if (this->_checkBoxChannels[i].isChecked() == true)
                    this->_checkBoxChannelsValue[i] = true;
                else
                {
                    _series[i].clear();
                    this->_checkBoxChannelsValue[i] = false;
                }

                DEBUGGER();
            });
        this->_hBoxLayout->addWidget(&_checkBoxChannels[i]);
    }

#  ifdef Q_OS_MAC
            _sliderHorizontalValues = new QLabel("  2        3        4        5        6         7        8        9       10", this);
#  else
    _sliderHorizontalValues = new QLabel(" 2         3         4        5          6         7         8        9       10", this);
#  endif
    _sliderHorizontalValues->setStyleSheet("font-size: 12px;");
    _sliderHorizontalValues->setFixedWidth(_sliderHorizontal->width());

    this->_autoScale = new QCheckBox("Autoscale");
    this->_autoScale->setChecked(true);
    this->_autoScale->setStyleSheet("font-size: 16px;");
    connect(this->_autoScale, &QCheckBox::clicked, this,
        [=]()
        {
            DEBUGGER();

            if (_autoScale->isChecked() == true)
            {
                this->_autoScale->setStyleSheet("color: black; font-size: 16px;");
                _sliderHorizontal->setValue(_sliderHorizontalLastValue);
                _sliderHorizontal->setEnabled(true);
            }
            else
            {
                this->_autoScale->setStyleSheet("color: gray; font-size: 16px;");
                _sliderHorizontalLastValue = _sliderHorizontal->value();
                _sliderHorizontal->setValue(10);
                _sliderHorizontal->setEnabled(false);
            }

            DEBUGGER();
        });

    this->_gridLayout->addWidget(_chartView, 0, 0, 1, 5);
    this->_gridLayout->addWidget(_autoScale, 1, 0, 1, 1, Qt::AlignLeft);
    this->_gridLayout->addLayout(_hBoxLayout, 2, 0, 1, 4, Qt::AlignCenter);
    this->_gridLayout->addWidget(_sliderHorizontalValues, 1, 4, 1, 1, Qt::AlignCenter);
    this->_gridLayout->addWidget(_sliderHorizontal, 2, 4, 1, 1, Qt::AlignCenter);
    this->_chartDialog->setLayout(_gridLayout);
}

void    WindowNext::execPicDialog(void)
{
    DEBUGGER();

    int screenWidth = QApplication::primaryScreen()->size().width();
    int screenHeight = QApplication::primaryScreen()->size().height();
    int windowWidth = screenWidth * 3 / 10 - 25;
    int windowHeight = screenHeight * 9 / 10;

    this->_picDialog->setGeometry(10 + screenWidth * 7 / 10 + 5, 30, windowWidth, windowHeight);
    this->_picDialog->setMinimumHeight(windowHeight * 3 / 5);
    this->_picDialog->setMinimumWidth(windowWidth * 3 / 5);
    this->_picDialog->show();
    this->raise();

    _gridLayoutPic = new QGridLayout(_picDialog);
    _picDialog->setLayout(_gridLayoutPic);

    _displayTimerPic = new QLabel("", this->_picDialog);
    _displayTimerPic->setStyleSheet("font-size: 50px; color: #B22222; font-weight: bold;");
    _gridLayoutPic ->addWidget(_displayTimerPic, 0, 0, Qt::AlignCenter);

    connect(this->_threadDisplayTimer, &ThreadDisplayTimer::displayTimerText, this,
        [=](QString text)
        {
            DEBUGGER();
            _displayTimerPic->setText(text);
        });

    this->_imageLabel = new QLabel("", this->_picDialog);
    _gridLayoutPic ->addWidget(_imageLabel, 1, 0, 6, 1, Qt::AlignCenter);

    this->_imageSecondsLabel = new QLabel("", this->_picDialog);
    _gridLayoutPic ->addWidget(_imageSecondsLabel, 7, 0, 2, 1, Qt::AlignCenter);
    connect(this->_threadDisplayTimer, &ThreadDisplayTimer::currentSecondAndImgPath, this,
        [=](int currentSecond, QString imgPath)
        {
            DEBUGGER();
            this->showImage(currentSecond, imgPath);
        });

    DEBUGGER();
}

void    WindowNext::showImage(int currentSecond, QString imgPath)
{
    DEBUGGER();

    QPixmap pixmap(imgPath);

    int width = _picDialog->size().width();
    int height = width * pixmap.height() / pixmap.width(); // to keep image size ratio

    // check if "height" is greater than 6 rows of gridLayout (total rows = 9), in which the image is placed
    if (height > _picDialog->size().height() * 6 / 9)
     {
        height = _picDialog->size().height() * 6 / 9;
        width = height * pixmap.width() / pixmap.height();
     }

    QPixmap scaledPixmap = pixmap.scaled(height, width * 11 / 10, Qt::KeepAspectRatio); // increase "width" by 10%
    QString imageSeconds = QString::number(currentSecond);

    this->_imageSecondsLabel->setStyleSheet("font-size: " + \
                                            QString::number(_picDialog->size().height() * 2 / 9) + \
                                            "px; font-weight: bold;"); // 2/9 = rows of ImageSeconds

    this->_imageLabel->setPixmap(scaledPixmap);
    this->_imageSecondsLabel->setText(imageSeconds);

    DEBUGGER();
}

void    WindowNext::infoMessageBox(const QString &msg)
{
    DEBUGGER();

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Information"));
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    msgBox.exec();

    DEBUGGER();
}

void    WindowNext::getSeriesMinMaxY(unsigned int *minY, unsigned int *maxY)
{
    DEBUGGER();

    (*minY) = -1;
    (*maxY) = 0;

    for (int i = 0; i < _seriesMinY.size(); ++i)
    {
        if (_checkBoxChannels[i].isChecked())
        {
            if (_seriesMinY[i] < (*minY))
                (*minY) = _seriesMinY[i];
            if (_seriesMaxY[i] > (*maxY))
                (*maxY) = _seriesMaxY[i];
        }
    }

    DEBUGGER();
}

void   WindowNext::onThreadDisplayTimerFinished(void)
{
    DEBUGGER();

    QString msg;
    if (_durationMax == _durationTimerValue && this->_labelIsOk == true)
    {
        this->saveMetaData("DB",_recordingFolder3->text());
        if (_metaDataSavingFailMsg == "")
            msg = this->saveDataToFile(_recordingFolder3->text());
        else
            msg = this->saveDataToFile("000") + _metaDataSavingFailMsg;
    }
    else
    {
        if (_metaDataSavingFailMsg == "" && this->_labelIsOk == false)
            _metaDataSavingFailMsg = "<br><b>REASON:</b> the pic checkbox (label) was not checked during the session.";
        msg = this->saveDataToFile("000") + _metaDataSavingFailMsg;
    }

    bool showChartWasChecked = this->_showChart->isChecked();
    this->_showChart->setChecked(false);

    bool showPicWasChecked = this->_showPic->isChecked();
    this->_showPic->setChecked(false);

    this->_closeEventFlag = true;

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
    this->_recordingFolder4->setEnabled(true);
    this->_recordingFolder4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_placement2->setEnabled(true);
    this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_placement4->setEnabled(true);
    this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_protocol2->setEnabled(true);
    this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_protocol4->setEnabled(true);
    this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    if (showChartWasChecked)
        this->_showChart->setChecked(true);
    if (showPicWasChecked)
        this->_showPic->setChecked(true);

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
    this->infoMessageBox(msg);
    this->_metaDataSavingFailMsg = "";

    DEBUGGER();
}
