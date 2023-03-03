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
		ThreadDisplayTimer(int durationTimerValue, QDialog *windowSaveTo, QString &expProtocolsPath, QList<QStringList>	&expProtocol);
		~ThreadDisplayTimer();

    public:
		void    run() override;
        void    showImage(int currecntSecond, const QString &imgPath);
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
        void finishedSignal();
};

#endif
