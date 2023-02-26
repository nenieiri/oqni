#include "windowsaveto.hpp"

WindowSaveTo::WindowSaveTo(MainWindow *parent)
    : QDialog(parent)
{
    QFileDialog dialog;
    QString     selectedDirectory;
    QScreen *screen = QApplication::primaryScreen();
    QSize screenSize = screen->size();
    int screenWidth = screenSize.width();
    int screenHeight = screenSize.height();
    int windowWidth = 500;
    int windowHeight = 155;    

    this->_buttonStart = nullptr;
    this->_buttonStop = nullptr;
    this->_buttonClose = nullptr;
    
    this->_selectedComPort = parent->getSelectedComPort();
    
    this->_showReadingPort1 = new QLabel("Read from:", this);
	this->_showReadingPort2 = new QLabel(this->_selectedComPort->getPortName(), this);
    this->_showSelectedDir1 = new QLabel("Save to:", this);
    
    dialog.setOption(QFileDialog::ShowDirsOnly);
    selectedDirectory = dialog.getExistingDirectory(this, tr("Save to"), \
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if (selectedDirectory == "")
		this->_showSelectedDir2 = nullptr;
    else
    {
		this->_showSelectedDir2 = new QLabel(selectedDirectory, this);
		this->_showSelectedDir2->setToolTip(selectedDirectory);
    }

    this->_timer1 = new QLabel("Duration:", this);
    this->_timer2 = new QLabel("seconds  ", this);
    
    this->_lineEdit = new QLineEdit(this);
    this->_finishMsgLabel = new QLabel("", this);
    
    this->setModal(true);
    
    this->setGeometry((screenWidth - windowWidth) / 2, \
    						(screenHeight - windowHeight) / 2 - 300, \
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

WindowSaveTo::~WindowSaveTo()
{
    delete _buttonClose;
    delete _buttonStart;
    delete _buttonStop;
    delete _showReadingPort1;
    delete _showReadingPort2;
    delete _showSelectedDir1;
    delete _showSelectedDir2;
    delete _timer1;
    delete _timer2;
    delete _lineEdit;
    delete _finishMsgLabel;
}

/* -------------------------------- Setters --------------------------------- */

void		WindowSaveTo::setButtonStart(QPushButton *buttonStart)
{
    this->_buttonStart = buttonStart;
    this->_buttonStart->setEnabled(false);
	this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
    connect(this->_buttonStart, &QPushButton::clicked, this,
		[=](void)
		{
            if (this->_durationTimerValue == 0)
                return ;
			this->setMinimumSize(500, 700);
			this->setMaximumSize(500, 700);
			this->_buttonClose->setEnabled(false);
			this->_buttonClose->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
			this->_buttonStart->setEnabled(false);
			this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
			this->_buttonStop->setEnabled(true);
			this->_buttonStop->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
			this->_lineEdit->setEnabled(false);
			this->_lineEdit->setStyleSheet("background-color: #D3D3D3; padding: 0 5px; color: blue;");
            
            this->_threadDisplayTimer = new ThreadDisplayTimer(this->_durationTimerValue, this);
            this->_threadDisplayTimer->start();
            
            this->_threadReader = new ThreadReader(_selectedComPort, _showSelectedDir2->text(), _threadDisplayTimer);
            this->_threadReader->start();
            
            this->_finishMsgLabel->hide();
			connect(this->_threadDisplayTimer, &ThreadDisplayTimer::finishedSignal, this, &WindowSaveTo::onThreadDisplayTimerFinished);
		});
}

void		WindowSaveTo::setButtonStop(QPushButton *buttonStop)
{
    this->_buttonStop = buttonStop;
    this->_buttonStop->setEnabled(false);
    this->_buttonStop->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
    connect(this->_buttonStop, &QPushButton::clicked, this,
		[=](void)
		{
            this->setMinimumSize(500, 155);
            this->setMaximumSize(500, 155);
            this->_buttonClose->setEnabled(true);
            this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
            this->_buttonStart->setEnabled(true);
            this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
            this->_buttonStop->setEnabled(false);
            this->_buttonStop->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
            this->_lineEdit->setEnabled(true);
            this->_lineEdit->setStyleSheet("background-color: white; padding: 0 5px; color: blue;");
            
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

void		WindowSaveTo::setButtonClose(QPushButton *buttonClose)
{
    this->_buttonClose = buttonClose;
    connect(this->_buttonClose, &QPushButton::clicked, this,
        [=](void)
        {
            this->close();
        });
}

/* -------------------------------- Getters --------------------------------- */

QPushButton	*WindowSaveTo::getButtonStart() const
{
    return (this->_buttonStart);
}

QPushButton	*WindowSaveTo::getButtonStop() const
{
    return (this->_buttonStop);
}

QPushButton	*WindowSaveTo::getButtonClose() const
{
    return (this->_buttonClose);
}

QLineEdit	*WindowSaveTo::getLineEdit() const
{
    return (this->_lineEdit);
}

QLabel	*WindowSaveTo::getFinishMsgLabel() const
{
    return (this->_finishMsgLabel);
}

QLabel	*WindowSaveTo::getShowReadingPort2() const
{
    return (this->_showReadingPort2);
}

QLabel	*WindowSaveTo::getShowSelectedDir2() const
{
    return (this->_showSelectedDir2);
}

/* ---------------------------- Member functions ---------------------------- */

void    WindowSaveTo::setParametersDesign(void)
{
    this->_showReadingPort1->setGeometry(10, 10, 100, 30);
    this->_showReadingPort2->setGeometry(120, 10, 480, 30);
    this->_showReadingPort2->setStyleSheet("font-size: 14px; color: blue;");
    
    this->_showSelectedDir1->setGeometry(10, 40, 100, 30);
    this->_showSelectedDir2->setGeometry(120, 40, 480, 30);
    this->_showSelectedDir2->setStyleSheet("font-size: 14px; color: blue;");

    this->_timer1->setGeometry(10, 70, 100, 30);
    this->_timer2->setGeometry(210, 70, 100, 30);
    this->_timer2->setStyleSheet("font-size: 14px; color: blue;");

    this->_lineEdit->setPlaceholderText("enter here");
    this->_lineEdit->setGeometry(120, 70, 83, 30);
    this->_lineEdit->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_lineEdit->setToolTip("Please enter only numeric values.");
    this->_lineEdit->setMaxLength(4);
    this->_lineEdit->setAlignment(Qt::AlignCenter);
    
    this->_finishMsgLabel->setGeometry(285, 65, 160, 40);
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
}

void   WindowSaveTo::onThreadDisplayTimerFinished(void)
{
	this->setMinimumSize(500, 155);
	this->setMaximumSize(500, 155);
	this->_buttonClose->setEnabled(true);
	this->_buttonClose->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
	this->_buttonStart->setEnabled(true);
	this->_buttonStart->setStyleSheet(MY_DEFINED_DEFAULT_BUTTON);
	this->_buttonStop->setEnabled(false);
	this->_buttonStop->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
	this->_lineEdit->setEnabled(true);
	this->_lineEdit->setStyleSheet("background-color: white; padding: 0 5px; color: blue;");

    this->_threadDisplayTimer->wait();
	delete this->_threadDisplayTimer;
    this->_threadDisplayTimer = nullptr;
    
    this->_threadReader->requestInterruption();
    this->_threadReader->wait();
    delete this->_threadReader;
    this->_threadReader = nullptr;
    
    this->_finishMsgLabel->setText("Finished");
    this->_finishMsgLabel->show();
}
