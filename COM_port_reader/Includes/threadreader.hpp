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
        int					requestPortConfig(QSerialPort &port);
        int					requestPortStart(QSerialPort &port);
    
    private:
		ComPort             *_comPort;
        QString 	        _fileCreationDate;
        QString 	        _fileCreationTime;
        QString 	        _fileNamePrefix;
        ThreadDisplayTimer  *_threadDisplayTimer;
        char                _bytesPA;  // Preamble bytes
        char                _bytesID;  // ID bytes
        char                _bytesCO;  // Counter bytes
        char                _bytesCH;  // Channels bytes
        char                _bytesOCH; // One channel bytes
        char                _numOfCH; // Number of channels following (N)
        char                _sizeOfCH; //Number of bytes in one channel data (M)
    
    public: //tmp
        std::array<QStringList, 2>	_data;
        QByteArray                  _dataRead;
        qint64                      _startTime;
};

#endif
