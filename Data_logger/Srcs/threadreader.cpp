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
    this->_fileNamePrefix += "/" + _fileCreationDate + "_" + _fileCreationTime;
    this->_threadDisplayTimer = threadDisplayTimer;
    this->_bytesPA = 4;  // Preamble bytes
    this->_bytesID = 1;  // ID bytes
    this->_bytesCO = 1;  // Counter bytes
    this->_numOfS_OPT = 2; // Number of OPT sensors
    this->_numOfS_IMU = 3; // Number of IMU sensors
    this->_bytesLBL = 1; // Label bytes (frame type 2 format)
    this->_bytesTC = 4; // Time Counter bytes (frame type 2 format)
    this->_timeCounter.resize(4); // initial 4 bytes of Time Counter
    this->_timeCounter.fill(0); // initial value (0) of Time Counter
    
    DEBUGGER();
}

ThreadReader::~ThreadReader()
{
    DEBUGGER();
}

const QString	ThreadReader::getFileNamePrefix(int id) const
{
    DEBUGGER();
    return (id == 4 ? _fileNamePrefix + "_IMU" : _fileNamePrefix + "_OPT");
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

const char	ThreadReader::getBytesTC() const
{
    DEBUGGER();
    return (this->_bytesTC);
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

QVector<QByteArray>	&ThreadReader::getDataRead()
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
    QByteArray  dataRead;
    int			bytesTillData;
    int			bytesTotal_OPT;
    int			bytesTotal_IMU;
    int			bytesTotal = 0;
    int			restOfBytes = 0;
    int         breakCondition;
    qint64      currentTime = 0;
    int         label = 0;

    port.setPort(_comPort->getPort());
    port.setBaudRate(_comPort->getBaudRate());
    port.setDataBits(_comPort->getDataBits());
    port.setParity(_comPort->getParity());
    port.setStopBits(_comPort->getStopBits());
    port.setFlowControl(_comPort->getFlowControl());

    // first we open and close the port to fix the problem getting configuration from microcontroller
    // this problem should be fixed in microcontroller code
    if (port.open(QIODevice::ReadWrite))
        port.close();

    if (!port.open(QIODevice::ReadWrite))
    {
        qDebug() << "Faild to open serial port!";
        DEBUGGER();
        emit failedToRun(1);
        return ;
    }
    if (requestPortConfigAndStart(port) == false)
    {
        DEBUGGER();
        emit failedToRun(2);
        return ;
    }
    
    bytesTillData = _bytesPA + _bytesID + _bytesCO;
    bytesTotal_OPT = bytesTillData + _numOfCH_OPT * _sizeOfCH_OPT;
    bytesTotal_IMU = bytesTillData + _numOfS_IMU * _numOfCH_IMU * _sizeOfCH_IMU;
    emit protocolConfigDataIsReady();

    // reserve capacity for 2 OPT and 1 IMU sensors by sample rate, +10% - for security reason
    _dataRead.reserve(_durationTimerValue * (_sampleRate_OPT * 2 + _sampleRate_IMU * 1) * 1.1);

    while (!isInterruptionRequested())
    {
        // checking which ID was received
        breakCondition = 5;
        while (--breakCondition && port.waitForReadyRead(MY_READY_READ_TIME))
        {
            if (port.size() >= _bytesPA + _bytesID)
            {
                dataRead = port.read(_bytesPA + _bytesID); // first 5 bytes to check ID
                break ;
            }
        }
        if (!breakCondition)
        {
            qDebug() << "TimeOut while reading data.";
            break ;
        }

        char ID = qFromLittleEndian<char>(dataRead.right(1).constData());

        switch (ID) {
        case 1:
        case 2:
            restOfBytes = bytesTotal_OPT - _bytesPA - _bytesID;
            bytesTotal = bytesTotal_OPT;
            break;
        case 4:
            restOfBytes = bytesTotal_IMU - _bytesPA - _bytesID;
            bytesTotal = bytesTotal_IMU;
            break;
        case 7:
            restOfBytes = _bytesLBL + _bytesTC;
            bytesTotal = _bytesPA + _bytesID + restOfBytes;
            break;
        default :
            qDebug() << "Wrong ID received";
        }

        // reading rest of bytes
        breakCondition = 5;
        while (--breakCondition && port.waitForReadyRead(MY_READY_READ_TIME))
        {
            if (port.size() >= bytesTotal)
            {
                dataRead.append(port.read(restOfBytes));
                switch (ID) {
                case 1:
                case 2:
                case 4:
                    currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
                    dataRead.append(QByteArray::fromRawData(reinterpret_cast<const char*>(&currentTime), sizeof(qint64)));
                    label = this->_threadDisplayTimer->getCurrentImgLabel();
                    dataRead.append(_showPic->isChecked() ? label : 0);
                    emit lastRowOfData(dataRead);
                    dataRead.append(_timeCounter);
                    _dataRead.push_back(dataRead);                    
                    break;
                case 7:
                    label = qFromLittleEndian<int>(dataRead.mid(_bytesPA + _bytesID, _bytesLBL).constData());
                    this->_threadDisplayTimer->setCurrentLabel(label);
                    _timeCounter = dataRead.right(_bytesTC);
                    break;
                }
                break ;
            }
        }
        if (!breakCondition)
        {
            qDebug() << "TimeOut while reading data.";
            break ;
        }
    }
    this->stopAndClosePort(port);
    
    DEBUGGER();
}

void    ThreadReader::stopAndClosePort(QSerialPort &port)
{
    DEBUGGER();
    
    QByteArray  dataWrite;
    
    if (port.isOpen())
    {
        port.clear();
        dataWrite.append(static_cast<char>(-1));
        dataWrite.append(static_cast<char>(0));
        port.write(dataWrite);
        port.flush();
        port.close();
    }

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
    while (dataRead.size() < 16)
    {
        if (port.waitForReadyRead(MY_READY_READ_TIME))
            dataRead += port.read(port.size());
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
    qDebug() << "'AA55AA55' received: SUCCESS"; // tmp

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

    // sending current label to the microcontroller
    connect(_threadDisplayTimer, &ThreadDisplayTimer::currentLabel, this,
        [&](int label)
        {
            QByteArray dataWrite;
            dataWrite.append(static_cast<char>(7)); // First byte
            dataWrite.append(static_cast<char>(label)); // Second byte
            port.write(dataWrite);
        });

    DEBUGGER();
    return true;
}
