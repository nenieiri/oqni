#ifndef THREADREADER_HPP
# define THREADREADER_HPP

# include <QObject>
# include <QThread>
# include <QMessageBox>
# include <QList>
# include <QString>
# include <QtEndian>
# include <array>
# include "errorlogger.hpp"

# include "comport.hpp"
# include "threaddisplaytimer.hpp"

# define MY_READY_READ_TIME 1000

class ThreadReader : public QThread
{
	Q_OBJECT
    
	public:
        ThreadReader(ComPort *comPort, ThreadDisplayTimer *threadDisplayTimer, QCheckBox *showPic);
		~ThreadReader();

    public:
		void				run() override;
        const QString		&getFileNamePrefix() const;
        const QString		&getFileCreationDate() const;
        const QString		&getFileCreationTime() const;
        const char          getBytesPA() const;
        const char          getBytesID() const;
        const char          getBytesCO() const;
        const char          getBytesCH() const;
        const char          getBytesOCH() const;
        const char          getNumOfCH() const;
        const char          getSizeOfCH() const;
        const char          getNumOfOS() const;
        QByteArray			&getDataRead();
        qint64				getStartTime() const;
        
    signals:
        void				lastRowOfData(QByteArray data);
        void				protocolConfigDataIsReady(void);
        
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
        char                _numOfCH;  // Number of channels following (N)
        char                _sizeOfCH; // Number of bytes in one channel data (M)
        char                _numOfOS;  // Number of connected optical sensors
        char                _numOfAdcCH;  // Number of ADC channels following (N)
        char                _sizeOfAdcCH; // Number of bytes in one ADC channel data (M)
        char                _typeOfSensor; // 02 - OPT, 03 - ADC
        QByteArray          _dataRead;
        qint64              _startTime;
        QCheckBox           *_showPic;
};

#endif
