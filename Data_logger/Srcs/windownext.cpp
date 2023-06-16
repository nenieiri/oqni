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
    int         windowHeight = 390;

    this->_chartDuration = 4 * 1000;
    this->_chartUpdateRatio_OPT = 3;
    this->_chartUpdateRatio_IMU = 6;

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

    this->_selectedDirectory = this->getExecutableGrandparentDirPath() + "/Recordings";
    this->_showSelectedDir2->setText(_selectedDirectory);

    this->_metaDataFilePath = this->getExecutableGrandparentDirPath() + "/Recordings/metadata.xlsx";
    QFile metaDataFile(_metaDataFilePath);
    if(!metaDataFile.exists())
    {
        this->_metaDataFilePath = "";
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText("metadata.xlsx not found.<br>Continue?");
        msgBox.setIconPixmap(QPixmap(":/Imgs/xlsxNotFound.png"));
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
    this->_expProtocolsPath = this->getExecutableGrandparentDirPath() + "/exp_protocols";
    QStringList *items = this->findExpProtocols(this->_expProtocolsPath);
    this->_protocol2->addItems(*items);
    delete items;
    items = nullptr;
    this->_protocol3 = new QLabel("Limb:", this);
    this->_protocol4 = new QComboBox(this);
    this->_protocol4->addItems({"left leg", "right leg"});
    this->readExpProtocol();

    this->_durationSec1 = new QLabel("Duration (seconds):", this);
    this->_durationSec2 = new QLineEdit(this);
    this->_finishMsgLabel = new QLabel("", this);

    this->_display = new QLabel("Display:", this);
    this->_showChart = new QCheckBox("chart", this);
    this->_showPic = new QCheckBox("pic", this);
    this->_saveCheckBox = new QCheckBox("save and update MD", this);
    this->_saveCheckBox->setChecked(true);

    this->_lightIntensity1 = new QLabel("Light intensity:", this);
    this->_lightIntensity2 = new QLineEdit(this);

    this->_distance1 = new QLabel("Distance:", this);
    this->_distance2 = new QLineEdit(this);

    this->_chartDialog = nullptr;
    this->_chart_OPT = nullptr;
    this->_chart_IMU = nullptr;
    this->_chartView_OPT = nullptr;
    this->_chartView_IMU = nullptr;
    this->_axisX_OPT = nullptr;
    this->_axisX_IMU = nullptr;
    this->_axisY_OPT = nullptr;
    this->_axisY_IMU = nullptr;
    this->_series_OPT = nullptr;
    this->_series_IMU = nullptr;
    this->_hBoxLayoutLegends = nullptr;
    this->_hBoxLayoutOptions = nullptr;
    this->_gridLayout = nullptr;
    this->_gridLayoutPic = nullptr;
    this->_displayTimerPic = nullptr;
    this->_imageLabel = nullptr;
    this->_imageSecondsLabel = nullptr;
    this->_picDialog = nullptr;
    this->_autoScale = nullptr;
    this->_checkBoxSensors = nullptr;

    this->setModal(true);

    this->setGeometry((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2, \
                            windowWidth, windowHeight);
    this->setMinimumSize(windowWidth, windowHeight);
    this->setMaximumSize(windowWidth, windowHeight);
    this->setWindowTitle("OQNI: Session configuration");
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
    _buttonBrowse = nullptr;
    delete _buttonClose;
    _buttonClose = nullptr;
    delete _buttonStart;
    _buttonStart = nullptr;
    delete _buttonStop;
    _buttonStop = nullptr;
    delete _showReadingPort1;
    _showReadingPort1 = nullptr;
    delete _showReadingPort2;
    _showReadingPort2 = nullptr;
    delete _showSelectedDir1;
    _showSelectedDir1 = nullptr;
    delete _showSelectedDir2;
    _showSelectedDir2 = nullptr;
    delete _recordingFolder1;
    _recordingFolder1 = nullptr;
    delete _recordingFolder2;
    _recordingFolder2 = nullptr;
    delete _recordingFolder3;
    _recordingFolder3 = nullptr;
    delete _recordingFolder4;
    _recordingFolder4 = nullptr;
    delete _placement1;
    _placement1 = nullptr;
    delete _placement2;
    _placement2 = nullptr;
    delete _placement3;
    _placement3 = nullptr;
    delete _placement4;
    _placement4 = nullptr;
    delete _protocol1;
    _protocol1 = nullptr;
    delete _protocol2;
    _protocol2 = nullptr;
    delete _protocol3;
    _protocol3 = nullptr;
    delete _protocol4;
    _protocol4 = nullptr;
    delete _durationSec1;
    _durationSec1 = nullptr;
    delete _durationSec2;
    _durationSec2 = nullptr;
    delete _finishMsgLabel;
    _finishMsgLabel = nullptr;
    delete _display;
    _display = nullptr;
    delete _showChart;
    _showChart = nullptr;
    delete _showPic;
    _showPic = nullptr;
    delete _saveCheckBox;
    _saveCheckBox = nullptr;
    delete _lightIntensity1;
    _lightIntensity1 = nullptr;
    delete _lightIntensity2;
    _lightIntensity2 = nullptr;
    delete _distance1;
    _distance1 = nullptr;
    delete _distance2;
    _distance2 = nullptr;

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
            this->_durationSec2->setEnabled(false);
            this->_durationSec2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            this->_lightIntensity2->setEnabled(false);
            this->_lightIntensity2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);
            this->_distance2->setEnabled(false);
            this->_distance2->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_TEXT);

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

            this->_labelIsOk = _showPic->isChecked();

            this->_finishMsgLabel->hide();

            connect(this->_threadDisplayTimer, &ThreadDisplayTimer::finishedSignal, this, &WindowNext::onThreadDisplayTimerFinished);

            connect(_threadReader, &ThreadReader::badEventHappened, this,
                    [=](QString msg)
                    {
                        this->_saveCheckBox->setChecked(false);
                        this->_threadDisplayTimer->requestInterruption();
                        this->warningMessageBox(msg);
                        this->_buttonStop->click();
                    });

            connect(_threadReader, &ThreadReader::protocolConfigDataIsReady, this,
                    [=](void)
                    {
                        DEBUGGER();

                        this->_bytesPA = _threadReader->getBytesPA();
                        this->_bytesID = _threadReader->getBytesID();
                        this->_bytesCO = _threadReader->getBytesCO();
                        this->_bytesTC = _threadReader->getBytesTC();
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
            this->setCellInMetadata("exp_param", 2, 1, _lightIntensity2->text());
            this->setCellInMetadata("exp_param", 2, 2, _distance2->text());
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
                this->_durationSec2->setEnabled(true);
                this->_durationSec2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
                this->_lightIntensity2->setEnabled(true);
                this->_lightIntensity2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
                this->_distance2->setEnabled(true);
                this->_distance2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

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
    this->_showReadingPort1->setStyleSheet("color: black; font-size: 18px;");

    this->_showReadingPort2->setGeometry(120, 10, 480, 30);
    this->_showReadingPort2->setStyleSheet("font-size: 14px; color: blue;");

    this->_showSelectedDir1->setGeometry(10, 50, 100, 30);
    this->_showSelectedDir1->setStyleSheet("color: black; font-size: 18px;");

    this->_showSelectedDir2->setGeometry(120, 50, 360, 30);
    this->_showSelectedDir2->setCursorPosition(0);
    this->_showSelectedDir2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_showSelectedDir2->setToolTip(_selectedDirectory);

    this->_recordingFolder1->setGeometry(10, 90, 160, 30);
    this->_recordingFolder1->setStyleSheet("color: black; font-size: 18px;");

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
    this->_placement1->setStyleSheet("color: black; font-size: 18px;");
    this->_placement2->setGeometry(180, 130, 105, 30);
    this->_placement2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_placement3->setGeometry(300, 130, 160, 30);
    this->_placement3->setStyleSheet("color: black; font-size: 18px;");
    this->_placement4->setGeometry(438, 130, 150, 30);
    this->_placement4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_protocol1->setGeometry(10, 170, 160, 30);
    this->_protocol1->setStyleSheet("color: black; font-size: 18px;");
    this->_protocol2->setGeometry(180, 170, 105, 30);
    this->_protocol2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_protocol3->setGeometry(300, 170, 160, 30);
    this->_protocol3->setStyleSheet("color: black; font-size: 18px;");
    this->_protocol4->setGeometry(438, 170, 150, 30);
    this->_protocol4->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

    this->_durationSec1->setGeometry(10, 210, 160, 30);
    this->_durationSec1->setStyleSheet("color: black; font-size: 18px;");

    this->_durationSec2->setPlaceholderText("enter here");
    this->_durationSec2->setGeometry(180, 210, 105, 30);
    this->_durationSec2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_durationSec2->setToolTip("For test mode, set the duration to 0.");

    this->_durationSec2->setMaxLength(3);
    this->_durationSec2->setAlignment(Qt::AlignCenter);

    this->_lightIntensity2->setPlaceholderText("enter here");
    this->_lightIntensity2->setGeometry(180, 250, 105, 30);
    this->_lightIntensity2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_lightIntensity2->setToolTip("If you change this value and click Start, it will be updated in the MetaData file.");
    this->_lightIntensity2->setAlignment(Qt::AlignCenter);

    this->_distance2->setPlaceholderText("enter here");
    this->_distance2->setGeometry(180, 290, 105, 30);
    this->_distance2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_distance2->setToolTip("If you change this value and click Start, it will be updated in the MetaData file.");
    this->_distance2->setAlignment(Qt::AlignCenter);

    this->_finishMsgLabel->setGeometry(367, 280, 160, 40);
    this->_finishMsgLabel->setAlignment(Qt::AlignCenter);

    this->_display->setGeometry(300, 210, 160, 30);
    this->_display->setStyleSheet("color: black; font-size: 18px;");

    this->_showChart->setGeometry(438, 210, 160, 30);
    this->_showChart->setStyleSheet("color: black; font-size: 18px;");
    this->_showChart->setChecked(false);

    this->_showPic->setGeometry(518, 210, 160, 30);
    this->_showPic->setStyleSheet("color: black; font-size: 18px;");
    this->_showPic->setChecked(true);

    this->_saveCheckBox->setGeometry(438, 240, 160, 30);
    this->_saveCheckBox->setStyleSheet("color: black; font-size: 14px;");

    this->_lightIntensity1->setGeometry(10, 250, 160, 30);
    this->_lightIntensity1->setStyleSheet("color: black; font-size: 18px;");

    this->_distance1->setGeometry(10, 290, 160, 30);
    this->_distance1->setStyleSheet("color: black; font-size: 18px;");

    /* --- If the text contains a non-numeric character, show warrnig msg --- */
    this->_durationSec2->setText(QString::number(this->_durationMax));
    connect(this->_durationSec2, &QLineEdit::textChanged, this,
            [=](void)
            {
                DEBUGGER();

                this->_durationTimerValue = 0;
                this->_buttonStart->setEnabled(false);
                this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
                if (this->_durationSec2->text().length() == 0)
                {
                    this->_durationSec2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
                    return ;
                }
                QString		text = this->_durationSec2->text();
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
                        this->_durationSec2->setStyleSheet("background-color: red; padding: 0 5px; color: blue;");
                        msgBox.setText(tr("Please enter a numeric value."));
                        msgBox.exec();
                        break ;
                    }
                }
                if (hasOnlyDigits == true)
                {
                    if (this->_durationSec2->text().toInt() > this->_durationMax)
                    {
                        this->_durationSec2->setStyleSheet("background-color: red; padding: 0 5px; color: blue;");
                        QString msg = "Duration can't be greater<br>than protocol time (";
                        msg += QString::number(this->_durationMax) + " sec).";
                        msgBox.setText(msg);
                        msgBox.exec();
                    }
                    else
                    {
                        this->_durationSec2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

                        // if duration is set to 0, enable test mode 23:59:59
                        if (text.toInt() == 0)
                        {
                            this->_durationTimerValue = 24 * 3600;
                            this->_saveCheckBox->setChecked(false);
                            this->_saveCheckBox->setCheckable(false);
                            this->_saveCheckBox->setStyleSheet("color: gray; font-size: 14px;");
                        }
                        else
                        {
                            this->_durationTimerValue = text.toInt();
                            this->_saveCheckBox->setCheckable(true);
                            this->_saveCheckBox->setStyleSheet("color: black; font-size: 14px;");
                        }

                        this->_buttonStart->setEnabled(true);
                        this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                    }
                }

                DEBUGGER();
            });

    /* --- If the Light intensity lineEdit text changed --- */
    this->_lightIntensity2->setText(this->getCellFromMetadata("exp_param", 2, 1));
    connect(this->_lightIntensity2, &QLineEdit::textChanged, this,
            [=](void)
            {
                DEBUGGER();

                switch (this->_lightIntensity2->text().length()) {
                case 0:
                    this->_buttonStart->setEnabled(false);
                    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
                    break;
                default :
                    this->_buttonStart->setEnabled(true);
                    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
                }

                DEBUGGER();
            });

    /* --- If the Distance lineEdit text changed --- */
    this->_distance2->setText(this->getCellFromMetadata("exp_param", 2, 2));
    connect(this->_distance2, &QLineEdit::textChanged, this,
            [=](void)
            {
                DEBUGGER();

                switch (this->_distance2->text().length()) {
                case 0:
                    this->_buttonStart->setEnabled(false);
                    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_PASSIVE_BUTTON);
                    break;
                default :
                    this->_buttonStart->setEnabled(true);
                    this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_BUTTON);
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
                this->_durationSec2->setText(QString::number(this->_durationMax));
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
                    delete _axisX_OPT;
                    this->_axisX_OPT = nullptr;
                    delete [] _axisX_IMU;
                    this->_axisX_IMU = nullptr;
                    delete _axisY_OPT;
                    this->_axisY_OPT = nullptr;
                    delete [] _axisY_IMU;
                    this->_axisY_IMU = nullptr;
                    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
                        this->_chart_OPT->removeSeries(&_series_OPT[i]);
                    for (int i = 0; i < _numOfS_IMU * _numOfCH_IMU; ++i)
                        this->_chart_IMU[i / _numOfS_IMU].removeSeries(&_series_IMU[i]);
                    delete [] _series_OPT;
                    this->_series_OPT = nullptr;
                    delete [] _series_IMU;
                    this->_series_IMU = nullptr;
                    delete this->_chart_OPT;
                    this->_chart_OPT = nullptr;
                    delete [] _chart_IMU;
                    this->_chart_IMU = nullptr;
                    delete _chartView_OPT;
                    this->_chartView_OPT = nullptr;
                    delete [] _chartView_IMU;
                    this->_chartView_IMU = nullptr;
                    delete _autoScale;
                    this->_autoScale = nullptr;
                    delete [] _checkBoxSensors;
                    this->_checkBoxSensors = nullptr;
                    delete _sliderHorizontal;
                    this->_sliderHorizontal = nullptr;
                    delete _sliderHorizontalValues;
                    this->_sliderHorizontalValues = nullptr;
                    delete [] _checkBoxChannelsValue;
                    this->_checkBoxChannelsValue = nullptr;
                    delete [] _checkBoxChannels;
                    this->_checkBoxChannels = nullptr;
                    delete _hBoxLayoutLegends;
                    this->_hBoxLayoutLegends = nullptr;
                    delete _hBoxLayoutOptions;
                    this->_hBoxLayoutOptions = nullptr;
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
                    disconnect(this->_threadDisplayTimer, &ThreadDisplayTimer::currentLabel, this, nullptr);
                    this->_labelIsOk = false; // save the file in "000" directory because it will have "labes" with a 0 value
                    DEBUGGER();
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

void	WindowNext::readExpProtocol(void)
{
    DEBUGGER();

    QString	protocol = this->_expProtocolsPath + "/";
    protocol += this->_protocol2->currentText() + ".csv";
    this->_expProtocol.clear();
    this->_durationMax = 0;
    QFile file(protocol);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUGGER();

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
    }
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
    int                 timeCounter[maxIdPlusOne] = {0, 0, 0, 0, 0};

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
            for (int j = 1; j <= _numOfS_IMU * _numOfCH_IMU; ++j)
                out[i] << ",led" + QString::number(i * 10 + j);
            break;
        }
        out[i] << ",label,time counter\n";
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
        // bytes --> [xxxxxx....xxxx-TTTTTTT-x-xxxx]
        out[id] << qFromLittleEndian<qint64>(data.mid(data.size() - 8 - 1 - _bytesTC, 8).constData()) - _startTime << ",";

        for (int j = 0; j < numOfCH[id]; ++j)
        {
            switch (id) {
            case 1:
            case 2:
                // unsigned int for OPT sensors data (4 bytes by protocol)
                // bytes --> [xxxx-x-x-DDDD-DDDD-DDDD-xxxxxxxx-x-xxxx]
                out[id] << qFromLittleEndian<unsigned int>(data.mid(_bytesPA + _bytesID + _bytesCO + j * sizeOfCH[id], sizeOfCH[id]).constData()) << ",";
                break;
            case 4:
                // int for IMU sensors data (2 bytes by protocol)
                // bytes --> [xxxx-x-x-DD-DD-DD-DD-DD-DD-DD-DD-DD-xxxxxxxx-x-xxxx]
                out[id] << qFromLittleEndian<short>(data.mid(_bytesPA + _bytesID + _bytesCO + j * sizeOfCH[id], sizeOfCH[id]).constData()) << ",";
                break;
            }
        }

        // writing current label in the .csv file (1 bytes by protocol)
        // bytes --> [xxxx-x-x-xx...xx-xxxxxxxx-L-xxxx]
        out[id] << qFromLittleEndian<unsigned char>(data.mid(data.size() - 1 - _bytesTC, 1).constData()) << ",";

        // writing Time Counter (received from MC) in the .csv file (4 bytes by protocol)
        // bytes --> [xxxx-x-x-xx...xx-xxxxxxxx-x-CCCC]
        int tc = qFromLittleEndian<int>(data.right(_bytesTC).constData());
        if (tc != timeCounter[id])
            out[id] << tc << '\n';
        else
            out[id] << '\n';
        timeCounter[id] = tc;
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
        data.append(_lightIntensity2->text());
        data.append(_distance2->text());

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
        msgBox.setIconPixmap(QPixmap(":/Imgs/updateExcel.png"));
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
    msgBox.setIconPixmap(QPixmap(":/Imgs/xlsxNotFound.png"));
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
            _metaDataFilePath = this->getExecutableGrandparentDirPath() + "/Recordings/metadata.xlsx";
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

