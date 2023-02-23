#ifndef THREADDISPLAYTIMER_HPP
# define THREADDISPLAYTIMER_HPP

# include <QObject>
# include <QThread>
# include <QLabel>
# include <QDialog>

# include "comport.hpp"
# include "all.hpp"

class ThreadDisplayTimer : public QThread
{
	Q_OBJECT
    
	public:
		ThreadDisplayTimer(int durationTimerValue, QDialog *windowSaveTo);
		~ThreadDisplayTimer();

    public:
		void    run() override;
        
    private:
        int     _durationTimerValue;
        QLabel *_displayTimerLabel;
};

#endif
