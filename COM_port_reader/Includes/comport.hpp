#ifndef COMPORT_HPP
# define COMPORT_HPP

# include <QString>

class ComPort
{
    public:
        ComPort(QString &name) : \
        _portName(name), \
        _baudRate(9600), \
        _dataBits(8), \
        _parity("None"), \
        _stopBits(1), \
        _flowControl("None")
        {}

        ~ComPort(void);

    public:
        void            setPortName(QString &name) {_portName = name;}
        void            setBaudRate(unsigned int rate) {_baudRate = rate;}
        void            setDataBits(unsigned short bits) {_dataBits = bits;}
        void            setParity(QString &parity) {_parity = parity;}
        void            setStopBits(float bits) {_stopBits = bits;}
        void            setFlowControl(QString &flowcontrol) {_flowControl = flowcontrol;}

        const QString   getPortName(void) const {return _portName;}
        unsigned int    getBaudRate(void) const {return _baudRate;}
        unsigned short  getDataBits(void) const {return _dataBits;}
        const QString   getParity(void) const {return _parity;}
        float           getStopBits(void) const {return _stopBits;}
        const QString   getFlowControl(void) const {return _flowControl;}

    private:
        QString         _portName;
        unsigned int    _baudRate;
        unsigned short  _dataBits;
        QString         _parity;
        float           _stopBits;
        QString         _flowControl;
};

#endif
