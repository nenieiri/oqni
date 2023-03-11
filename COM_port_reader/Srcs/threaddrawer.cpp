#include "../Includes/threaddrawer.hpp"

ThreadDrawer::ThreadDrawer(QWidget *parent, ThreadReader *threadReader)
    : QThread{parent}
{
    this->_threadReader = threadReader;
    this->_chartDialog = new QDialog(parent);
    this->_chartDialog->setWindowTitle("OQNI: Chart");
    this->_chartDialog->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->_chartDialog->setWindowFilePath(":/Imgs/oqni.ico");
    this->_chartDialog->setStyleSheet("background: #e6e6e6;");
    
    connect(this->_chartDialog, &QDialog::rejected, this, 
        [=]()
        {
            this->_chartDialog = nullptr;
            emit chartDialogIsRejected();
        });
}

ThreadDrawer::~ThreadDrawer()
{
    delete this->_chartDialog;
}

QDialog	*ThreadDrawer::getChartDialog()
{
    return (this->_chartDialog);
}

void	ThreadDrawer::run()
{
    connect(_threadReader, &ThreadReader::protocolConfigDataIsReady, this,
        [&](void)
        {
            _bytesPA = this->_threadReader->getBytesPA();
            _bytesID = this->_threadReader->getBytesID();
            _bytesCO = this->_threadReader->getBytesCO();
            _bytesCH = this->_threadReader->getBytesCH();
            _bytesOCH = this->_threadReader->getBytesOCH();
            _numOfCH = this->_threadReader->getNumOfCH();
            _sizeOfCH = this->_threadReader->getSizeOfCH();
            _startTime = this->_threadReader->getStartTime();
            _totalBytes = _bytesPA + _bytesID + _bytesCO + _bytesCH + _bytesOCH + _numOfCH * _sizeOfCH + 8 + 1; // 8 - sizeof time; 1 - sizeof label
        });
    
    connect(this->_threadReader, &ThreadReader::lastRowOfData, this,
        [=](QByteArray data)
        {
            char    id = qFromBigEndian<unsigned char>(data.mid(_bytesPA, _bytesID).constData());
            qint64  time = qFromLittleEndian<qint64>(data.mid(_totalBytes - 8 - 1, 8).constData()) - _startTime;
            
            unsigned int value;
            int ledID;            
            for (int j = 0; j < _numOfCH; ++j)
            {
                value = qFromLittleEndian<unsigned int>(data.mid(_bytesPA + _bytesID + _bytesCO + \
                                                                            _bytesCH + _bytesOCH + j * _sizeOfCH, _sizeOfCH).constData());
                if (id == 1)
                    ledID = j;
                else
                    ledID = j + 3;                
                emit currentPointCoordinate(ledID, time, value);
//                qDebug() << "ledID = " << ledID << "    time = " << time << "    value = " << value;
            }
        });

        emit    chartDialogReadyToStart();
}
