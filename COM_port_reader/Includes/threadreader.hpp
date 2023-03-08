#ifndef THREADREADER_HPP
# define THREADREADER_HPP

# include <QObject>
# include <QThread>
# include <QMessageBox>
# include <QList>
# include <QString>
# include <array>

# include "comport.hpp"
# include "threaddisplaytimer.hpp"

# define MY_READY_READ_TIME 1000

class ThreadReader : public QThread
{
	Q_OBJECT
    
	public:
        ThreadReader(ComPort *comPort, ThreadDisplayTimer *threadDisplayTimer);
		~ThreadReader();

    public:
		void				run() override;
        const QString		&getFileNamePrefix() const;
        const QString		&getFileCreationDate() const;
        const QString		&getFileCreationTime() const;
        
    signals:
        void				stringAdded();
        
    private:
        void				stopAndClosePort(QSerialPort &port);
        int					requestPortConfig(QSerialPort &port, int *info);
        int					requestPortStart(QSerialPort &port, qint64 *start);
    
    private:
		ComPort             *_comPort;
        QString 	        _fileCreationDate;
        QString 	        _fileCreationTime;
        QString 	        _fileNamePrefix;
        ThreadDisplayTimer  *_threadDisplayTimer;
    
    public: //tmp
        std::array<QStringList, 2>	_data;
};

#endif
