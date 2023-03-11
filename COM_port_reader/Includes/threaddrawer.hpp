#ifndef THREADDRAWER_HPP
# define THREADDRAWER_HPP

# include <QThread>
# include <QObject>
# include <QDialog>
# include <QtCharts>
# include <QtEndian>
# include "threadreader.hpp"

class ThreadDrawer : public QThread
{
	Q_OBJECT
    
	public:
		ThreadDrawer(QWidget *parent = nullptr, ThreadReader *threadReader = nullptr);
        ~ThreadDrawer();

    public:
		void		run() override;
        QDialog		*getChartDialog();

    signals:
        void		chartDialogIsRejected();
        void        chartDialogReadyToStart();
        void        currentPointCoordinate(int ledID, qint64 time, unsigned int value);
        
    private:
    	QDialog         *_chartDialog;
        ThreadReader    *_threadReader;
        
        char    _bytesPA; // Preamble bytes
        char    _bytesID; // ID bytes
        char    _bytesCO; // Counter bytes
        char    _bytesCH; // Channels bytes
        char    _bytesOCH; // Channels numbers bytes
        char    _numOfCH; // Number of channels following (N)
        char    _sizeOfCH; // Number of bytes in one channel data (M)
        qint64  _startTime;
        int     _totalBytes;
};

#endif
