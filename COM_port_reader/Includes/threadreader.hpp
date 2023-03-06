#ifndef THREADREADER_HPP
# define THREADREADER_HPP

# include <QObject>
# include <QThread>
# include <QMessageBox>
# include <QList>
# include <QtCharts>

# include <iostream>
# include <string>
# include <fstream>
# include <cstdlib>

# include "comport.hpp"
# include "threaddisplaytimer.hpp"

# define MY_READY_READ_TIME 1000

class ThreadReader : public QThread
{
	Q_OBJECT
    
	public:
        ThreadReader(ComPort *comPort, const QString &fullSavingPath, ThreadDisplayTimer *threadDisplayTimer);
		~ThreadReader();

    public:
		void    run() override;
        
    private:
        void    stopAndClosePort(QSerialPort &port);
        int     requestPortConfig(QSerialPort &port, int *info);
        int     requestPortStart(QSerialPort &port, qint64 *start);
    
    private:
		ComPort             *_comPort;
        std::string         _fileNamePrefix;
        ThreadDisplayTimer  *_threadDisplayTimer;
    
    public: //tmp
        QList<QPoint>		_pointLed[6];
};

#endif