QString	WindowNext::getCellFromMetadata(QString sheet, int row, int col)
{
    DEBUGGER();

    if (_metaDataFilePath == "")
    {
        DEBUGGER();
        return "";
    }
    QXlsx::Document xlsx(_metaDataFilePath);
    xlsx.selectSheet(sheet);
    return xlsx.read(row, col).toString();
}

void	WindowNext::setCellInMetadata(QString sheet, int row, int col, const QString &text)
{
    DEBUGGER();

    if (_metaDataFilePath == "" || text == "")
    {
        DEBUGGER();
        return;
    }
    QXlsx::Document xlsx(_metaDataFilePath);
    xlsx.selectSheet(sheet);
    xlsx.write(QXlsx::CellReference(row, col).toString(), text);
    xlsx.save();

    DEBUGGER();
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

    this->_chart_OPT = new QChart();
    _chart_OPT->setTitle("<b>OPT sensors:</b> ID1 and ID2");
    _chart_OPT->setBackgroundBrush(QBrush(QColor::fromRgb(235, 255, 255)));
    _chart_OPT->legend()->hide();

    this->_chart_IMU = new QChart[_numOfS_IMU];
    _chart_IMU[0].setTitle("<b>Accelerometer:</b> red-<b>X</b> green-<b>Y</b> blue-<b>Z</b>");
    _chart_IMU[1].setTitle("<b>Gyroscope:</b> red-<b>X</b> green-<b>Y</b> blue-<b>Z</b>");
    _chart_IMU[2].setTitle("<b>Magnetometer:</b> red-<b>X</b> green-<b>Y</b> blue-<b>Z</b>");
    for (int i = 0; i < _numOfS_IMU; ++i)
    {
        _chart_IMU[i].setBackgroundBrush(QBrush(QColor::fromRgb(255, 245, 255)));
        _chart_IMU[i].legend()->hide();
    }


    // creating axis X for OPT sersors
    this->_axisX_OPT = new QValueAxis();
    _axisX_OPT->setTitleText("Time (milliseconds)");
    _chart_OPT->addAxis(_axisX_OPT, Qt::AlignBottom);

    // creating axes X for IMU sersors
    this->_axisX_IMU = new QValueAxis[_numOfS_IMU];
    for (int i = 0; i < _numOfS_IMU; ++i)
        _chart_IMU[i].addAxis(&_axisX_IMU[i], Qt::AlignBottom);

    // creating axis Y for OPT sersors
    this->_axisY_OPT = new QValueAxis();
    _axisY_OPT->setTitleText("Values");
    _chart_OPT->addAxis(_axisY_OPT, Qt::AlignLeft);

    // creating axes Y for IMU sersors
    this->_axisY_IMU = new QValueAxis[_numOfS_IMU];
    for (int i = 0; i < _numOfS_IMU; ++i)
    {
        _axisY_IMU[i].setTitleText("Values");
        _chart_IMU[i].addAxis(&_axisY_IMU[i], Qt::AlignLeft);
    }

    // creating series for OPT sersors
    _series_OPT = new QLineSeries[_numOfS_OPT * _numOfCH_OPT];
    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
    {
        _chart_OPT->addSeries(&_series_OPT[i]);
        _series_OPT[i].attachAxis(_axisX_OPT);
        _series_OPT[i].attachAxis(_axisY_OPT);
        switch (i % _numOfCH_OPT) {
        case 0:
            _series_OPT[i].setColor(Qt::green);
            break;
        case 1:
            _series_OPT[i].setColor(Qt::red);
            break;
        case 2:
            _series_OPT[i].setColor(Qt::blue); // infraRed
            break;
        }
    }

    // creating series for IMU sersors
    _series_IMU = new QLineSeries[_numOfS_IMU * _numOfCH_IMU];
    for (int i = 0; i < _numOfS_IMU * _numOfCH_IMU; ++i)
    {
        _chart_IMU[i / _numOfS_IMU].addSeries(&_series_IMU[i]);
        _series_IMU[i].attachAxis(&_axisX_IMU[i / _numOfS_IMU]);
        _series_IMU[i].attachAxis(&_axisY_IMU[i / _numOfS_IMU]);
        switch (i % _numOfCH_IMU) {
        case 0:
            _series_IMU[i].setColor(Qt::green); // X
            break;
        case 1:
            _series_IMU[i].setColor(Qt::red);   // Y
            break;
        case 2:
            _series_IMU[i].setColor(Qt::blue);  // Z
            break;
        }
    }

    this->_checkBoxChannelsValue = new bool[_numOfS_OPT * _numOfCH_OPT];
    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
        this->_checkBoxChannelsValue[i] = true;

    this->_chartTimeFlag_OPT = 0;
    this->_chartTimeFlag_IMU.resize(_numOfS_IMU, 0);

    _seriesMinMaxY_autoscale_OPT.resize(_numOfS_OPT * _numOfCH_OPT);
    _seriesMinY_NoAutoscale_OPT.resize(_numOfS_OPT * _numOfCH_OPT);
    _seriesMaxY_NoAutoscale_OPT.resize(_numOfS_OPT * _numOfCH_OPT);
    _seriesMinY_NoAutoscale_OPT.fill(-1);
    _seriesMaxY_NoAutoscale_OPT.fill(0);

    _seriesMinMaxY_autoscale_IMU.resize(_numOfS_IMU);
    for (int j = 0; j < _seriesMinMaxY_autoscale_IMU.size(); ++j)
        _seriesMinMaxY_autoscale_IMU[j].resize(_numOfCH_IMU);
    _seriesMinY_NoAutoscale_IMU.resize(_numOfS_IMU);
    _seriesMaxY_NoAutoscale_IMU.resize(_numOfS_IMU);
    for (int j = 0; j < _numOfS_IMU; ++j)
        for (int i = 0; i < _numOfCH_IMU; ++i)
            _seriesMinY_NoAutoscale_IMU[j].push_back(SHRT_MAX),
            _seriesMaxY_NoAutoscale_IMU[j].push_back(SHRT_MIN);

    connect(this->_threadReader, &ThreadReader::lastRowOfData, this,
            [=](QByteArray data)
            {
                DEBUGGER();
                if (_chartDialog == nullptr)
                    return;

                char    id = qFromBigEndian<unsigned char>(data.mid(_bytesPA, _bytesID).constData());
                qint64  time = qFromLittleEndian<qint64>(data.mid(data.size() - 8 - 1, 8).constData()) - _startTime;

                switch (id) {
                case 1:
                case 2:
                    fillSeriesAndUpdateAxes_OPT(data, id, time);
                    break;
                case 4:
                    fillSeriesAndUpdateAxes_IMU(data, id, time);
                    break;
                }
            });

    this->_chartView_OPT = new QChartView(_chart_OPT);
    this->_chartView_OPT->setRenderHint(QPainter::Antialiasing);

    this->_chartView_IMU = new QChartView[_numOfS_IMU];
    for (int i = 0; i < _numOfS_IMU; ++i)
        _chartView_IMU[i].setChart(&_chart_IMU[i]),
        _chartView_IMU[i].setRenderHint(QPainter::Antialiasing);

    this->_sliderHorizontal = new QSlider(Qt::Horizontal, _chartDialog);
    this->_sliderHorizontal->setRange(2, 6);
    this->_sliderHorizontal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    this->_sliderHorizontal->setFixedWidth(200);
    this->_sliderHorizontal->setTickInterval(1);
    this->_sliderHorizontal->setValue(this->_chartDuration / 1000);
    this->_sliderHorizontal->setFixedHeight(15);
    this->_sliderHorizontal->setStyleSheet(MY_DEFINED_SLIDER_HORIZONTAL_STYLE);
    connect(this->_sliderHorizontal, &QSlider::valueChanged, this,
            [=]()
            {
                DEBUGGER();
                this->_chartDuration = this->_sliderHorizontal->value() * 1000;
            });

    this->_gridLayout = new QGridLayout;
    this->_hBoxLayoutLegends = new QHBoxLayout;
    this->_checkBoxChannels = new QCheckBox[_numOfS_OPT * _numOfCH_OPT];

    for (int i = 0; i < _numOfS_OPT * _numOfCH_OPT; ++i)
    {
        switch (i % _numOfCH_OPT) {
        case 0:
            this->_checkBoxChannels[i].setText("OPT" + QString::number(i / _numOfCH_OPT + 1) + "green  ");
            this->_checkBoxChannels[i].setStyleSheet("color: green; font-size: 14px;");
            break;
        case 1:
            this->_checkBoxChannels[i].setText("OPT" + QString::number(i / _numOfCH_OPT + 1) + "red  ");
            this->_checkBoxChannels[i].setStyleSheet("color: red; font-size: 14px;");
            break;
        case 2:            
            this->_checkBoxChannels[i].setText("OPT" + QString::number(i / _numOfCH_OPT + 1) + "infrared               ");
            this->_checkBoxChannels[i].setStyleSheet("color: blue; font-size: 14px;");
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
                        _series_OPT[i].clear();
                        _seriesMinMaxY_autoscale_OPT[i].clear();
                        this->_checkBoxChannelsValue[i] = false;
                    }

                    DEBUGGER();
                });
        this->_hBoxLayoutLegends->addWidget(&_checkBoxChannels[i]);
    }

