#include "threadreader.hpp"
#include "debugger.hpp"

ThreadReader::ThreadReader(int durationTimerValue, ComPort *comPort, ThreadDisplayTimer *threadDisplayTimer, QCheckBox *showPic)
			: _comPort(comPort) \
            , _showPic(showPic) \
            , _durationTimerValue(durationTimerValue)
{
    DEBUGGER();
    
    this->_fileCreationDate = QDateTime::currentDateTime().toString("yyMMdd");
    this->_fileCreationTime = QDateTime::currentDateTime().toString("hhmmss");
    this->_fileNamePrefix += "/" + _fileCreationDate + "_" + _fileCreationTime + "_OPT";
    this->_threadDisplayTimer = threadDisplayTimer;
    this->_bytesPA = 4;  // Preamble bytes
    this->_bytesID = 1;  // ID bytes
    this->_bytesCO = 1;  // Counter bytes
    this->_numOfS_OPT = 2; // Number of OPT sensors
    this->_numOfS_IMU = 3; // Number of IMU sensors
    
    DEBUGGER();
}

ThreadReader::~ThreadReader()
{
    DEBUGGER();
}

const QString	&ThreadReader::getFileNamePrefix() const
{
    DEBUGGER();
    return (this->_fileNamePrefix);
}

const QString	&ThreadReader::getFileCreationDate() const
{
    DEBUGGER();
    return (this->_fileCreationDate);
}

const QString	&ThreadReader::getFileCreationTime() const
{
    DEBUGGER();
    return (this->_fileCreationTime);
}

const char	ThreadReader::getBytesPA() const
{
    DEBUGGER();
    return (this->_bytesPA);
}

const char	ThreadReader::getBytesID() const
{
    DEBUGGER();
    return (this->_bytesID);
}

const char	ThreadReader::getBytesCO() const
{
    DEBUGGER();
    return (this->_bytesCO);
}

const char	ThreadReader::getNumOfS_OPT() const
{
    DEBUGGER();
    return (this->_numOfS_OPT);
}

const char	ThreadReader::getNumOfS_IMU() const
{
    DEBUGGER();
    return (this->_numOfS_IMU);
}

const short ThreadReader::getSampleRate_OPT() const
{
    DEBUGGER();
    return (this->_sampleRate_OPT);
}

const short ThreadReader::getSampleRate_IMU() const
{
    DEBUGGER();
    return (this->_sampleRate_IMU);
}

const char	ThreadReader::getNumOfCH_OPT() const
{
    DEBUGGER();
    return (this->_numOfCH_OPT);
}

const char	ThreadReader::getNumOfCH_IMU() const
{
    DEBUGGER();
    return (this->_numOfCH_IMU);
}

const char	ThreadReader::getSizeOfCH_OPT() const
{
    DEBUGGER();
    return (this->_sizeOfCH_OPT);
}

const char	ThreadReader::getSizeOfCH_IMU() const
{
    DEBUGGER();
    return (this->_sizeOfCH_IMU);
}

QByteArray	&ThreadReader::getDataRead()
{
    DEBUGGER();
    return (this->_dataRead);
}

qint64	ThreadReader::getStartTime() const
{
    DEBUGGER();
    return (this->_startTime);
}

void    ThreadReader::run()
{
    DEBUGGER();
    
    QSerialPort port;
    QByteArray  prefixData;
    int			bytesTillData;
    int			bytesTotal_OPT;
    int			bytesTotal_IMU;
    int			bytesTotal;
    int			restOfBytes;
    int         breakCondition;
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
        DEBUGGER();
        return ;
    }
    if (requestPortConfigAndStart(port) == false)
    {
        DEBUGGER();
        return ;
    }
    
    bytesTillData = _bytesPA + _bytesID + _bytesCO;
    bytesTotal_OPT = bytesTillData + _numOfCH_OPT * _sizeOfCH_OPT;
    bytesTotal_IMU = bytesTillData + _numOfS_IMU * _numOfCH_IMU * _sizeOfCH_IMU;
    emit protocolConfigDataIsReady();

