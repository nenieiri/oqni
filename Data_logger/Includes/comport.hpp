#ifndef COMPORT_HPP
# define COMPORT_HPP

# include <QString>
# include <QCheckBox>
# include <QRadioButton>
# include <QGroupBox>
# include <QDebug>
# include <QDialog>
# include <QPushButton>
# include <QSerialPort>
# include <QSerialPortInfo>
# include <QStringList>
# include <QToolButton>

class	ComPort
{
    public:
        ComPort(const QSerialPortInfo &port, QGroupBox *groupbox);
        ~ComPort(void);

    public:
        void    	setBaudRate(const QString &rate, QStringList &items);
        void    	setDataBits(const QString &bits, QStringList &items);
        void    	setParity(const QString &parity, QStringList &items);
        void    	setStopBits(const QString &bits, QStringList &items);
        void    	setFlowControl(const QString &flowcontrol, QStringList &items);

        const QSerialPortInfo       &getPort(void) const {return _port;}
        const QString               &getPortName(void) const {return _portName;}
        QSerialPort::BaudRate       getBaudRate(void) const {return _baudRate;}
        QSerialPort::DataBits       getDataBits(void) const {return _dataBits;}
        QSerialPort::Parity         getParity(void) const {return _parity;}
        QSerialPort::StopBits       getStopBits(void) const {return _stopBits;}
        QSerialPort::FlowControl	getFlowControl(void) const {return _flowControl;}

        unsigned int                getBaudRateIndex(void) const {return _baudRateIndex;}
        unsigned int                getDataBitsIndex(void) const {return _dataBitsIndex;}
        unsigned int                getParityIndex(void) const {return _parityIndex;}
        unsigned int                getStopBitsIndex(void) const {return _stopBitsIndex;}
        unsigned int                getFlowControlIndex(void) const {return _flowControlIndex;}
        
        QRadioButton                *getCheckBox(void) const {return _radioButton;}
        QToolButton                 *getToolButton(void) const {return _toolButton;}

    private:
        const QSerialPortInfo       _port;
        const QString               _portName;
        QSerialPort::BaudRate       _baudRate;
        QSerialPort::DataBits       _dataBits;
        QSerialPort::Parity         _parity;
        QSerialPort::StopBits       _stopBits;
        QSerialPort::FlowControl    _flowControl;

        unsigned int                _baudRateIndex;
        unsigned int                _dataBitsIndex;
        unsigned int                _parityIndex;
        unsigned int                _stopBitsIndex;
        unsigned int                _flowControlIndex;

        QRadioButton    			*_radioButton;

        QToolButton                 *_toolButton;
        
    public: //temp
        QDialog         *_windowProperty;
        QPushButton		*_cancelProperties;
        QPushButton		*_setDefaultProperties;
        QPushButton		*_saveProperies;
};

#endif
