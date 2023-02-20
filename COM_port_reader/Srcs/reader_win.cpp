#include <Windows.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <QDebug>

HANDLE	connectedPort;

static int	max(int x, int y, int z)
{
    return ((x > y and x > z) ? x : ((y > x and y > z) ? y : z));
}

void    parser(std::string &msg, const std::string &pathFileName)
{
    size_t	foundX, foundY, foundZ;
    size_t	spaceX, spaceY, spaceZ;
    int		file_is_empty = 0;

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

    foundX = msg.find("XVALUE=");
    foundY = msg.find("YVALUE=");
    foundZ = msg.find("ZVALUE=");
    while (foundX != std::string::npos or foundY != std::string::npos or foundZ != std::string::npos)
    {
        spaceX = msg.find("\t", foundX + 7);
        spaceY = msg.find("\t", foundX + 7);
        spaceZ = msg.find("\t", foundZ + 7);        
        
        if (foundX != std::string::npos)
            MyFile << std::stoi(msg.substr(foundX + 7, spaceX - foundX - 7)) << ",";
        if (foundY != std::string::npos)
            MyFile << std::stoi(msg.substr(foundY + 7, spaceY - foundY - 7)) << ",";
        if (foundZ != std::string::npos)
            MyFile << std::stoi(msg.substr(foundZ + 7, spaceZ - foundZ - 7)) << "\n";

        if (max(spaceX, spaceY, spaceZ) < 0)
            break ;
        msg = msg.substr(max(spaceX, spaceY, spaceZ));
        foundX = msg.find("XVALUE=");
        foundY = msg.find("YVALUE=");
        foundZ = msg.find("ZVALUE=");
    }
    MyFile.close();
}

int	SerialBegin(const std::string &name, unsigned int BaudRate)
{
    DCB				SerialParams;
    COMMTIMEOUTS	SerialTimeouts;

    CloseHandle(connectedPort);

    connectedPort = CreateFileA(
        name.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (connectedPort == INVALID_HANDLE_VALUE)
        return -4; //No Port

    SerialParams.DCBlength = sizeof(SerialParams);
    if (!GetCommState(connectedPort, &SerialParams))
        return -3; //GetState error

    SerialParams.BaudRate = BaudRate;
    SerialParams.ByteSize = 8;
    SerialParams.StopBits = ONESTOPBIT;
    SerialParams.Parity = NOPARITY;

    SerialTimeouts.ReadIntervalTimeout = 1;
    SerialTimeouts.ReadTotalTimeoutConstant = 1;
    SerialTimeouts.ReadTotalTimeoutMultiplier = 1;
    SerialTimeouts.WriteTotalTimeoutConstant = 1;
    SerialTimeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(connectedPort, &SerialTimeouts))
        return (-1);

    return (0);
}

void	SerialRead(const std::string &name, unsigned int BaudRate, const std::string &pathFileName)
{
    char        Buffer[1024];
    std::string str_fstream;

    if (!SetCommMask(connectedPort, EV_RXCHAR))
        SerialBegin(name, BaudRate);

    DWORD	BytesIterated;
    while (ReadFile(connectedPort, Buffer, 255, &BytesIterated, NULL))
    {
        str_fstream += Buffer;
        parser(str_fstream, pathFileName);
    }
}

int reader_win(const std::string &name, unsigned int BaudRate, const std::string &pathFileName)
{
//    qDebug() << QString::fromStdString(pathFileName);
    SerialBegin(name, BaudRate);
    SerialRead(name, BaudRate, pathFileName);
    
    return 0;
}
