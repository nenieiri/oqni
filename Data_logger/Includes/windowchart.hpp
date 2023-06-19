#ifndef WINDOWCHART_HPP
# define WINDOWCHART_HPP

# include <QDialog>
# include <QString>
# include <QtCharts>
# include <QFile>
# include <QTextStream>
# include <QSlider>
# include <QChartView>
# include <QPointF>

# include <algorithm>

class	MainWindow;
class	MyChartView;

class	WindowChart : public QDialog
{
	public:
        WindowChart(MainWindow *parent, const QString &pathToFiles, \
                    QCheckBox *filesList, int filesCount, const QString pathToSnapshots);
		~WindowChart();
    
    private:
        void			execChartDialog(void);
        void    		readFromFile(void);
		void			updateValueLineAxis(void);
        QString         staticChartTitle(void);
        void            connectStaticChatCheckBox(int i);
        QString         getSnapshotFileName(void);
        void            shnapshotMessage(QString &filename);

    private:
        const QString   _pathToFiles;
        const QString   _pathToSnapshots;
        QCheckBox       *_filesList;
        int             _filesCount;
        int				_checkedFilesCount_OPT;
        int				_checkedFilesCount_IMU;
        
        QChart          *_chart_OPT;
        QChart          *_chart_IMU;
        MyChartView   	*_chartView_OPT;
        MyChartView   	*_chartView_IMU[3];
        QValueAxis		*_axisX_OPT;
        QValueAxis		*_axisX_IMU;
        QValueAxis		*_axisY_OPT;
        QValueAxis		*_axisY_IMU;
        QValueAxis		*_axisYLabel_OPT;
        QValueAxis		*_axisYLabel_IMU;
        QLineSeries		*_series_OPT_original; // to keep data without norming
        QLineSeries		*_series_IMU_original; // to keep data without norming
        QLineSeries		*_series_OPT;
        QLineSeries		*_series_IMU;
        QGridLayout		*_gridLayout;
        QHBoxLayout		*_hBoxLayoutLegends;
        QHBoxLayout		*_hBoxLayoutOptions;
        QCheckBox		*_checkBoxChannels_OPT;
        QPushButton		*_normingButton;
        QPushButton		*_snapshotButton;
        QPushButton		*_zoomToHomeButton;
        QIcon			*_iconON;
        QIcon			*_iconOFF;
        QIcon			*_iconHome;
        QIcon			*_iconCamera;
        QScrollBar		*_horizontalScrollBar_OPT;
        QScrollBar		*_horizontalScrollBar_IMU[3];
        QScrollBar		*_verticalScrollBar_OPT;
        QScrollBar		*_verticalScrollBar_IMU[3];
        QCheckBox       *_checkBoxSensors;
        QLabel          *_chartTitle;
        QLabel          *_sensorNames_IMU;

        char            _numOfSeries_OPT;
        char            _numOfSeries_IMU;
        char            _numOfChart_OPT;
        char            _numOfChart_IMU;
        qreal           _timeLineMin;
        qreal           _timeLineMax_OPT;
        qreal           _timeLineMax_IMU;
        qreal           _valueLineMin_OPT;
        qreal           _valueLineMin_IMU[3];
        qreal           _valueLineMax_OPT;
        qreal           _valueLineMax_IMU[3];
        int             _maxLabel_OPT;
        int             _maxLabel_IMU;
        bool            _normingIsOn;
        QVector<qreal>  _seriesMaxAbsY_OPT;
        QVector<qreal>  _seriesMaxAbsY_IMU;
        qreal           _HSBsensitivity; // horizontal scroll bar sensitivity (10x)
        qreal           _VSBsensitivity; // vertical scroll bar sensitivity (100x)
};

class	MyChartView : public QChartView
{
    Q_OBJECT

	public:
		MyChartView(QChart *parent, \
                    qreal   timeLineMin, \
                    qreal   timeLineMax, \
                    qreal   valueLineMin, \
                    qreal   valueLineMax, \
                    QValueAxis *axisX, \
                    QValueAxis *axisY, \
                    QValueAxis *axisYLabel,
                    int maxLabel, \
                    QPushButton *zoomToHomeButton, \
                    QScrollBar *horizontalScrollBar, \
                    QScrollBar *verticalScrollBar, \
                    qreal HSBsensitivity, \
                    qreal VSBsensitivity)
            : QChartView(parent) \
            , _timeLineMin(timeLineMin) \
            , _timeLineMax(timeLineMax) \
            , _valueLineMin(valueLineMin) \
            , _valueLineMax(valueLineMax) \
            , _axisX(axisX) \
            , _axisY(axisY) \
            , _axisYLabel(axisYLabel) \
            , _maxLabel(maxLabel) \
            , _zoomed(false) \
            , _firstTimeZooming(true) \
            , _zoomToHomeButton(zoomToHomeButton) \
            , _horizontalScrollBar(horizontalScrollBar) \
            , _verticalScrollBar(verticalScrollBar) \
            , _HSBsensitivity(HSBsensitivity) \
            , _VSBsensitivity(VSBsensitivity)
        {}

