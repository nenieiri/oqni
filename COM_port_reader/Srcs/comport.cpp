#include "comport.hpp"

ComPort::ComPort(const QSerialPortInfo &port, QGroupBox *groupbox) : \
                _port(port), \
                _portName(port.portName()), \
                _baudRate(QSerialPort::Baud1200), \
                _dataBits(QSerialPort::Data5), \
                _parity(QSerialPort::NoParity), \
                _stopBits(QSerialPort::OneStop), \
                _flowControl(QSerialPort::NoFlowControl), \
                _baudRateIndex(-1), \
                _dataBitsIndex(-1), \
                _parityIndex(-1), \
                _stopBitsIndex(-1), \
                _flowControlIndex(-1) \
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
    _baudRateIndex = items.indexOf(rate);
    _baudRate = (QSerialPort::BaudRate)rate.toUInt();
}

void	ComPort::setDataBits(const QString &bits, QStringList &items)
{
    _dataBitsIndex = items.indexOf(bits);
    _dataBits = (QSerialPort::DataBits)bits.toUInt();
}

void	ComPort::setParity(const QString &parity, QStringList &items)
{
    _parityIndex = items.indexOf(parity);
    if (_parityIndex)
        _parityIndex++;
    _parity = (QSerialPort::Parity)_parityIndex;
    _parityIndex = items.indexOf(parity);
}

void	ComPort::setStopBits(const QString &bits, QStringList &items)
{
    _stopBitsIndex = items.indexOf(bits) + 1;
    if (_stopBitsIndex == 2)
        _stopBitsIndex++;
    else if (_stopBitsIndex == 3)
        _stopBitsIndex--;
    _stopBits = static_cast<QSerialPort::StopBits>(_stopBitsIndex);
    _stopBitsIndex = items.indexOf(bits);
}

void	ComPort::setFlowControl(const QString &flowcontrol, QStringList &items)
{
    _flowControlIndex = items.indexOf(flowcontrol);
    _flowControl = (QSerialPort::FlowControl)_flowControlIndex;
}
