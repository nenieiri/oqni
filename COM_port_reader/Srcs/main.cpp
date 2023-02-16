#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& port : portList) {        
        qDebug() << port.portName();
    }
        
    return a.exec();
}
