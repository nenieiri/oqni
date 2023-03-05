#include "threadreader.hpp"

ThreadReader::ThreadReader(ComPort *comPort, const QString &selectedDirectory, ThreadDisplayTimer *threadDisplayTimer)
			: _comPort(comPort)
{
    QDateTime   currentDateTime = QDateTime::currentDateTime();
    QString     fileName = selectedDirectory + "/" + \
                            comPort->getPortName() + "_" + \
                            currentDateTime.toString("yyyy-MM-dd_hh-mm-ss") + ".csv";
    
    this->_fileName = fileName.toStdString();
    this->_threadDisplayTimer = threadDisplayTimer;
}

ThreadReader::~ThreadReader()
{
}

void    ThreadReader::run()
{
	reader(_comPort, _fileName);
}

void    ThreadReader::parserUno(std::string &line, const std::string &pathFileName)
{
    size_t      found;
    std::string tokenX = "XVALUE=";
    std::string tokenY = "ZVALUE=";
    std::string tokenZ = "YVALUE=";
    int         file_is_empty = 0;

    std::ofstream createFile(pathFileName, std::ios::app);
    createFile.close();

    std::ifstream infile(pathFileName, std::ios::binary | std::ios::ate);
    if (infile.tellg() == 0)
        file_is_empty = 1;
    infile.close();

    std::ofstream MyFile(pathFileName, std::ios::app);
    if (!MyFile.is_open())
        return ;
    if (file_is_empty)
        MyFile << "X,Y,Z,LABEL\n";

    found = line.find(tokenX);
    if (found != std::string::npos)
        MyFile << abs(std::stoi(line.substr(found + tokenX.length())) % 100) << ",";
    else
        MyFile << ",";

    found = line.find(tokenY);
    if (found != std::string::npos)
        MyFile << abs(std::stoi(line.substr(found + tokenY.length())) % 100) << ",";
    else
        MyFile << ",";

    found = line.find(tokenZ);
    if (found != std::string::npos)
        MyFile << abs(std::stoi(line.substr(found + tokenZ.length())) % 100) << "," << this->_threadDisplayTimer->getCurrentImgLabel() << "\n";
    else
        MyFile << "," << this->_threadDisplayTimer->getCurrentImgLabel() << "\n";

    MyFile.close();
}

void    ThreadReader::reader(const ComPort *comPort, const std::string &pathFileName)
{
    QSerialPort port;
    QByteArray  dataRead;
    std::string line;
    int			bytesTillData;
    int			bytesTotal;
    char        bytesPA     = 4; // Preamble bytes
    char        bytesID     = 1; // ID bytes
    char        bytesCO     = 1; // Counter bytes
    char        bytesCH     = 1; // Channels bytes
    char        bytesOCH    = 1; // One channel bytes
    int         info[2];
    
    char            id;
    char            counter;
    unsigned int    data;

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
    bytesTillData = bytesPA + bytesID + bytesCO + bytesCH + bytesOCH;
    bytesTotal = bytesTillData + info[0] * info[1];
    while (!isInterruptionRequested())
    {
        for (int i = 0; i < 15 && !isInterruptionRequested(); ++i) //tmp
        {
            if (port.waitForReadyRead(MY_READY_READ_TIME))
            {
                dataRead = port.read(bytesTotal);
                qDebug() << dataRead.toHex();
                
                id = dataRead.mid(bytesPA, bytesID).toHex().toUInt(nullptr, 16);
                counter = dataRead.mid(bytesPA + bytesID, bytesCO).toHex().toUInt(nullptr, 16);
                for (int i = 0; i < info[0]; ++i)
                {
                    data = dataRead.mid(bytesTillData + i * info[1], info[1]).toHex().toUInt(nullptr, 16);
                    qDebug() << static_cast<int>(id) << "  " << static_cast<int>(counter) << "  " << data;
                }
                
                if (!dataRead.isEmpty())
                {
                    line += dataRead.at(0);
                    parserUno(line, pathFileName);
                    line = "";
                }
            }
            else
                break ;
        }
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
