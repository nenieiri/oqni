#include <iostream>
#include <fstream>
#include <string>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>

void    parser(std::string &line, const std::string &pathFileName)
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

void reader_win(const QString &portName, QSerialPort::BaudRate BR, QSerialPort::DataBits DB,
                QSerialPort::Parity P, QSerialPort::StopBits SB, QSerialPort::FlowControl FC,
                const std::string &pathFileName)
{
    QSerialPort serial;
    QByteArray  data;
    std::string line;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.portName() == portName)
        {
            serial.setPort(info);
            break;
        }
    }

    serial.setBaudRate(BR);
    serial.setDataBits(DB);
    serial.setParity(P);
    serial.setStopBits(SB);
    serial.setFlowControl(FC);

//    serial.setBaudRate(QSerialPort::Baud9600);
//    serial.setDataBits(QSerialPort::Data8);
//    serial.setParity(QSerialPort::NoParity);
//    serial.setStopBits(QSerialPort::OneStop);
//    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open serial port";
        return ;
    }

    while (true)
    {
        if (serial.waitForReadyRead(1000))
        {
            data = serial.read(1); // Read one byte from the serial port
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