#  ifdef Q_OS_MAC
    _sliderHorizontalValues = new QLabel("  2           3             4             5           6", this);
#  else
    _sliderHorizontalValues = new QLabel(" 2            3             4            5            6", this);

#  endif
    _sliderHorizontalValues->setStyleSheet("color: black; font-size: 12px;");
    _sliderHorizontalValues->setFixedWidth(_sliderHorizontal->width());

    this->_hBoxLayoutOptions = new QHBoxLayout;

    this->_autoScale = new QCheckBox("Autoscale               ");
    this->_autoScale->setChecked(true);
    this->_autoScale->setStyleSheet("color: black; font-size: 16px;");
    this->_hBoxLayoutOptions->addWidget(_autoScale);
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
                    _sliderHorizontal->setValue(_sliderHorizontal->maximum());
                    _sliderHorizontal->setEnabled(false);
                }
                DEBUGGER();
            });

    // _gridLayout's initial layout
    this->_gridLayout->addWidget(_chartView_OPT, 0, 0, 1, 5);
    this->_gridLayout->addLayout(_hBoxLayoutLegends, 1, 0, 1, 4, Qt::AlignCenter);
    this->_gridLayout->addWidget(_sliderHorizontalValues, 1, 4, 1, 1, Qt::AlignCenter);
    this->_gridLayout->addLayout(_hBoxLayoutOptions, 2, 0, 1, 1, Qt::AlignLeft);
    this->_gridLayout->addWidget(_sliderHorizontal, 2, 4, 1, 1, Qt::AlignCenter);
    this->_chartDialog->setLayout(_gridLayout);

    int chartsCount = _numOfS_OPT / 2 + _numOfS_IMU; // 1 for OPT sensors and 3 for IMU sensores
    this->_checkBoxSensors = new QCheckBox[chartsCount];
    QStringList title = {"Accelerometer  ", "Gyroscope  ", "Magnetometer  ", "OPT sensores"};
    for (int i = 0; i < chartsCount; ++i)
    {
        this->_checkBoxSensors[i].setText(title[i]);
        this->_checkBoxSensors[i].setChecked(i == 3);
        this->_checkBoxSensors[i].setEnabled(i != 3);
        this->_checkBoxSensors[i].setStyleSheet("color: black; font-size: 14px;");
        connect(&this->_checkBoxSensors[i], &QCheckBox::clicked, this,
                [=]()
                {
                    DEBUGGER();

                    // we calculate how many boxes are checked
                    int checkedSum = 0;
                    for (int j = 0; j < chartsCount; ++j)
                        checkedSum += _checkBoxSensors[j].isChecked();

                    // setting update frequency of axes X
                    switch (checkedSum) {
                    case 1:
                        if (_checkBoxSensors[chartsCount - 1].isChecked() == true)
                            _chartUpdateRatio_OPT = 3;
                        else
                            _chartUpdateRatio_IMU = 1;
                        break;
                    case 2:
                        if (_checkBoxSensors[chartsCount - 1].isChecked() == false)
                            _chartUpdateRatio_IMU = 3;
                        else
                        {
                            _chartUpdateRatio_OPT = 6;
                            _chartUpdateRatio_IMU = 5;
                        }
                        break;
                    case 3:
                    case 4:
                        if (_checkBoxSensors[chartsCount - 1].isChecked() == false)
                            _chartUpdateRatio_IMU = 3;
                        else
                        {
                            _chartUpdateRatio_OPT = 10;
                            _chartUpdateRatio_IMU = 8;
                        }
                        break;
                    }

                    // if only one box is checked, we disable that box
                    // so at least one box must be checked
                    for (int j = 0; j < chartsCount; ++j)
                        _checkBoxSensors[j].setEnabled(checkedSum != 1 || !_checkBoxSensors[j].isChecked());

                    // fist we removed all items from the _gridLayout to add in new order
                    for (int j = 0; j < _numOfS_IMU; ++j)
                    {
                        _gridLayout->removeWidget(&_chartView_IMU[j]);
                        _chartView_IMU[j].hide();
                    }
                    _gridLayout->removeWidget(_chartView_OPT);
                    _chartView_OPT->hide();
                    _gridLayout->removeItem(_hBoxLayoutLegends);
                    _gridLayout->removeWidget(_sliderHorizontalValues);
                    _gridLayout->removeItem(_hBoxLayoutOptions);
                    _gridLayout->removeWidget(_sliderHorizontal);
                    _gridLayout->update();

                    // than we add items to the _gridLayout in new order
                    int offset = 0;
                    for (int j = 0; j < _numOfS_IMU; ++j)
                    {
                        if (_checkBoxSensors[j].isChecked())
                        {
                            _gridLayout->addWidget(&_chartView_IMU[j], offset, 0, 1, 5);
                            _gridLayout->setRowStretch(offset, true); // set the stretch factor for the rows
                            _chartView_IMU[j].show();
                            ++offset;
                        }
                    }
                    if (_checkBoxSensors[_numOfS_IMU].isChecked())
                    {
                        _gridLayout->addWidget(_chartView_OPT, offset, 0, 1, 5);
                        _gridLayout->setRowStretch(offset, true); // set the stretch factor for the rows
                        _chartView_OPT->show();
                        ++offset;
                    }
                    _gridLayout->addLayout(_hBoxLayoutLegends, offset, 0, 1, 4, Qt::AlignCenter);
                    _gridLayout->addWidget(_sliderHorizontalValues, offset, 4, 1, 1, Qt::AlignCenter);
                    _gridLayout->setRowStretch(offset, false); // UNset the stretch factor for the rows
                    _gridLayout->addLayout(_hBoxLayoutOptions, ++offset, 0, 1, 1, Qt::AlignLeft); // increasing offset (i.e. go to the next ров)
                    _gridLayout->addWidget(_sliderHorizontal, offset, 4, 1, 1, Qt::AlignCenter);
                    _gridLayout->setRowStretch(offset, false); // UNset the stretch factor for the rows

                    // if OPT _checkBoxSensors is checked/unchecked enable/disable all _checkBoxChannels
                    QStringList format = {"color: green; font-size: 14px;", "color: red; font-size: 14px;", "color: blue; font-size: 14px;"};
                    if (_checkBoxSensors[chartsCount - 1].isChecked())
                    {
                        for (int j = 0; j < _numOfS_OPT * _numOfCH_OPT; ++j)
                            _checkBoxChannels[j].setEnabled(true),
                            _checkBoxChannels[j].setStyleSheet(format[j % 3]);
                    }
                    else
                    {
                        for (int j = 0; j < _numOfS_OPT * _numOfCH_OPT; ++j)
                            _checkBoxChannels[j].setEnabled(false),
                            _checkBoxChannels[j].setStyleSheet("color: gray; font-size: 14px;");
                    }

                    _gridLayout->update();

                    DEBUGGER();
                });
    this->_hBoxLayoutOptions->addWidget(&_checkBoxSensors[i]);
    }
}