//    _dataRead.reserve()

    while (!isInterruptionRequested())
    {
        // checking which ID was received
        breakCondition = 5;
        while (--breakCondition && port.waitForReadyRead(MY_READY_READ_TIME))
        {
            if (port.size() >= _bytesPA + _bytesID)
            {
                prefixData = port.read(_bytesPA + _bytesID);
                break ;
            }
        }
        if (!breakCondition)
        {
            qDebug() << "TimeOut while reading data.";
            break ;
        }

        char ID = qFromLittleEndian<char>(prefixData.right(1).constData());

        switch (ID) {
            case 4 :
                restOfBytes = bytesTotal_IMU - _bytesPA - _bytesID;
                bytesTotal = bytesTotal_IMU;
                break;
            default :
                restOfBytes = bytesTotal_OPT - _bytesPA - _bytesID;
                bytesTotal = bytesTotal_OPT;
                break;
        }

        breakCondition = 5;
        while (--breakCondition && port.waitForReadyRead(MY_READY_READ_TIME))
        {
            if (port.size() >= bytesTotal)
            {
                _dataRead += prefixData + port.read(restOfBytes);
                break ;
            }
        }
        if (!breakCondition)
        {
            qDebug() << "TimeOut while reading data.";
            break ;
        }

        currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        _dataRead.append(QByteArray::fromRawData(reinterpret_cast<const char*>(&currentTime), sizeof(qint64)));
        _dataRead.append(_showPic->isChecked() ? this->_threadDisplayTimer->getCurrentImgLabel() : 0); // when _showPic checkbox is not checked, label = 0
        emit lastRowOfData(_dataRead.right(bytesTotal + 8 + 1)); // 8 - sizeof time; 1 - sizeof label
    }
    this->stopAndClosePort(port);
    
    DEBUGGER();
}

void    ThreadReader::stopAndClosePort(QSerialPort &port)
{
    DEBUGGER();
    
    QByteArray  dataWrite;
    
    dataWrite.append(static_cast<char>(-1));
    port.write(dataWrite);
    port.flush();
    port.close();
    
    DEBUGGER();
}

bool    ThreadReader::requestPortConfigAndStart(QSerialPort &port)
{
    DEBUGGER();
    
    QByteArray      dataWrite;
    QByteArray      dataRead;    
    unsigned int    preamble = 0xaa55aa55;    

    // cleaning buffer before requesting configs
    port.clear();
    
    // requesting configuration
    dataWrite.append(static_cast<char>(127));
    dataWrite.append(static_cast<char>(7));
    port.write(dataWrite);

    // reading config for OPT and IMU sensors
    while (dataRead.size() != 16)
    {
        if (port.waitForReadyRead(MY_READY_READ_TIME))
            dataRead += port.read(8);
        else
        {
            qDebug() << "TimeOut while waiting for config.";
            this->stopAndClosePort(port);
            return false;
        }
    }

    // checking if the first 4 bytes are 'aa55aa55'
    if (dataRead.mid(0, 4).toHex().toUInt(nullptr, 16) != preamble)
    {
        qDebug() << "'AA55AA55' not received when requesting configuration.";
        this->stopAndClosePort(port);
        return false;
    }
    qDebug() << "'AA55AA55' received."; // tmp

    // parsing data from frame of type3 fromat by the protocol (two frames: OPT and IMU)
    this->_sampleRate_OPT = qFromLittleEndian<short>(dataRead.mid(4, 2).constData());
    this->_numOfCH_OPT = qFromLittleEndian<char>(dataRead.mid(6, 1).constData());
    this->_sizeOfCH_OPT = qFromLittleEndian<char>(dataRead.mid(7, 1).constData());

    this->_sampleRate_IMU = qFromLittleEndian<short>(dataRead.mid(12, 2).constData());
    this->_numOfCH_IMU = qFromLittleEndian<char>(dataRead.mid(14, 1).constData());
    this->_sizeOfCH_IMU = qFromLittleEndian<char>(dataRead.mid(15, 1).constData());

    // cleaning buffer before starting
    port.clear();

    // starting acquisition
    dataWrite.clear();
    dataWrite.append(static_cast<char>(0)); // First byte
    dataWrite.append(static_cast<char>(7)); // Second byte
    port.write(dataWrite);

    if (port.waitForReadyRead(MY_READY_READ_TIME))
    {
        this->_threadDisplayTimer->start();
        this->_startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    else
    {
        qDebug() << "TimeOut while waiting for start.";
        this->stopAndClosePort(port);
        DEBUGGER();
        return false;
    }

    DEBUGGER();
    return true;
}
