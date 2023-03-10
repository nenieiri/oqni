#include "threadreader.hpp"

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
    this->_bytesOCH = 1; // Channels numbers bytes
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

const char	ThreadReader::getBytesPA() const
{
    return (this->_bytesPA);
}

const char	ThreadReader::getBytesID() const
{
    return (this->_bytesID);
}

const char	ThreadReader::getBytesCO() const
{
    return (this->_bytesCO);
}

const char	ThreadReader::getBytesCH() const
{
    return (this->_bytesCH);
}

const char	ThreadReader::getBytesOCH() const
{
    return (this->_bytesOCH);
}

const char	ThreadReader::getNumOfCH() const
{
    return (this->_numOfCH);
}

const char	ThreadReader::getSizeOfCH() const
{
    return (this->_sizeOfCH);
}

QByteArray	&ThreadReader::getDataRead()
{
    return (this->_dataRead);
}

qint64	ThreadReader::getStartTime() const
{
    return (this->_startTime);
}

void    ThreadReader::run()
{
    QSerialPort port;
    int			bytesTillData;
    int			bytesTotal;
    qint64      currentTime;

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
    
    bytesTillData = _bytesPA + _bytesID + _bytesCO + _bytesCH + _bytesOCH;
    bytesTotal = bytesTillData + _numOfCH * _sizeOfCH;
    emit protocolConfigDataIsReady();
    
    while (!isInterruptionRequested())
    {
        if (port.waitForReadyRead(MY_READY_READ_TIME))
        {
            _dataRead.append(port.read(bytesTotal));
            currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
            _dataRead.append(QByteArray::fromRawData(reinterpret_cast<const char*>(&currentTime), sizeof(qint64)));
            _dataRead.append(QString::number(this->_threadDisplayTimer->getCurrentImgLabel()).toUInt());
            emit lastRowOfData(_dataRead.right(bytesTotal + 8 + 1)); // 8 - sizeof time; 1 - sizeof label
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
