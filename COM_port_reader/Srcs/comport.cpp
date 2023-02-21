#include "comport.hpp"

ComPort::ComPort(const QSerialPortInfo &port, QGroupBox *groupbox) : \
                _port(port), \
                _portName(port.portName()), \
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

void	ComPort::setBaudRate(const QString &rate, QStringList &items)
{
    (void)&items;
    _baudRate = (QSerialPort::BaudRate)rate.toUInt();
}

void	ComPort::setDataBits(const QString &bits, QStringList &items)
{
    (void)&items;
    _dataBits = (QSerialPort::DataBits)bits.toUInt();
}

void	ComPort::setParity(const QString &parity, QStringList &items)
{
    unsigned int index = items.indexOf(parity);
    if (index)
        index++;
    _parity = (QSerialPort::Parity)index;
}

void	ComPort::setStopBits(const QString &bits, QStringList &items)
{
    unsigned int index = items.indexOf(bits) + 1;
    if (index == 2)
        index++;
    else if (index == 3)
        index--;
    _stopBits = static_cast<QSerialPort::StopBits>(index);
}

void	ComPort::setFlowControl(const QString &flowcontrol, QStringList &items)
{
    unsigned int index = items.indexOf(flowcontrol);
    _flowControl = (QSerialPort::FlowControl)index;
}
