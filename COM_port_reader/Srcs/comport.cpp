#include "comport.hpp"

ComPort::ComPort(const QString &name, QGroupBox *groupbox) : \
                _portName(name), \
                _baudRate(QSerialPort::Baud115200), \
                _dataBits(QSerialPort::Data8), \
                _parity(QSerialPort::NoParity), \
                _stopBits(QSerialPort::OneStop), \
                _flowControl(QSerialPort::NoFlowControl)
{
    //this->_checkBox = new QCheckBox(this->_portName, groupbox);
    this->_checkBox = new QRadioButton(this->_portName, groupbox); //delete
}

ComPort::~ComPort()
{
    delete this->_checkBox;
}

void	ComPort::setPortName(QString &name)
{
    _portName = name;
}

void	ComPort::setBaudRate(const QString &rate, QStringList &items)
{
    unsigned int    index;
    index = items.indexOf(rate);
    _baudRate = static_cast<QSerialPort::BaudRate>(index);
}

void	ComPort::setDataBits(const QString &bits, QStringList &items)
{
    unsigned int    index;
    index = items.indexOf(bits);
    _dataBits = static_cast<QSerialPort::DataBits>(index);
}

void	ComPort::setParity(const QString &parity, QStringList &items)
{
    unsigned int    index;
    index = items.indexOf(parity);
    _parity = static_cast<QSerialPort::Parity>(index);
}

void	ComPort::setStopBits(const QString &bits, QStringList &items)
{
    unsigned int    index;
    index = items.indexOf(bits);
    _stopBits = static_cast<QSerialPort::StopBits>(index);
}

void	ComPort::setFlowControl(const QString &flowcontrol, QStringList &items)
{
    unsigned int    index;
    index = items.indexOf(flowcontrol);
    _flowControl = static_cast<QSerialPort::FlowControl>(index);
}
