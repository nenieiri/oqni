#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    // Get a list of all available serial ports
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    
    // Iterate over the list of available serial ports
    for (const QSerialPortInfo& port : portList) {
        
        qDebug() << port.portName();
        // Check if the port has been added or removed
//        if (port.isValid() && !m_serialPortNames.contains(port.portName())) {
//            // A new port has been added, so update the list of port names
//            m_serialPortNames.append(port.portName());
//            qDebug() << "New serial port detected: " << port.portName();
//        }
    }
        
    return a.exec();
}
