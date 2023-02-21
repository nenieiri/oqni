#include "../Includes/threadruner.hpp"

ThreadRuner::ThreadRuner(ComPort *comPort, const std::string &fileName)
			: _comPort(comPort)
            , _fileName(fileName)
{
}

ThreadRuner::~ThreadRuner()
{
}

void		ThreadRuner::run()
{
	reader(_comPort, _fileName);
}

void		ThreadRuner::parserUno(std::string &line, const std::string &pathFileName)
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

void ThreadRuner::reader(const ComPort *comPort, const std::string &pathFileName)
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

    if (!port.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open serial port";
        return ;
    }

    while (!isInterruptionRequested())
    {
        if (port.waitForReadyRead(1000))
        {
            data = port.read(1);
            if (!data.isEmpty() && data.at(0) == '\n')
            {
                line += data.at(0);
                parserUno(line, pathFileName);
                line = "";
            }
            else if (!data.isEmpty() && data.at(0) != '\n')
                    line += data.at(0);
        }
    }
    port.close();
}
