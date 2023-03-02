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

void ThreadReader::reader(const ComPort *comPort, const std::string &pathFileName)
{
    QSerialPort port;
    QByteArray  data;
    std::string line;

    port.setPort(comPort->getPort());
    port.setBaudRate(comPort->getBaudRate());
    port.setDataBits(comPort->getDataBits());
    port.setParity(comPort->getParity());
    port.setStopBits(comPort->getStopBits());
    port.setFlowControl(comPort->getFlowControl());

    port.close();
    if (!port.open(QIODevice::ReadWrite))
    {
        qDebug() << "Faild to open serial port!";
        return ;
    }
    
    QByteArray dataWrite;
    dataWrite.append(static_cast<char>(0)); // First byte
    dataWrite.append(static_cast<char>(4)); // Second byte
    
    port.write(dataWrite); 
    
    QByteArray dataClose;
    dataClose.append(static_cast<unsigned char>(-1)); // First byte
    port.write(dataClose);
    
    data = port.read(7);
    qDebug() << data.toHex();
//    while (!isInterruptionRequested())
    for (int i = 0; i < 20 && !isInterruptionRequested(); ++i)
    {
        if (port.waitForReadyRead(5000))
        {
            data = port.read(20);
            qDebug() << data.toHex();
            if (!data.isEmpty())
            {
                line += data.at(0);
                parserUno(line, pathFileName);
                line = "";
            }
            else if (!data.isEmpty() && data.at(0) != '\n')
                    line += data.at(0);
        }
    }
//    QByteArray dataClose;
//    dataClose.append(static_cast<char>(-1)); // First byte
//    port.write(dataClose); 
    port.close();
}
