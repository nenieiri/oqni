#ifndef THREADDISPLAYTIMER_HPP
# define THREADDISPLAYTIMER_HPP

# include <QObject>
# include <QThread>
# include <QLabel>
# include <QDialog>

# include "all.hpp"

class ThreadDisplayTimer : public QThread
{
	Q_OBJECT
    
	public:
		ThreadDisplayTimer(int durationTimerValue, QDialog *windowSaveTo);
		~ThreadDisplayTimer();

    public:
		void    run() override;
        void    showImage(int *imgFlag, int sec1, int sec2, int sec3, int sec4);
        
    private:
        int     _durationTimerValue;
        QLabel *_displayTimerLabel;
        QLabel *_imageLabel;
        QLabel *_imageSecondsLabel;

    signals:
        void finishedSignal();
};

#endif
