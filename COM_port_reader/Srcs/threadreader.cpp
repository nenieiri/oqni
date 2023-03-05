#include "threadreader.hpp"

ThreadReader::ThreadReader(ComPort *comPort, const QString &fullSavingPath, ThreadDisplayTimer *threadDisplayTimer)
			: _comPort(comPort)
{
    QString fileNamePrefix = fullSavingPath + "/" + QDateTime::currentDateTime().toString("yyMMdd_hhmmss") + "_DGT";
    this->_fileNamePrefix = fileNamePrefix.toStdString();
    this->_threadDisplayTimer = threadDisplayTimer;
}

ThreadReader::~ThreadReader()
{
}

void    ThreadReader::run()
{
    reader(_comPort, _fileNamePrefix);
}

void    ThreadReader::reader(const ComPort *comPort, const std::string &pathFileNamePrefix)
{
    QSerialPort port;
    QByteArray  dataRead;
    QString     line;
    std::array<std::ofstream, 2>  myFiles;

    qint64      start;
    int			bytesTillData;
    int			bytesTotal;
    char        bytesPA     = 4; // Preamble bytes
    char        bytesID     = 1; // ID bytes
    char        bytesCO     = 1; // Counter bytes
    char        bytesCH     = 1; // Channels bytes
    char        bytesOCH    = 1; // One channel bytes
    int         info[2];
    
    char            id;
    unsigned char   counter;
    unsigned int    data;
    
    unsigned char   oldCounter[2];

    port.setPort(comPort->getPort());
    port.setBaudRate(comPort->getBaudRate());
    port.setDataBits(comPort->getDataBits());
    port.setParity(comPort->getParity());
    port.setStopBits(comPort->getStopBits());
    port.setFlowControl(comPort->getFlowControl());

    if (!port.open(QIODevice::ReadWrite))
    {
        qDebug() << "Faild to open serial port!";
        return ;
    }
    if (requestPortConfig(port, info) == -1)
        return ;
    if (requestPortStart(port) == -1)
        return ;
    start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    
    myFiles[0].open(this->_fileNamePrefix + "1.csv", std::ios::app);
    if (!myFiles[0].is_open())
        return ;
    myFiles[0] << "time_millisec,led11,led12,led13,label\n";

    myFiles[1].open(this->_fileNamePrefix + "2.csv", std::ios::app);
    if (!myFiles[1].is_open())
        return ;
    myFiles[1] << "time_millisec,led21,led22,led23,label\n";

    
    bytesTillData = bytesPA + bytesID + bytesCO + bytesCH + bytesOCH;
    bytesTotal = bytesTillData + info[0] * info[1];
    
    for (int i = 0; i < 2; ++i)
    {
        line = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() - start) + ",";
        if (port.waitForReadyRead(MY_READY_READ_TIME))
        {
            dataRead = port.read(bytesTotal);
            id = dataRead.mid(bytesPA, bytesID).toHex().toUInt(nullptr, 16);
            oldCounter[id -1] = dataRead.mid(bytesPA + bytesID, bytesCO).toHex().toUInt(nullptr, 16);
            for (int i = 0; i < info[0]; ++i)
            {
                data = dataRead.mid(bytesTillData + i * info[1], info[1]).toHex().toUInt(nullptr, 16);
                line += QString::number(data) + ",";
            }
            line += QString::number(this->_threadDisplayTimer->getCurrentImgLabel()) + "\n";
            myFiles[id - 1] << line.toStdString();
        }
    }
    while (!isInterruptionRequested())
    {
        line = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() - start) + ",";
        if (port.waitForReadyRead(MY_READY_READ_TIME))
        {
            dataRead = port.read(bytesTotal);
            id = dataRead.mid(bytesPA, bytesID).toHex().toUInt(nullptr, 16);
            counter = dataRead.mid(bytesPA + bytesID, bytesCO).toHex().toUInt(nullptr, 16);
            if ((counter - oldCounter[id - 1]) != 1)
            {
                for (int i = 0; i < (counter - oldCounter[id - 1]) - 1; ++i)
                    line += "\n";
            }
            oldCounter[id -1] = counter;
            for (int i = 0; i < info[0]; ++i)
            {
                data = dataRead.mid(bytesTillData + i * info[1], info[1]).toHex().toUInt(nullptr, 16);
                line += QString::number(data) + ",";
            }
            line += QString::number(this->_threadDisplayTimer->getCurrentImgLabel()) + "\n";
            myFiles[id - 1] << line.toStdString();
        }
        else
            break ;
    }
    myFiles[0].close();
    myFiles[1].close();
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

int    ThreadReader::requestPortConfig(QSerialPort &port, int *info)
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
        qDebug() << "'aa55aa55' is not recevied.";
		this->stopAndClosePort(port);
        return -1;
    }
    info[0] = dataRead.mid(9, 1).toHex().toUInt(nullptr, 16);  // Number of channels following (N)
    info[1] = dataRead.mid(10, 1).toHex().toUInt(nullptr, 16); // Number of bytes in one channel data (M)
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
        dataRead = port.read(7);
    else
    {
        this->stopAndClosePort(port);
        return -1;
    }
    if (dataRead.mid(0, 4).toHex().toUInt(nullptr, 16) != preamble) // checking if the first 4 bytes are 'aa55aa55'
    {
        qDebug() << "'aa55aa55' is not recevied.";
		this->stopAndClosePort(port);
        return -1;
    }
    return (0);
}
