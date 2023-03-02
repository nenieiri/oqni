#include "windownext.hpp"

WindowNext::WindowNext(MainWindow *parent)
    : QDialog(parent)
{
    QScreen     *screen = QApplication::primaryScreen();
    QSize       screenSize = screen->size();
    int         screenWidth = screenSize.width();
    int         screenHeight = screenSize.height();
    int         windowWidth = 600;
    int         windowHeight = 195;    

    this->_buttonStart = nullptr;
    this->_buttonStop = nullptr;
    this->_buttonClose = nullptr;
    
    this->_selectedComPort = parent->getSelectedComPort();
    
    this->_showReadingPort1 = new QLabel("Read from:", this);
	this->_showReadingPort2 = new QLabel(this->_selectedComPort->getPortName(), this);
    
    this->_showSelectedDir1 = new QLabel("Save to:", this);
    this->_showSelectedDir2 = new QLineEdit(this);
    this->_selectedDirectory = QCoreApplication::applicationDirPath() + "/Recordings/";
    this->_showSelectedDir2->setText(_selectedDirectory);

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

WindowNext::~WindowNext()
{
    delete _buttonClose;
    delete _buttonStart;
    delete _buttonStop;
    delete _buttonBrowse;
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

void		WindowNext::setButtonStart(QPushButton *buttonStart)
{
    this->_buttonStart = buttonStart;
    this->_buttonStart->setEnabled(false);
	this->_buttonStart->setStyleSheet("border-radius: 6px; background-color: #D3D3D3;");
    connect(this->_buttonStart, &QPushButton::clicked, this,
		[=](void)
		{
            if (this->_durationTimerValue == 0)
                return ;
			this->setMinimumSize(600, 700);
			this->setMaximumSize(600, 700);
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
            this->setMinimumSize(600, 195);
            this->setMaximumSize(600, 195);
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

void		WindowNext::setButtonClose(QPushButton *buttonClose)
{
    this->_buttonClose = buttonClose;
    connect(this->_buttonClose, &QPushButton::clicked, this,
        [=](void)
        {
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

/* -------------------------------- Getters --------------------------------- */

QPushButton	*WindowNext::getButtonStart() const
{
    return (this->_buttonStart);
}

QPushButton	*WindowNext::getButtonStop() const
{
    return (this->_buttonStop);
}

QPushButton	*WindowNext::getButtonClose() const
{
    return (this->_buttonClose);
}

QLineEdit	*WindowNext::getLineEdit() const
{
    return (this->_lineEdit);
}

QLabel	*WindowNext::getFinishMsgLabel() const
{
    return (this->_finishMsgLabel);
}

QLabel	*WindowNext::getShowReadingPort2() const
{
    return (this->_showReadingPort2);
}

/* ---------------------------- Member functions ---------------------------- */

void    WindowNext::setParametersDesign(void)
{
    this->_showReadingPort1->setGeometry(10, 10, 100, 30);
    this->_showReadingPort2->setGeometry(120, 10, 480, 30);
    this->_showReadingPort2->setStyleSheet("font-size: 14px; color: blue;");
    
    this->_showSelectedDir1->setGeometry(10, 50, 100, 30);
    this->_showSelectedDir2->setGeometry(120, 50, 360, 30);
    this->_showSelectedDir2->setEnabled(false);
    this->_showSelectedDir2->setCursorPosition(0);
    this->_showSelectedDir2->setStyleSheet("font-size: 14px; background-color: #D3D3D3; padding: 0 5px; color: blue;");
    this->_showSelectedDir2->setToolTip(_selectedDirectory);

    this->_timer1->setGeometry(10, 90, 100, 30);
    this->_timer2->setGeometry(210, 90, 100, 30);
    this->_timer2->setStyleSheet("font-size: 14px; color: blue;");

    this->_lineEdit->setPlaceholderText("enter here");
    this->_lineEdit->setGeometry(120, 90, 83, 30);
    this->_lineEdit->setStyleSheet("background: white; font-size: 14px; padding: 0 5px; color: blue;");
    this->_lineEdit->setToolTip("Please enter only numeric values.");
    this->_lineEdit->setMaxLength(4);
    this->_lineEdit->setAlignment(Qt::AlignCenter);
    
    this->_finishMsgLabel->setGeometry(285, 85, 160, 40);
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

void   WindowNext::onThreadDisplayTimerFinished(void)
{
	this->setMinimumSize(600, 195);
	this->setMaximumSize(600, 195);
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