void    WindowNext::fillSeriesAndUpdateAxes_OPT(QByteArray &data, char &id, qint64 &time)
{
    DEBUGGER();

    unsigned int    value, ledID, minY = -1, maxY = 0;
    qint64          minX = time;

    for (int ch = 0; ch < _numOfCH_OPT; ++ch)
    {
        value = qFromLittleEndian<unsigned int>(data.mid(_bytesPA + _bytesID + _bytesCO + ch * _sizeOfCH_OPT, _sizeOfCH_OPT).constData());
        ledID = ch + id * id - 1;

        _seriesMinY_NoAutoscale_OPT[ledID] = std::min(value, _seriesMinY_NoAutoscale_OPT[ledID]);
        _seriesMaxY_NoAutoscale_OPT[ledID] = std::max(value, _seriesMaxY_NoAutoscale_OPT[ledID]);

        if (_checkBoxChannelsValue[ledID] == true)
        {
            _series_OPT[ledID].append(time, value);
            _seriesMinMaxY_autoscale_OPT[ledID].insert(value);
            while (_series_OPT[ledID].count() > _chartDuration / 1000 * _sampleRate_OPT)
            {
                _seriesMinMaxY_autoscale_OPT[ledID].erase(_series_OPT[ledID].at(0).y());
                _series_OPT[ledID].remove(0);
            }
        }

        // updating axisX and axisY in interval "_chartDuration / 1000 * _chartUpdateRatio_OPT"
        if (_checkBoxSensors[_numOfS_IMU].isChecked()) // _checkBoxSensors[3]
        {
            if (time + _startTime - _chartTimeFlag_OPT >= _chartDuration / 1000 * _chartUpdateRatio_OPT)
            {
                getSeriesMinMaxY_OPT(minY, maxY);
                _axisY_OPT->setRange(minY, maxY);

                for (int k = 0; k < _numOfS_OPT * _numOfCH_OPT; ++k)
                    if (_series_OPT[k].count() != 0)
                        minX = std::min(minX, (qint64)_series_OPT[k].at(0).x());

                _axisX_OPT->setRange(minX + 70, minX + _chartDuration);
                _chartTimeFlag_OPT = time + _startTime;
            }
        }
    }
    DEBUGGER();
}

