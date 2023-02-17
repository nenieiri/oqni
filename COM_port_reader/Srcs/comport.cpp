#include "comport.hpp"

ComPort::ComPort(const QString &name, QGroupBox *groupbox) : \
                _portName(name), \
                _baudRate(9600), \
                _dataBits(8), \
                _parity("None"), \
                _stopBits(1), \
                _flowControl("None")
{
    //this->_checkBox = new QCheckBox(this->_portName, groupbox);
    this->_checkBox = new QRadioButton(this->_portName, groupbox); //delete
}

ComPort::~ComPort()
{
    delete this->_checkBox;
}
