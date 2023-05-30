#ifndef THREADREADER_HPP
# define THREADREADER_HPP

# include <QObject>
# include <QThread>
# include <QMessageBox>
# include <QList>
# include <QString>
# include <QtEndian>
# include <array>

# include "comport.hpp"
# include "threaddisplaytimer.hpp"

# define MY_READY_READ_TIME 1000

class ThreadReader : public QThread
{
	Q_OBJECT
    
	public:
        ThreadReader(int durationTimerValue, ComPort *comPort, ThreadDisplayTimer *threadDisplayTimer, QCheckBox *showPic);
		~ThreadReader();

    public:
		void				run() override;
        const QString		getFileNamePrefix(int id) const;
        const QString		&getFileCreationDate() const;
        const QString		&getFileCreationTime() const;
        const char          getBytesPA() const;
        const char          getBytesID() const;
        const char          getBytesCO() const;
        const char          getBytesTC() const;

        const short         getSampleRate_OPT() const;
        const short         getSampleRate_IMU() const;
        const char          getNumOfCH_OPT() const;
        const char          getNumOfCH_IMU() const;
        const char          getSizeOfCH_OPT() const;
        const char          getSizeOfCH_IMU() const;
        const char          getNumOfS_OPT() const;
        const char          getNumOfS_IMU() const;

        QVector<QByteArray>		&getDataRead();
        qint64                  getStartTime() const;
        
    signals:
        void				lastRowOfData(QByteArray data);
        void				protocolConfigDataIsReady(void);
        void				failedToRun(int errorCode);
        
    private:
        void				stopAndClosePort(QSerialPort &port);
        bool				requestPortConfigAndStart(QSerialPort &port);
    
    private:
        int                 _durationTimerValue;
		ComPort             *_comPort;
        QString 	        _fileCreationDate;
        QString 	        _fileCreationTime;
        QString 	        _fileNamePrefix;
        ThreadDisplayTimer  *_threadDisplayTimer;

        char                _bytesPA;  // Preamble bytes
        char                _bytesID;  // ID bytes
        char                _bytesCO;  // Counter bytes
        char                _bytesLBL; // Label bytes (frame type 2 format)
        char                _bytesTC;  // Time Counter bytes (frame type 2 format)
        char                _numOfS_OPT;  // Number of connected OPT sensors
        char                _numOfS_IMU;  // Number of connected IMU sensors

        short               _sampleRate_OPT; // Sample Rate for OPT sensors
        short               _sampleRate_IMU; // Sample Rate for IMU sensors
        char                _numOfCH_OPT; // Number of OTP channels (N)
        char                _numOfCH_IMU; // Number of IMU channels (N)
        char                _sizeOfCH_OPT; // Number of bytes in one OPT channel data (M)
        char                _sizeOfCH_IMU; // Number of bytes in one IMU channel data (M)

        QByteArray          _timeCounter; // Time Conuter received from MC (frame type 2 format)

        QVector<QByteArray> _dataRead;
        qint64              _startTime;
        QCheckBox           *_showPic;
};

#endif
