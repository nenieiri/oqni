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
    QByteArray  dataWrite;
    std::string line;
    int			byteCount;

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
    
    byteCount = requestPortConfig(port);
    if (byteCount == -1)
        return ;
    if (requestPortStart(port) == -1)
        return ;
    
    while (!isInterruptionRequested())
    {
        for (int i = 0; i < 5 && !isInterruptionRequested(); ++i) //tmp
        {
            if (port.waitForReadyRead(MY_READY_READ_TIME))
            {
                dataRead = port.read(byteCount);
                qDebug() << dataRead.toHex();
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

int    ThreadReader::requestPortConfig(QSerialPort &port)
{
    QByteArray  dataWrite;
    QByteArray  dataRead;
    int         preamble = 0xaa55aa55;
    int         previewsBytes = 0;
    int         length;
    
    dataWrite.append(static_cast<char>(127)); // Requesting configuration
    port.write(dataWrite);
    if (port.waitForReadyRead(MY_READY_READ_TIME))
        dataRead = port.read(12);
    else
    {
		this->stopAndClosePort(port);
        return -1;
    }
    if (dataRead.toHex().mid(0, 4).toInt() != preamble) // checking if the first 4 bytes are 'aa55aa55'
    {
        auto y = dataRead.mid(0, 4);
        bool ok;
        auto x = y.toInt(&ok, 16);
        qDebug() << "x = " << x;
        qDebug() << "type of x is " << typeid(x).name();
        
//        qDebug() << dataRead.mid(0, 4).toHex() << "   O: " << preamble;
        qDebug() << "'aa55aa55' is not recevied.";
		this->stopAndClosePort(port);
        return -1;
    }
    previewsBytes += 4; // preamble bytes
    previewsBytes += 1; // ID bytes
    previewsBytes += 1; // Counter bytes
    previewsBytes += 1; // Number of channels
    previewsBytes += 1; // Number of bytes in one channel
    length = previewsBytes + (dataRead.toHex().mid(18, 2).toInt() * dataRead.toHex().mid(20, 2).toInt());
    
    return length;
}

int    ThreadReader::requestPortStart(QSerialPort &port)
{
    QByteArray  dataWrite;
    QByteArray  dataRead;
    QString     preamble = "aa55aa55";

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
    if (dataRead.startsWith(preamble.toUtf8()) != 0) // checking if the first 4 bytes are 'aa55aa55'
    {
        qDebug() << "'aa55aa55' is not recevied.";
		this->stopAndClosePort(port);
        return -1;
    }
    return (0);
}
