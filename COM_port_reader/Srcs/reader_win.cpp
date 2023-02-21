#include "all.hpp"

static void    parser(std::string &line, const std::string &pathFileName)
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
        MyFile << "X,Y,Z\n";

    found = line.find(tokenX);
    if (found != std::string::npos)
        MyFile << std::stoi(line.substr(found + tokenX.length())) << ",";
    else
        MyFile << ",";

    found = line.find(tokenY);
    if (found != std::string::npos)
        MyFile << std::stoi(line.substr(found + tokenY.length())) << ",";
    else
        MyFile << ",";

    found = line.find(tokenZ);
    if (found != std::string::npos)
        MyFile << std::stoi(line.substr(found + tokenZ.length())) << "\n";
    else
        MyFile << "\n";

    MyFile.close();
}

void reader_win(const ComPort *comPort, const std::string &pathFileName)
{
    QSerialPort port;
    QByteArray  data;
    std::string line;

    port.setPort(comPort->getPort());
//    port.setBaudRate(comPort->getBaudRate());
//    port.setDataBits(comPort->getDataBits());
//    port.setParity(comPort->getParity());
//    port.setStopBits(comPort->getStopBits());
//    port.setFlowControl(comPort->getFlowControl());

    port.setBaudRate(QSerialPort::Baud9600);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);

    qDebug() << (comPort->getBaudRate() == QSerialPort::Baud9600);
    qDebug() << (comPort->getDataBits() == QSerialPort::Data8);
    qDebug() << (comPort->getParity() == QSerialPort::SpaceParity);
    qDebug() << (comPort->getStopBits() == QSerialPort::OneAndHalfStop);
    qDebug() << (comPort->getFlowControl() == QSerialPort::SoftwareControl);

    if (!port.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open serial port";
        return ;
    }

    while (true)
    {
        if (port.waitForReadyRead(1000))
        {
            data = port.read(1); // Read one byte from the serial port
            if (!data.isEmpty() && data.at(0) == '\n') // Get the character from the QByteArray
            {
                line += data.at(0);
//                qDebug() << QString::fromStdString(line);
                parser(line, pathFileName);
                line = "";
            }
            else if (!data.isEmpty() && data.at(0) != '\n')
                    line += data.at(0);
        }
    }
}
