#include "threadreader.hpp"

ThreadReader::ThreadReader(ComPort *comPort, ThreadDisplayTimer *threadDisplayTimer, QCheckBox *showPic)
			: _comPort(comPort) \
            , _showPic(showPic)
{
    ERROR_LOGGER();
    
    this->_fileCreationDate = QDateTime::currentDateTime().toString("yyMMdd");
    this->_fileCreationTime = QDateTime::currentDateTime().toString("hhmmss");
    this->_fileNamePrefix += "/" + _fileCreationDate + "_" + _fileCreationTime + "_OPT";
    this->_threadDisplayTimer = threadDisplayTimer;
    this->_bytesPA = 4;  // Preamble bytes
    this->_bytesID = 1;  // ID bytes
    this->_bytesCO = 1;  // Counter bytes
    this->_bytesCH = 1;  // Channels bytes
    this->_bytesOCH = 1; // Channels numbers bytes
    
    ERROR_LOGGER();
}

ThreadReader::~ThreadReader()
{
    ERROR_LOGGER();
}

const QString	&ThreadReader::getFileNamePrefix() const
{
    ERROR_LOGGER();
    return (this->_fileNamePrefix);
}

const QString	&ThreadReader::getFileCreationDate() const
{
    ERROR_LOGGER();
    return (this->_fileCreationDate);
}

const QString	&ThreadReader::getFileCreationTime() const
{
    ERROR_LOGGER();
    return (this->_fileCreationTime);
}

const char	ThreadReader::getBytesPA() const
{
    ERROR_LOGGER();
    return (this->_bytesPA);
}

const char	ThreadReader::getBytesID() const
{
    ERROR_LOGGER();
    return (this->_bytesID);
}

const char	ThreadReader::getBytesCO() const
{
    ERROR_LOGGER();
    return (this->_bytesCO);
}

const char	ThreadReader::getBytesCH() const
{
    ERROR_LOGGER();
    return (this->_bytesCH);
}

const char	ThreadReader::getBytesOCH() const
{
    ERROR_LOGGER();
    return (this->_bytesOCH);
}

const char	ThreadReader::getNumOfCH() const
{
    ERROR_LOGGER();
    return (this->_numOfCH);
}

const char	ThreadReader::getSizeOfCH() const
{
    ERROR_LOGGER();
    return (this->_sizeOfCH);
}

const char	ThreadReader::getNumOfOS() const
{
    ERROR_LOGGER();
    return (this->_numOfOS);
}

QByteArray	&ThreadReader::getDataRead()
{
    ERROR_LOGGER();
    return (this->_dataRead);
}

qint64	ThreadReader::getStartTime() const
{
    ERROR_LOGGER();
    return (this->_startTime);
}

void    ThreadReader::run()
{
    ERROR_LOGGER();
    
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
        ERROR_LOGGER();
        return ;
    }
    if (requestPortConfig(port) == -1)
    {
        ERROR_LOGGER();
        return ;
    }
    if (requestPortStart(port) == -1)
    {
        ERROR_LOGGER();
        return ;
    }
    
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
            _dataRead.append(_showPic->isChecked() ? this->_threadDisplayTimer->getCurrentImgLabel() : 0); // when _showPic checkbox is not checked, label = 0
            emit lastRowOfData(_dataRead.right(bytesTotal + 8 + 1)); // 8 - sizeof time; 1 - sizeof label
        }
        else
            break ;
    }
    this->stopAndClosePort(port);
    
    ERROR_LOGGER();
}

void    ThreadReader::stopAndClosePort(QSerialPort &port)
{
    ERROR_LOGGER();
    
    QByteArray  dataWrite;
    
    dataWrite.append(static_cast<char>(-1));
    port.write(dataWrite);
    port.flush();
    port.close();
    
    ERROR_LOGGER();
}

int    ThreadReader::requestPortConfig(QSerialPort &port)
{
    ERROR_LOGGER();
    
    QByteArray  dataWrite;
    QByteArray  dataRead;
    int         preamble = 0xaa55aa55;
    
    dataWrite.append(static_cast<char>(127)); // Requesting configuration
    port.write(dataWrite);
    if (port.waitForReadyRead(MY_READY_READ_TIME))
        dataRead = port.read(12);
    else
    {
        qDebug() << "TimeOut while waiting for configs";
		this->stopAndClosePort(port);
        ERROR_LOGGER();
        return -1;
    }
    if (dataRead.mid(0, 4).toHex().toUInt(nullptr, 16) != preamble) // checking if the first 4 bytes are 'aa55aa55'
    {
        qDebug() << "'aa55aa55' is not recevied, when try to config.";
		this->stopAndClosePort(port);
        ERROR_LOGGER();
        return -1;
    }
	this->_typeOfSensor = qFromBigEndian<unsigned char>(dataRead.mid(5, 1).constData());   // 5 and 1 according to the protocol
	this->_numOfAdcCH = qFromBigEndian<unsigned char>(dataRead.mid(6, 1).constData());   // 6 and 1 according to the protocol
	this->_sizeOfAdcCH = qFromBigEndian<unsigned char>(dataRead.mid(7, 1).constData());   // 7 and 1 according to the protocol
	this->_numOfOS = qFromBigEndian<unsigned char>(dataRead.mid(8, 1).constData());   // 8 and 1 according to the protocol
	this->_numOfCH = qFromBigEndian<unsigned char>(dataRead.mid(9, 1).constData());   // 9 and 1 according to the protocol
	this->_sizeOfCH = qFromBigEndian<unsigned char>(dataRead.mid(10, 1).constData()); // 10 and 1 according to the protocol
    ERROR_LOGGER();
    return (0);
}

int    ThreadReader::requestPortStart(QSerialPort &port)
{
    ERROR_LOGGER();
    
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
        ERROR_LOGGER();
        return -1;
    }
    if (dataRead.mid(0, 4).toHex().toUInt(nullptr, 16) != preamble) // checking if the first 4 bytes are 'aa55aa55'
    {
        qDebug() << "'aa55aa55' is not recevied, when try to start.";
		this->stopAndClosePort(port);
        ERROR_LOGGER();
        return -1;
    }
    ERROR_LOGGER();
    return (0);
}
