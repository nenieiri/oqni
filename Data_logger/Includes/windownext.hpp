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
# include <QtCharts>
# include <QtEndian>
# include <QSlider>

# include <xlsxdocument.h>

# include "threadreader.hpp"
# include "threaddisplaytimer.hpp"
# include "mainwindow.h"

class MainWindow;
class MySlider;

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
        void                infoMessageBox(const QString &msg);
        
    private:
		void				setParametersDesign(void);
        void				createDirectory(const QString &path);
        QStringList         *findExpProtocols(const QString &path);
        int					readExpProtocol(void);
        QString            	saveDataToFile(const QString &subject);
        bool            	saveMetaData(const QString &excelSheet, const QString &subject);
        void                execChartDialog(void);
        void                execPicDialog(void);
        void                showImage(int currentSecond, QString imgPath);
        QString             findSubjectInMetadata(QString subject, int *subjectRow);
        
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
        
        QLabel				*_display;
        QCheckBox           *_showChart;
        QCheckBox           *_showPic;
		QCheckBox			*_saveCheckBox;

        ThreadDisplayTimer  *_threadDisplayTimer;
        ThreadReader        *_threadReader;
		int					_durationTimerValue;

        QString				_expProtocolsPath;
        QList<QStringList>	_expProtocol;
        int					_durationMax;

        bool                _closeEventFlag;
        bool                _labelIsOk;

        char                _bytesPA;  // Preamble bytes
        char                _bytesID;  // ID bytes
        char                _bytesCO;  // Counter bytes
        char                _bytesCH;  // Channels bytes
        char                _bytesOCH; // Channels numbers bytes
        char                _numOfCH;  // Number of channels following (N)
        char                _sizeOfCH; // Number of bytes in one channel data (M)
        char                _numOfOS;  // Number of connected optical sensors
        qint64              _startTime;
        int					_totalBytes;

        QDialog				*_picDialog;
        QDialog				*_chartDialog;
        QChart				*_chart;
        QChartView			*_chartView;
        QValueAxis			*_axisX;
        QValueAxis			*_axisY;
        QLineSeries			*_series;
        QGridLayout         *_gridLayoutPic;
        QGridLayout			*_gridLayout;
        QHBoxLayout			*_hBoxLayout;
        QCheckBox			*_checkBoxChannels;
        bool                *_checkBoxChannelsValue;
        QSlider             *_sliderHorizontal;
        QLabel              *_sliderHorizontalValues;

        unsigned int        _chartDuration;
        unsigned int        _chartUpdateRatio;
        qint64				_chartTimeFlag;
        
        QLabel              *_displayTimerPic;
        QLabel              *_imageLabel;
        QLabel              *_imageSecondsLabel;
        
        QString             _metaDataFilePath;
};

#endif
