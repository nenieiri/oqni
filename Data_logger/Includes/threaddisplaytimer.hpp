#ifndef THREADDISPLAYTIMER_HPP
# define THREADDISPLAYTIMER_HPP

# include <QObject>
# include <QThread>
# include <QLabel>
# include <QDialog>
# include <QApplication>
# include <QScreen>
# include <QDateTime>
# include <QList>

class ThreadDisplayTimer : public QThread
{
	Q_OBJECT
    
	public:
        ThreadDisplayTimer(int durationTimerValue, QDialog *windowNext, \
                           QString &expProtocolsPath, QList<QStringList> &expProtocol);
		~ThreadDisplayTimer();

    public:
        void                run() override;
        unsigned char       getCurrentImgLabel(void);
        void                setCurrentLabel(int receivedLabel);
        
    private:
        int                 _durationTimerValue;
        unsigned char       _currentImgLabel;
        unsigned char       _tmpLabel;
        QLabel              *_displayTimerLabel;
        QLabel              *_imageLabel;
        QLabel              *_imageSecondsLabel;
        QString             _expProtocolsPath;
        QList<QStringList>	_expProtocol;
        
    signals:
        void    finishedSignal();
        void    displayTimerText(QString text);
        void    currentSecondAndImgPath(int currentSecond, QString imgPath);
        void    currentLabel(int label);
};

#endif
