#include "comport.hpp"
#include "debugger.hpp"

ComPort::ComPort(const QSerialPortInfo &port, QGroupBox *groupbox) : \
                _port(port), \
                _portName(port.portName()), \
                _baudRate(QSerialPort::Baud115200), \
                _dataBits(QSerialPort::Data8), \
                _parity(QSerialPort::NoParity), \
                _stopBits(QSerialPort::OneStop), \
                _flowControl(QSerialPort::NoFlowControl), \
                _baudRateIndex(7), \
                _dataBitsIndex(3), \
                _parityIndex(0), \
                _stopBitsIndex(0), \
                _flowControlIndex(0) \
{
    DEBUGGER();
    
    this->_radioButton = new QRadioButton(this->_portName, groupbox); //delete

    this->_toolButton = new QToolButton(groupbox);
    this->_toolButton->setIcon(QIcon(":/Imgs/config.png"));
    this->_toolButton->setIconSize(QSize(25, 25));
    this->_toolButton->setCursor(Qt::PointingHandCursor);
    this->_toolButton->setToolTip("Set properties");
   	this->_toolButton->setStyleSheet("QToolButton { border: 0px; } \
									  QToolButton:hover { border-radius: 5px; border: 1px solid red; background: yellow;} \
									  QToolTip { font-size: 14pt; border: 0px; }");
    DEBUGGER();
}

ComPort::~ComPort()
{
    DEBUGGER();

    delete this->_radioButton;
    delete this->_toolButton;

    DEBUGGER();
}

void	ComPort::setBaudRate(const QString &rate, QStringList &items)
{
    
    DEBUGGER();

    _baudRateIndex = items.indexOf(rate);
    _baudRate = (QSerialPort::BaudRate)rate.toUInt();

    DEBUGGER();
}

void	ComPort::setDataBits(const QString &bits, QStringList &items)
{
    DEBUGGER();

    _dataBitsIndex = items.indexOf(bits);
    _dataBits = (QSerialPort::DataBits)bits.toUInt();

    DEBUGGER();
}

void	ComPort::setParity(const QString &parity, QStringList &items)
{
    DEBUGGER();
    
    _parityIndex = items.indexOf(parity);
    if (_parityIndex)
        _parityIndex++;
    _parity = (QSerialPort::Parity)_parityIndex;
    _parityIndex = items.indexOf(parity);
    
    DEBUGGER();
}

void	ComPort::setStopBits(const QString &bits, QStringList &items)
{
    DEBUGGER();

    _stopBitsIndex = items.indexOf(bits) + 1;
    if (_stopBitsIndex == 2)
        _stopBitsIndex++;
    else if (_stopBitsIndex == 3)
        _stopBitsIndex--;
    _stopBits = static_cast<QSerialPort::StopBits>(_stopBitsIndex);
    _stopBitsIndex = items.indexOf(bits);

    DEBUGGER();
}

void	ComPort::setFlowControl(const QString &flowcontrol, QStringList &items)
{
    DEBUGGER();

    _flowControlIndex = items.indexOf(flowcontrol);
    _flowControl = (QSerialPort::FlowControl)_flowControlIndex;

    DEBUGGER();
}
