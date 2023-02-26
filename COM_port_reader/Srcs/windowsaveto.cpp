#include "windowsaveto.hpp"

WindowSaveTo::WindowSaveTo(QWidget *parent)
    : QDialog(parent)
{
    
}

WindowSaveTo::~WindowSaveTo()
{
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
