#include "threaddisplaytimer.hpp"

ThreadDisplayTimer::ThreadDisplayTimer(int durationTimerValue, QDialog *windowSaveTo, QString &expProtocolsPath, QList<QStringList>	&expProtocol)
			: _durationTimerValue(durationTimerValue)
{
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
    
    this->_displayTimerLabel = new QLabel(text, windowSaveTo);
    this->_imageLabel = new QLabel("", windowSaveTo);
    this->_imageSecondsLabel = new QLabel("", windowSaveTo);
    this->_expProtocolsPath = expProtocolsPath;
    this->_expProtocol = expProtocol;
}

ThreadDisplayTimer::~ThreadDisplayTimer()
{
    delete _displayTimerLabel;
    delete _imageLabel;
    delete _imageSecondsLabel;
}

void    ThreadDisplayTimer::run()
{
    this->_displayTimerLabel->setGeometry(360, 205, 160, 40);
    this->_displayTimerLabel->setAlignment(Qt::AlignCenter);
    this->_displayTimerLabel->setStyleSheet("font-size: 34px; color: #B22222; font-weight: bold;");
    this->_displayTimerLabel->show();

    int         seconds = this->_durationTimerValue;
    int         label;
    int         currecntSecond;
    QString     imgPath;
    
    QList<QStringList>::iterator it = this->_expProtocol.begin();
    
    currecntSecond = (*it)[2].toInt();
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
        if (currecntSecond == 0)
        {
            it++;
            currecntSecond = (*it)[2].toInt();
            imgPath = (*it)[3];
            imgPath = this->_expProtocolsPath.left(this->_expProtocolsPath.length() - 13) + imgPath;
            label = (*it)[1].toInt();
        }
        this->_currentImgLabel = label;
        this->showImage(currecntSecond, imgPath);
        currecntSecond--;
        
        int minutes = seconds / 60;
        int hours = minutes / 60;
        QString text = QString("%1:%2:%3")
                           .arg(hours, 2, 10, QLatin1Char('0'))
                           .arg(minutes % 60, 2, 10, QLatin1Char('0'))
                           .arg(seconds % 60, 2, 10, QLatin1Char('0'));
        this->_displayTimerLabel->setText(text);
        QThread::usleep(1000 * (1000 - (QDateTime::currentDateTime().toMSecsSinceEpoch() - start - 1000 * (this->_durationTimerValue - seconds))));
        seconds--;
    }
}

void    ThreadDisplayTimer::showImage(int currecntSecond, const QString &imgPath)
{
    QPixmap pixmap(imgPath);
    QPixmap scaledPixmap = pixmap.scaled(550, 550, Qt::KeepAspectRatio);
    QString imageSeconds = QString::number(currecntSecond);    
    
    
    this->_imageLabel->setGeometry(620, 20, 550, 550);
    this->_imageLabel->setPixmap(scaledPixmap);
    this->_imageLabel->show();

    this->_imageSecondsLabel->setText(imageSeconds);
    this->_imageSecondsLabel->setGeometry(850, 550, 280, 150);
    this->_imageSecondsLabel->setStyleSheet("font-size: 150px; font-weight: bold;");
    this->_imageSecondsLabel->show();
}

int ThreadDisplayTimer::getCurrentImgLabel(void)
{
    return this->_currentImgLabel;
}
