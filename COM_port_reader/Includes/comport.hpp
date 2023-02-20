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
# include <QStringList>

class	ComPort
{
    public:
        ComPort(const QString &name, QGroupBox *groupbox);
        ~ComPort(void);

    public:
        void    	setPortName(QString &name);
        void    	setBaudRate(const QString &rate, QStringList &items);
        void    	setDataBits(const QString &bits, QStringList &items);
        void    	setParity(const QString &parity, QStringList &items);
        void    	setStopBits(const QString &bits, QStringList &items);
        void    	setFlowControl(const QString &flowcontrol, QStringList &items);

        const QString               getPortName(void) const {return _portName;}
        QSerialPort::BaudRate       getBaudRate(void) const {return _baudRate;}
        QSerialPort::DataBits       getDataBits(void) const {return _dataBits;}
        QSerialPort::Parity         getParity(void) const {return _parity;}
        QSerialPort::StopBits       getStopBits(void) const {return _stopBits;}
        QSerialPort::FlowControl	getFlowControl(void) const {return _flowControl;}
        
        //QCheckBox       *getCheckBox(void) const {return _checkBox;}
        QRadioButton	*getCheckBox(void) const {return _checkBox;}  //delete

    private:
        QString					_portName;
        QSerialPort::BaudRate	_baudRate;
        QSerialPort::DataBits	_dataBits;
        QSerialPort::Parity		_parity;
        QSerialPort::StopBits	_stopBits;
        QSerialPort::FlowControl	_flowControl;
        
        //QCheckBox       			*_checkBox;
        QRadioButton    			*_checkBox; //delete
        
    public: //temp
        QDialog         *_propertyWindow;
        QPushButton		*_cancel;
        QPushButton		*_setDefault;
        QPushButton		*_start;
};

#endif