	protected:
        void mousePressEvent(QMouseEvent *event) override
        {
            if (_firstTimeZooming == true)
            {
                _mPx = this->chart()->mapToValue(event->pos()).x();
                _mPy = this->chart()->mapToValue(event->pos()).y();
                _mPx = std::max((qreal)_axisX->min(), _mPx);
                _mPx = std::min((qreal)_axisX->max(), _mPx);
                _mPy = std::max((qreal)_axisY->min(), _mPy);
                _mPy = std::min((qreal)_axisY->max(), _mPy);
            }
            QChartView::mousePressEvent(event);
        }

        void mouseReleaseEvent(QMouseEvent *event) override
        {
            if (_firstTimeZooming == true)
            {
                _mRx = this->chart()->mapToValue(event->pos()).x();
                _mRy = this->chart()->mapToValue(event->pos()).y();
                _mRx = std::max((qreal)_axisX->min(), _mRx);
                _mRx = std::min((qreal)_axisX->max(), _mRx);
                _mRy = std::max((qreal)_axisY->min(), _mRy);
                _mRy = std::min((qreal)_axisY->max(), _mRy);
            }
            if (event->button() == Qt::RightButton)
            {
                _axisX->setRange(_timeLineMin, _timeLineMax);
                _axisY->setRange(_valueLineMin, _valueLineMax);
                _axisYLabel->setRange(0, _maxLabel + 1);
                _zoomed = false;
                emit this->rightClickAction();
				_currentAxisXLength = _timeLineMax - _timeLineMin;
                _horizontalScrollBar->setRange(_timeLineMin * _HSBsensitivity, _timeLineMin * _HSBsensitivity);
                _horizontalScrollBar->setValue(_timeLineMin * _HSBsensitivity);
				_currentAxisYLength = _valueLineMax - _valueLineMin;
				_verticalScrollBar->setRange(_valueLineMin * _VSBsensitivity, _valueLineMin * _VSBsensitivity);
				_verticalScrollBar->setValue(_valueLineMin * _VSBsensitivity);
                return;
            }
            QChartView::mouseReleaseEvent(event);
            _zoomed = true;
            _zoomToHomeButton->setEnabled(true);
            _currentAxisXLength = _axisX->max() - _axisX->min();
            _horizontalScrollBar->setRange(_timeLineMin * _HSBsensitivity, (_timeLineMax - _currentAxisXLength) * _HSBsensitivity);
            _horizontalScrollBar->setValue(_axisX->min() * _HSBsensitivity);
            _currentAxisYLength = _axisY->max() - _axisY->min();
            _verticalScrollBar->setRange(_valueLineMin * _VSBsensitivity, (_valueLineMax - _currentAxisYLength) * _VSBsensitivity);
            if (_firstTimeZooming == true)
            {
                _axisX->setRange(std::min(_mPx, _mRx), std::max(_mPx, _mRx));
                _axisY->setRange(std::min(_mPy, _mRy), std::max(_mPy, _mRy));
                _firstTimeZooming = false;
            }
            _verticalScrollBar->setValue(_axisY->min() * _VSBsensitivity);
            _axisYLabel->setRange(0, _maxLabel + 1);
            emit this->leftClickReleaseAction(_axisX->min(), _axisX->max());
        }

    private:
        QValueAxis		*_axisX;
        QValueAxis		*_axisY;
        QValueAxis		*_axisYLabel;        
        qreal           _timeLineMin;
        qreal           _timeLineMax;
        qreal           _valueLineMin;
        qreal           _valueLineMax;
        int				_maxLabel;
        QPushButton		*_zoomToHomeButton;
        QScrollBar		*_horizontalScrollBar;
        QScrollBar		*_verticalScrollBar;
        qreal           _HSBsensitivity; // horizontal scroll bar sensitivity (10x)
        qreal           _VSBsensitivity; // vertical scroll bar sensitivity (100x)
        qreal           _mPx;   // mouse press X
        qreal           _mPy;   // mouse press Y
        qreal           _mRx;   // mouse release X
        qreal           _mRy;   // mouse release Y
        
    public:
        bool			_zoomed;
        bool            _firstTimeZooming;
        qreal           _currentAxisXLength;
        qreal           _currentAxisYLength;

    signals:
        void            rightClickAction(void);
        void            leftClickReleaseAction(qreal axisXmin, qreal axisXmax);
};

#endif
