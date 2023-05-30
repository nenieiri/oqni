#include "threaddisplaytimer.hpp"
#include "debugger.hpp"

ThreadDisplayTimer::ThreadDisplayTimer(int durationTimerValue, QDialog *windowNext, QString &expProtocolsPath, QList<QStringList>	&expProtocol)
			: _durationTimerValue(durationTimerValue)
{
    DEBUGGER();
    
    QString text;
    int     num;
    
    num = durationTimerValue / 3600;    
    text = "0" + QString::number(num) + ":";
    
    num = (durationTimerValue - num * 3600) / 60;
    if (num > 9)
        text += QString::number(num) + ":";
    else
        text += "0" + QString::number(num) + ":";
    
    num = (durationTimerValue - (durationTimerValue / 3600) * 3600) - num * 60;
    if (num > 9)
        text += QString::number(num);
    else
        text += "0" + QString::number(num);
    
    this->_displayTimerLabel = new QLabel(text, windowNext);
    this->_imageLabel = new QLabel("", windowNext);
    this->_imageSecondsLabel = new QLabel("", windowNext);
    this->_expProtocolsPath = expProtocolsPath;
    this->_expProtocol = expProtocol;

    this->_currentImgLabel = (char)(*_expProtocol.begin())[1].toInt();
    this->_tmpLabel = 0;
    
    DEBUGGER();
}

ThreadDisplayTimer::~ThreadDisplayTimer()
{
    DEBUGGER();
    
    delete _displayTimerLabel;
    delete _imageLabel;
    delete _imageSecondsLabel;
    
    DEBUGGER();
}

void    ThreadDisplayTimer::run()
{
    DEBUGGER();
    
    this->_displayTimerLabel->setGeometry(367, 280, 160, 40);
    this->_displayTimerLabel->setAlignment(Qt::AlignCenter);
    this->_displayTimerLabel->setStyleSheet("font-size: 34px; color: #B22222; font-weight: bold;");
    this->_displayTimerLabel->show();

    int         seconds = this->_durationTimerValue;
    int         label;
    int         currentSecond;
    QString     imgPath;
    
    QList<QStringList>::iterator it = this->_expProtocol.begin();
    
    currentSecond = (*it)[2].toInt();
    imgPath = (*it)[3];
    imgPath = this->_expProtocolsPath.left(this->_expProtocolsPath.length() - 13) + imgPath;
    label = (*it)[1].toInt();
    
    qint64 start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    while (!isInterruptionRequested())
    {
        if (seconds == 0)
        {
            this->_imageSecondsLabel->hide();
            this->_imageLabel->hide();
			emit finishedSignal();
            break ;
        }        
        if (currentSecond == 0)
        {
            it++;
            currentSecond = (*it)[2].toInt();            
            imgPath = this->_expProtocolsPath.left(this->_expProtocolsPath.length() - 13) + (*it)[3];
            label = (*it)[1].toInt();
        }
        if (this->_tmpLabel != (unsigned char)label)
            emit currentLabel(label);
        this->_tmpLabel = (unsigned char)label;
        emit currentSecondAndImgPath(currentSecond, imgPath);
        currentSecond--;
        
        int minutes = seconds / 60;
        int hours = minutes / 60;
        QString text = QString("%1:%2:%3")
                           .arg(hours, 2, 10, QLatin1Char('0'))
                           .arg(minutes % 60, 2, 10, QLatin1Char('0'))
                           .arg(seconds % 60, 2, 10, QLatin1Char('0'));
        this->_displayTimerLabel->setText(text);
        emit displayTimerText(text);
        QThread::usleep(1000 * (1000 - (QDateTime::currentDateTime().toMSecsSinceEpoch() - start - 1000 * (this->_durationTimerValue - seconds))));
        seconds--;
    }
    
    DEBUGGER();
}

unsigned char ThreadDisplayTimer::getCurrentImgLabel(void)
{
    DEBUGGER();
    if (this->_currentImgLabel)
        return this->_currentImgLabel;
    qDebug() << "Warning: Label not received. Temporary label was returned.";
    return this->_tmpLabel;
}

 void ThreadDisplayTimer::setCurrentLabel(int receivedLabel)
{
    DEBUGGER();
    this->_currentImgLabel = (unsigned char)receivedLabel;
}
