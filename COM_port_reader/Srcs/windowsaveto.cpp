#include "windowsaveto.hpp"

WindowSaveTo::WindowSaveTo(QWidget *parent)
    : QDialog(parent)
{
    this->_buttonStart = nullptr;
    this->_buttonStop = nullptr;
    this->_buttonClose = nullptr;
    
    this->_showReadingPort1 = new QLabel("Read from:", this);
//    this->_showReadingPort2 = new QLabel(comPort->getPortName(), this);
    this->_showSelectedDir1 = new QLabel("Save to:", this);
 //   this->_showSelectedDir2 = new QLabel(selectedDirectory, this);
    this->_timer1 = new QLabel("Duration:", this);
    this->_timer2 = new QLabel("seconds  ", this);
    
    this->_lineEdit = new QLineEdit(this);
    this->_finishMsgLabel = new QLabel("", this);
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

void		WindowSaveTo::setButtonStart(const QPushButton *buttonStart)
{
    this->_buttonStart = buttonStart;
}

void		WindowSaveTo::setButtonStop(const QPushButton *buttonStop)
{
    this->_buttonStart = buttonStop;
}

void		WindowSaveTo::setButtonClose(const QPushButton *buttonClose)
{
    this->_buttonStart = buttonClose;
}

/* -------------------------------- Getters --------------------------------- */

const QPushButton	*WindowSaveTo::getButtonStart() const
{
    return (this->_buttonStart);
}

const QPushButton	*WindowSaveTo::getButtonStop() const
{
    return (this->_buttonStop);
}

const QPushButton	*WindowSaveTo::getButtonClose() const
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

QLabel	*WindowSaveTo::getShowReadingPort1() const
{
    return (this->_showReadingPort1);
}

QLabel	*WindowSaveTo::getShowReadingPort2() const
{
    return (this->_showReadingPort1);
}

QLabel	*WindowSaveTo::getShowSelectedDir1() const
{
    return (this->_showSelectedDir1);
}

QLabel	*WindowSaveTo::getShowSelectedDir2() const
{
    return (this->_showSelectedDir2);
}

QLabel	*WindowSaveTo::getTimer1() const
{
    return (this->_timer1);
}

QLabel	*WindowSaveTo::getTimer2() const
{
    return (this->_timer2);
}