void    WindowNext::fillSeriesAndUpdateAxes_IMU(QByteArray &data, char &id, qint64 &time)
{
    DEBUGGER();

    short   value, minY = SHRT_MAX, maxY = SHRT_MIN;
    qint64  minX = time;

    for (int ch = 0; ch < _numOfS_IMU * _numOfCH_IMU; ++ch)
    {
        value = qFromLittleEndian<short>(data.mid(_bytesPA + _bytesID + _bytesCO + ch * _sizeOfCH_IMU, _sizeOfCH_IMU).constData());

        _seriesMinY_NoAutoscale_IMU[ch / _numOfS_IMU][ch % _numOfCH_IMU] = std::min(value, _seriesMinY_NoAutoscale_IMU[ch / _numOfS_IMU][ch % _numOfCH_IMU]);
        _seriesMaxY_NoAutoscale_IMU[ch / _numOfS_IMU][ch % _numOfCH_IMU] = std::max(value, _seriesMaxY_NoAutoscale_IMU[ch / _numOfS_IMU][ch % _numOfCH_IMU]);

        _series_IMU[ch].append(time, value);
        _seriesMinMaxY_autoscale_IMU[ch / _numOfS_IMU][ch % _numOfCH_IMU].insert(value);
        while (_series_IMU[ch].count() > _chartDuration / 1000 * _sampleRate_IMU)
        {
            _seriesMinMaxY_autoscale_IMU[ch / _numOfS_IMU][ch % _numOfCH_IMU].erase(_series_IMU[ch].at(0).y());
            _series_IMU[ch].remove(0);
        }

        // updating axisX and axisY in interval "_chartDuration / 1000 * _chartUpdateRatio_IMU"
        if (_checkBoxSensors[ch / _numOfS_IMU].isChecked())
        {
            if (time + _startTime - _chartTimeFlag_IMU[ch / _numOfS_IMU] >= _chartDuration / 1000 * _chartUpdateRatio_IMU)
            {
                getSeriesMinMaxY_IMU(minY, maxY, ch / _numOfS_IMU);
                _axisY_IMU[ch / _numOfS_IMU].setRange(minY, maxY);

                int begin = (ch / _numOfS_IMU) * _numOfCH_IMU;
                int end = begin + _numOfCH_IMU;
                for (int k = begin; k < end; ++k)
                {
                    if (_series_IMU[k].count() != 0)
                    {
                        minX = std::min(minX, (qint64)_series_IMU[k].at(0).x());
                        break;
                    }
                }
                _axisX_IMU[ch / _numOfS_IMU].setRange(minX + 70, minX + _chartDuration - 70);
                _chartTimeFlag_IMU[ch / _numOfS_IMU] = time + _startTime;
            }
        }
    }

    DEBUGGER();
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

    this->_imageSecondsLabel->setStyleSheet("color: black; font-size: " + \
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
    if (msg.startsWith("<b>Regular"))
        msgBox.setIconPixmap(QPixmap(":/Imgs/checkMark.png"));
    else if (msg.startsWith("<b>Temporary"))
        msgBox.setIconPixmap(QPixmap(":/Imgs/exclamationMark.png"));
    else if (msg.startsWith("Recording"))
        msgBox.setIconPixmap(QPixmap(":/Imgs/crossMark.png"));
    else
        msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    msgBox.exec();

    DEBUGGER();
}

void    WindowNext::warningMessageBox(const QString &msg)
{
    DEBUGGER();
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Warning"));
    if (msg.startsWith("Failed to get"))
        msgBox.setIconPixmap(QPixmap(":/Imgs/configFailed.png"));
    else if (msg.startsWith("Failed to open"))
        msgBox.setIconPixmap(QPixmap(":/Imgs/openFailed.png"));
    else
        msgBox.setIconPixmap(QPixmap(":/Imgs/bug.png"));
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    msgBox.setText(msg);
    msgBox.exec();

    DEBUGGER();
}

void    WindowNext::getSeriesMinMaxY_OPT(unsigned int &minY, unsigned int &maxY)
{
    DEBUGGER();
    minY = -1;
    maxY = 0;

    if (_autoScale->isChecked()) {
        for (int i = 0; i < _seriesMinMaxY_autoscale_OPT.size(); ++i)
            if (_checkBoxChannels[i].isChecked() && _seriesMinMaxY_autoscale_OPT[i].size())
                minY = std::min(minY, *(_seriesMinMaxY_autoscale_OPT[i].begin())),
                maxY = std::max(maxY, *(--(_seriesMinMaxY_autoscale_OPT[i].end())));
    } else {
        for (int i = 0; i < _seriesMinY_NoAutoscale_OPT.size(); ++i)
            if (_checkBoxChannels[i].isChecked())
                minY = std::min(minY, _seriesMinY_NoAutoscale_OPT[i]),
                maxY = std::max(maxY, _seriesMaxY_NoAutoscale_OPT[i]);
    }
    DEBUGGER();
}

void    WindowNext::getSeriesMinMaxY_IMU(short &minY, short &maxY, int index)
{
    DEBUGGER();

    minY = SHRT_MAX;
    maxY = SHRT_MIN;

    if (_autoScale->isChecked()) {
        for (int i = 0; i < _seriesMinMaxY_autoscale_IMU[index].size(); ++i)
            if (_seriesMinMaxY_autoscale_IMU[index][i].size())
                minY = std::min(minY, *(_seriesMinMaxY_autoscale_IMU[index][i].begin())),
                maxY = std::max(maxY, *(--(_seriesMinMaxY_autoscale_IMU[index][i].end())));
    } else {
        for (int i = 0; i < _seriesMinY_NoAutoscale_IMU[index].size(); ++i)
            minY = std::min(minY, _seriesMinY_NoAutoscale_IMU[index][i]),
            maxY = std::max(maxY, _seriesMaxY_NoAutoscale_IMU[index][i]);
    }
    DEBUGGER();
}

QString WindowNext::getExecutableGrandparentDirPath(void)
{
    QString executableDirPath = QCoreApplication::applicationDirPath();
    QDir parentDir(executableDirPath);

#  ifdef Q_OS_MAC
    parentDir.cdUp();
    parentDir.cdUp();
    parentDir.cdUp();
#  endif

    parentDir.cdUp();
    QString grandparentDirPath = parentDir.absolutePath();
    return grandparentDirPath;
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
    this->_durationSec2->setEnabled(true);
    this->_durationSec2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_lightIntensity2->setEnabled(true);
    this->_lightIntensity2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);
    this->_distance2->setEnabled(true);
    this->_distance2->setStyleSheet(MY_DEFINED_DEFAULT_ACTIVE_TEXT);

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
