#include "threaddisplaytimer.hpp"

ThreadDisplayTimer::ThreadDisplayTimer(int durationTimerValue, QDialog *windowSaveTo)
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
}

ThreadDisplayTimer::~ThreadDisplayTimer()
{
    delete _displayTimerLabel;
    delete _imageLabel;
    delete _imageSecondsLabel;
}

void    ThreadDisplayTimer::run()
{
    this->_displayTimerLabel->setGeometry(285, 65, 160, 40);
    this->_displayTimerLabel->setAlignment(Qt::AlignCenter);
    this->_displayTimerLabel->setStyleSheet("font-size: 34px; color: #B22222; font-weight: bold;");
    this->_displayTimerLabel->show();

    int seconds = this->_durationTimerValue;
    int imgFlag = 0;
    
    while (!isInterruptionRequested())
    {
        this->showImage(&imgFlag, 4, 3, 4, 3);
        imgFlag++;

        QThread::usleep(1000000);
        seconds--;
        int minutes = seconds / 60;
        int hours = minutes / 60;
        QString text = QString("%1:%2:%3")
                           .arg(hours, 2, 10, QLatin1Char('0'))
                           .arg(minutes % 60, 2, 10, QLatin1Char('0'))
                           .arg(seconds % 60, 2, 10, QLatin1Char('0'));
        this->_displayTimerLabel->setText(text);

        if (seconds == 0)
        {
            this->_imageSecondsLabel->hide();
            this->_imageLabel->hide();
            break ;
        }
    }
//    emit finished();
}

void    ThreadDisplayTimer::showImage(int *imgFlag, int sec1, int sec2, int sec3, int sec4)
{
    QString path;
    QString imageSeconds;

    if ((*imgFlag) > sec1 + sec2 + sec3 + sec4 - 1)
        (*imgFlag) = 0;
    if ((*imgFlag) < sec1)
    {
        path = ":/Imgs/1)relax_4sec.png";
        imageSeconds = QString::number(sec1 - (*imgFlag) % sec1);
    }
    else if ((*imgFlag) < sec1 + sec2)
    {
        path = ":/Imgs/2)tiptoe_3sec.png";
        imageSeconds = QString::number(sec1 + sec2 - (*imgFlag) % (sec1 + sec2));
    }
    else if ((*imgFlag) < sec1 + sec2 + sec3)
    {
        path = ":/Imgs/3)relax_4sec.png";
        imageSeconds = QString::number(sec1 + sec2 + sec3 - (*imgFlag) % (sec1 + sec2 + sec3));
    }
    else
    {
        path = ":/Imgs/4)heels_3sec.png";
        imageSeconds = QString::number(sec1 + sec2 + sec3 + sec4 - (*imgFlag) % (sec1 + sec2 + sec3 + sec4));
    }

    QPixmap pixmap(path);
    QPixmap scaledPixmap = pixmap.scaled(400, 400, Qt::KeepAspectRatio);
    this->_imageLabel->setGeometry(40, 160, 400, 400);
    this->_imageLabel->setPixmap(scaledPixmap);
    this->_imageLabel->show();

    this->_imageSecondsLabel->setText(imageSeconds);
    this->_imageSecondsLabel->setGeometry(220, 540, 150, 150);
    this->_imageSecondsLabel->setStyleSheet("font-size: 150px; font-weight: bold;");
    this->_imageSecondsLabel->show();
}
