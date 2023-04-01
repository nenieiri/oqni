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
		ThreadDisplayTimer(int durationTimerValue, QDialog *windowNext, QString &expProtocolsPath, QList<QStringList>	&expProtocol);
		~ThreadDisplayTimer();

    public:
		void    run() override;
        int     getCurrentImgLabel(void);
        
    private:
        int                 _durationTimerValue;
        int                 _currentImgLabel;
        QLabel              *_displayTimerLabel;
        QLabel              *_imageLabel;
        QLabel              *_imageSecondsLabel;
        QString             _expProtocolsPath;
        QList<QStringList>	_expProtocol;
        
    signals:
        void    finishedSignal();
        void    displayTimerText(QString text);
        void    currentSecondAndImgPath(int currentSecond, QString imgPath);
};

#endif
