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

# include <set>
# include <algorithm>
# include <xlsxdocument.h>

# include "threadreader.hpp"
# include "threaddisplaytimer.hpp"
# include "mainwindow.h"

# define MY_DEFINED_SLIDER_HORIZONTAL_STYLE "QSlider::handle:horizontal {\
                                            background: white; \
                                            border: 1px solid black; \
                                            width: 10px; \
                                            height: 10px; \
                                            margin: -8px 0; \
                                            border-radius: 5px; } \
                                            QSlider::handle:horizontal:hover { \
                                            background-color: yellow; \
                                            border: 1px solid black; }"

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
        void                warningMessageBox(const QString &msg);
        
    private:
		void				setParametersDesign(void);
        void				createDirectory(const QString &path);
        QStringList         *findExpProtocols(const QString &path);
        int					readExpProtocol(void);
        QString            	saveDataToFile(const QString &subject);
        bool            	saveMetaData(const QString &excelSheet, const QString &subject);
        void                retryToSaveMetaData(QXlsx::Document &xlsx, const QString &excelSheet);
        void                execChartDialog(void);
        void                fillSeriesAndUpdateAxes_OPT(QByteArray &data, char &id, qint64 &time);
        void                fillSeriesAndUpdateAxes_IMU(QByteArray &data, char &id, qint64 &time); // implementation is in process
        void                execPicDialog(void);
        void                showImage(int currentSecond, QString imgPath);
        QString            	findMaxSubjectInMetadata(void);
        QString             findSubjectInMetadata(QString subject, int *subjectRow);
        QString             getCellFromMetadata(QString sheet, int row, int col);
        void                setCellInMetadata(QString sheet, int row, int col, const QString &text);
        void                getSeriesMinMaxY_NoAutoscale_OPT(unsigned int &minY, unsigned int &maxY);
        void                getSeriesMinMaxY_autoscale_OPT(unsigned int &minY, unsigned int &maxY);
        void                getSeriesMinMaxY_NoAutoscale_IMU(short &minY, short &maxY, int index);
        
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
        
        QLabel				*_durationSec1;
        QLineEdit			*_durationSec2;
        QLabel				*_finishMsgLabel;

        QLabel				*_lightIntensity1;
        QLineEdit			*_lightIntensity2;

        QLabel				*_distance1;
        QLineEdit			*_distance2;

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
        char                _bytesTC;  // Time Counter bytes (frame type 2 format)
        char                _numOfS_OPT;  // Number of connected OPT sensors
        char                _numOfS_IMU;  // Number of connected IMU sensors

        short               _sampleRate_OPT; // Sample Rate for OPT sensors
        short               _sampleRate_IMU; // Sample Rate for IMU sensors
        char                _numOfCH_OPT;  // Number of OPT channels (N)
        char                _numOfCH_IMU;  // Number of IMU channels (N)
        char                _sizeOfCH_OPT; // Number of bytes in one OPT channel data (M)
        char                _sizeOfCH_IMU; // Number of bytes in one OPT channel data (M)

        qint64              _startTime;

        QDialog				*_picDialog;
        QDialog				*_chartDialog;
        QChart				*_chart_OPT;
        QChart				*_chart_IMU;
        QChartView			*_chartView_OPT;
        QChartView			*_chartView_IMU;
        QValueAxis			*_axisX_OPT;
        QValueAxis			*_axisX_IMU;
        QValueAxis			*_axisY_OPT;
        QValueAxis			*_axisY_IMU;
        QLineSeries			*_series_OPT;
        QLineSeries			*_series_IMU;
        QVector<unsigned>                   _seriesMinY_NoAutoscale_OPT;
        QVector<unsigned>                   _seriesMaxY_NoAutoscale_OPT;
        QVector<std::multiset<unsigned>>    _seriesMinMaxY_autoscale_OPT;
        QVector<QVector<short>>             _seriesMinY_NoAutoscale_IMU;
        QVector<QVector<short>>             _seriesMaxY_NoAutoscale_IMU;
        QGridLayout         *_gridLayoutPic;
        QGridLayout			*_gridLayout;
        QHBoxLayout			*_hBoxLayoutLegends;
        QHBoxLayout			*_hBoxLayoutOptions;
        QCheckBox			*_checkBoxChannels;
        bool                *_checkBoxChannelsValue;
        QCheckBox           *_autoScale;
        QCheckBox           *_checkBoxSensors;
        QSlider             *_sliderHorizontal;
        QLabel              *_sliderHorizontalValues;
        int                 _sliderHorizontalLastValue;

        unsigned int        _chartDuration;
        unsigned int        _chartUpdateRatio_OPT;
        unsigned int        _chartUpdateRatio_IMU;
        qint64				_chartTimeFlag_OPT;
        QVector<qint64>		_chartTimeFlag_IMU;
        
        QLabel              *_displayTimerPic;
        QLabel              *_imageLabel;
        QLabel              *_imageSecondsLabel;
        
        QString             _metaDataFilePath;
        QString             _metaDataSavingFailMsg;
};

#endif
