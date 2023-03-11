#ifndef WINDOWNEXT_HPP
# define WINDOWNEXT_HPP

# include <QDialog>
# include <QWidget>
# include <QPushButton>
# include <QLineEdit>
# include <QLabel>
# include <QString>
# include <QScreen>
# include <QApplication>
# include <QCoreApplication>
# include <QComboBox>
# include <QStringList>
# include <QCloseEvent>
# include <QStandardPaths>
# include <QFile>
# include <QTextStream>
# include <QList>
# include <QCheckBox>
# include <QFile>
# include <QTextStream>
# include <QtCharts>
# include <QtEndian>

# include "threadreader.hpp"
# include "threaddrawer.hpp"
# include "threaddisplaytimer.hpp"
# include "mainwindow.h"

class MainWindow;

class WindowNext : public QDialog
{
	public:
		WindowNext(MainWindow *parent);
		~WindowNext();
        
    public:
        void				setButtonStart(QPushButton *buttonStart);
        void				setButtonStop(QPushButton *buttonStop);
        void				setButtonClose(QPushButton *buttonClose);
        void				setButtonBrowse(QPushButton *buttonBrowse);
        
    private:
		void				setParametersDesign(void);
        void				createDirectory(const QString &path);
        QStringList         *findExpProtocols(const QString &path);
        int					readExpProtocol(void);
        void            	saveDataToFile(const QString &subject);
        void                execChartDialog(void);
        
    private slots:
		void				onThreadDisplayTimerFinished(void);
        
    protected:
        void                closeEvent(QCloseEvent *event) override;
        
    private:
        QPushButton			*_buttonStart;
        QPushButton			*_buttonStop;
        QPushButton			*_buttonClose;
		QPushButton			*_buttonBrowse;

		QLabel				*_showReadingPort1;
		QLabel				*_showReadingPort2;
        QString				_selectedDirectory;
        QString				_fullSavingPath;
        
		QLabel				*_showSelectedDir1;
		QLineEdit			*_showSelectedDir2;
        
		QLabel				*_recordingFolder1;
		QLineEdit			*_recordingFolder2;
		QLineEdit			*_recordingFolder3;
		QLineEdit			*_recordingFolder4;
		QLineEdit			*_recordingFolder5;
        
		QLabel				*_placement1;
		QComboBox			*_placement2;
		QLabel				*_placement3;
		QComboBox			*_placement4;
        
		QLabel				*_protocol1;
		QComboBox			*_protocol2;
		QLabel				*_protocol3;
		QComboBox			*_protocol4;
        
		QLabel				*_timer1;
        QLineEdit			*_lineEdit;
        QLabel				*_finishMsgLabel;

        ComPort				*_selectedComPort;
        
        QCheckBox           *_showChart;
        ThreadDisplayTimer  *_threadDisplayTimer;
        ThreadReader        *_threadReader;
        ThreadDrawer        *_threadDrawer;
		int					_durationTimerValue;

        QString				_expProtocolsPath;
        QList<QStringList>	_expProtocol;
        int					_durationMax;
        
        bool                _closeEventFlag;
        
        char                _bytesPA;  // Preamble bytes
        char                _bytesID;  // ID bytes
        char                _bytesCO;  // Counter bytes
        char                _bytesCH;  // Channels bytes
        char                _bytesOCH; // Channels numbers bytes
        char                _numOfCH;  // Number of channels following (N)
        char                _sizeOfCH; // Number of bytes in one channel data (M)
        qint64              _startTime;
};

#endif
