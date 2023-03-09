#include "threadreader.hpp"
//#include <qendian.h>
#include <QtEndian>

ThreadReader::ThreadReader(ComPort *comPort, ThreadDisplayTimer *threadDisplayTimer)
			: _comPort(comPort)
{
    this->_fileCreationDate = QDateTime::currentDateTime().toString("yyMMdd");
    this->_fileCreationTime = QDateTime::currentDateTime().toString("hhmmss");
    this->_fileNamePrefix += "/" + _fileCreationDate + "_" + _fileCreationTime + "_DGT";
    this->_threadDisplayTimer = threadDisplayTimer;
    this->_bytesPA = 4;  // Preamble bytes
    this->_bytesID = 1;  // ID bytes
    this->_bytesCO = 1;  // Counter bytes
    this->_bytesCH = 1;  // Channels bytes
    this->_bytesOCH = 1; // One channel bytes
}

ThreadReader::~ThreadReader()
{
}

const QString	&ThreadReader::getFileNamePrefix() const
{
    return (this->_fileNamePrefix);
}

const QString	&ThreadReader::getFileCreationDate() const
{
    return (this->_fileCreationDate);
}

const QString	&ThreadReader::getFileCreationTime() const
{
    return (this->_fileCreationTime);
}

void    ThreadReader::run()
{
    QSerialPort port;
    int			bytesTillData;
    int			bytesTotal;
    qint64      currentTime;
    
//    char            id;
//    unsigned char   counter;
//    unsigned int    data;
    
//    unsigned char   oldCounter[2];

    port.setPort(_comPort->getPort());
    port.setBaudRate(_comPort->getBaudRate());
    port.setDataBits(_comPort->getDataBits());
    port.setParity(_comPort->getParity());
    port.setStopBits(_comPort->getStopBits());
    port.setFlowControl(_comPort->getFlowControl());

    if (!port.open(QIODevice::ReadWrite))
    {
        qDebug() << "Faild to open serial port!";
        return ;
    }
    if (requestPortConfig(port) == -1)
        return ;
    if (requestPortStart(port) == -1)
        return ;
    
//    _data[0].push_back("time_millisec,led11,led12,led13,label\n");
//    _data[1].push_back("time_millisec,led21,led22,led23,label\n");
    
    bytesTillData = _bytesPA + _bytesID + _bytesCO + _bytesCH + _bytesOCH;
    bytesTotal = bytesTillData + _numOfCH * _sizeOfCH;
    
    while (!isInterruptionRequested())
    {
        if (port.waitForReadyRead(MY_READY_READ_TIME))
        {
            currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
            _dataRead.append(QByteArray::fromRawData(reinterpret_cast<const char*>(&currentTime), sizeof(qint64)));
            _dataRead.append(port.read(bytesTotal));
            _dataRead.append(QString::number(this->_threadDisplayTimer->getCurrentImgLabel()).toUInt());
            emit stringAdded();
        }
        else
            break ;
    }
    this->stopAndClosePort(port);
}

void    ThreadReader::stopAndClosePort(QSerialPort &port)
{
    QByteArray  dataWrite;
    
    dataWrite.append(static_cast<char>(-1));
    port.write(dataWrite);
    port.flush();
    port.close();
}

int    ThreadReader::requestPortConfig(QSerialPort &port)
{
    QByteArray  dataWrite;
    QByteArray  dataRead;
    int         preamble = 0xaa55aa55;
    
    dataWrite.append(static_cast<char>(127)); // Requesting configuration
    port.write(dataWrite);
    if (port.waitForReadyRead(MY_READY_READ_TIME))
        dataRead = port.read(12);
    else
    {
		this->stopAndClosePort(port);
        return -1;
    }
    if (dataRead.mid(0, 4).toHex().toUInt(nullptr, 16) != preamble) // checking if the first 4 bytes are 'aa55aa55'
    {
        qDebug() << "'aa55aa55' is not recevied, when try to config.";
		this->stopAndClosePort(port);
        return -1;
    }
    this->_numOfCH = dataRead.mid(9, 1).toHex().toUInt(nullptr, 16);  // Number of channels following (N)
    this->_sizeOfCH = dataRead.mid(10, 1).toHex().toUInt(nullptr, 16); // Number of bytes in one channel data (M)
    return 0;
}

int    ThreadReader::requestPortStart(QSerialPort &port)
{
    QByteArray  dataWrite;
    QByteArray  dataRead;
    int         preamble = 0xaa55aa55;

    dataWrite.append(static_cast<char>(0)); // First byte
    dataWrite.append(static_cast<char>(6)); // Second byte
    
    port.write(dataWrite); 
    if (port.waitForReadyRead(MY_READY_READ_TIME))
    {
        this->_threadDisplayTimer->start();
        this->_startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        dataRead = port.read(7);
    }
    else
    {
        this->stopAndClosePort(port);
        return -1;
    }
    if (dataRead.mid(0, 4).toHex().toUInt(nullptr, 16) != preamble) // checking if the first 4 bytes are 'aa55aa55'
    {
        qDebug() << "'aa55aa55' is not recevied, when try to start.";
		this->stopAndClosePort(port);
        return -1;
    }
    return (0);
}

//qDebug() << "Time is:" << qFromLittleEndian<qint64>(_dataRead.mid(0,8).constData());
//qDebug() << "Preamble is:" << qFromBigEndian<unsigned int>(_dataRead.mid(8,4).constData());
//qDebug() << "ID is:" << qFromBigEndian<unsigned char>(_dataRead.mid(12,1).constData());
//qDebug() << "Counter is:" << qFromBigEndian<unsigned char>(_dataRead.mid(13,1).constData());
//qDebug() << "Channel num is:" << qFromBigEndian<unsigned char>(_dataRead.mid(14,1).constData());
//qDebug() << "Channel size is:" << qFromBigEndian<unsigned char>(_dataRead.mid(15,1).constData());
//qDebug() << "Data1 is:" << qFromLittleEndian<unsigned int>(_dataRead.mid(16,4).constData());
//qDebug() << "Data2 is:" << qFromLittleEndian<unsigned int>(_dataRead.mid(20,4).constData());
//qDebug() << "Data3 is:" << qFromLittleEndian<unsigned int>(_dataRead.mid(24,4).constData());
//qDebug() << "Labvel is:" << qFromLittleEndian<unsigned char>(_dataRead.mid(28,1).constData());
