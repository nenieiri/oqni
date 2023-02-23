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
}

ThreadDisplayTimer::~ThreadDisplayTimer()
{
    delete _displayTimerLabel;
}

void		ThreadDisplayTimer::run()
{
    this->_displayTimerLabel->setGeometry(300, 70, 100, 30);
    this->_displayTimerLabel->setAlignment(Qt::AlignCenter);
    this->_displayTimerLabel->show();

    int seconds = this->_durationTimerValue;
    
    while (!isInterruptionRequested())
    {
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
            break ;
    }
}
