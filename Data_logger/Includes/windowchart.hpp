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
		WindowChart(MainWindow *parent, const QString &pathToFiles, QCheckBox *filesList, int filesCount);
		~WindowChart();
    
    private:
        void			execChartDialog(void);
        void    		readFromFile(void);
		void			updateValueLineAxis(void);
        QString         staticChartTitle(const QString &selectedFile);
        void            connectStaticChatCheckBox(int i);
    
    private:
        const QString   _pathToFiles;
        QCheckBox       *_filesList;
        int             _filesCount;
        int				_checkedFilesCount_OPT;
        int				_checkedFilesCount_IMU;
        
        QChart          *_chart_OPT;
        QChart          *_chart_IMU;
        MyChartView   	*_chartView_OPT;
        MyChartView   	*_chartView_IMU;
        QValueAxis		*_axisX_OPT;
        QValueAxis		*_axisX_IMU;
        QValueAxis		*_axisY_OPT;
        QValueAxis		*_axisY_IMU;
        QValueAxis		*_axisYLabel_OPT;
        QValueAxis		*_axisYLabel_IMU;
        QLineSeries		*_series_OPT;
        QLineSeries		*_series_IMU;
        QGridLayout		*_gridLayout;
        QHBoxLayout		*_hBoxLayout;
        QCheckBox		*_checkBoxChannels;
        bool            *_checkBoxChannelsValue;
        QPushButton		*_zoomToHomeButton;
		QIcon			*_iconHome;
        QScrollBar		*_horizontalScrollBar_OPT;
        QScrollBar		*_verticalScrollBar_OPT;

        char            _numOfSeries_OPT;
        char            _numOfSeries_IMU;
        char            _numOfChart_OPT;
        char            _numOfChart_IMU;
        qint64          _timeLineMin;
        qint64          _timeLineMax_OPT;
        qint64          _timeLineMax_IMU;
        unsigned int    _valueLineMin_OPT;
        unsigned int    _valueLineMin_IMU[3];
        unsigned int    _valueLineMax_OPT;
        unsigned int    _valueLineMax_IMU[3];
        int             _maxLabel_OPT;
        int             _maxLabel_IMU;
};

class	MyChartView : public QChartView
{
	public:
		MyChartView(QChart *parent, \
                    qint64 timeLineMin, \
                    qint64 timeLineMax, \
                    unsigned int valueLineMin, \
                    unsigned int valueLineMax, \
                    QValueAxis *axisX, \
                    QValueAxis *axisY, \
                    QValueAxis *axisYLabel,
                    int maxLabel, \
                    QPushButton *zoomToHomeButton, \
                    QScrollBar *horizontalScrollBar, \
                    QScrollBar *verticalScrollBar)
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
            , _verticalScrollBar(verticalScrollBar)
        {}
	
	protected:
        void mousePressEvent(QMouseEvent *event) override
        {
            if (_firstTimeZooming == true)
            {
                _mPx = this->chart()->mapToValue(event->pos()).x();
                _mPy = this->chart()->mapToValue(event->pos()).y();
                _mPx = std::max((int)_axisX->min(), _mPx);
                _mPx = std::min((int)_axisX->max(), _mPx);
                _mPy = std::max((int)_axisY->min(), _mPy);
                _mPy = std::min((int)_axisY->max(), _mPy);
            }
            QChartView::mousePressEvent(event);
        }
        void mouseReleaseEvent(QMouseEvent *event) override
        {
            if (_firstTimeZooming == true)
            {
                _mRx = this->chart()->mapToValue(event->pos()).x();
                _mRy = this->chart()->mapToValue(event->pos()).y();
                _mRx = std::max((int)_axisX->min(), _mRx);
                _mRx = std::min((int)_axisX->max(), _mRx);
                _mRy = std::max((int)_axisY->min(), _mRy);
                _mRy = std::min((int)_axisY->max(), _mRy);
            }
            if (event->button() == Qt::RightButton)
            {
                _axisX->setRange(_timeLineMin, _timeLineMax);
                _axisY->setRange(_valueLineMin, _valueLineMax);
                _axisYLabel->setRange(0, _maxLabel + 1);
                _zoomed = false;
				_zoomToHomeButton->setEnabled(false);
				_currentAxisXLength = _timeLineMax - _timeLineMin;
    			_horizontalScrollBar->setRange(_timeLineMin, _timeLineMin);
				_horizontalScrollBar->setValue(_timeLineMin);
				_currentAxisYLength = _valueLineMax - _valueLineMin;
				_verticalScrollBar->setRange(_valueLineMin, _valueLineMin);
				_verticalScrollBar->setValue(_valueLineMin);
                return;
            }
            QChartView::mouseReleaseEvent(event);
            _zoomed = true;
            _zoomToHomeButton->setEnabled(true);
            _currentAxisXLength = _axisX->max() - _axisX->min();
            _horizontalScrollBar->setRange(_timeLineMin, _timeLineMax - _currentAxisXLength);
            _horizontalScrollBar->setValue(_axisX->min());
            _currentAxisYLength = _axisY->max() - _axisY->min();
            _verticalScrollBar->setRange(_valueLineMin, _valueLineMax - _currentAxisYLength);
            if (_firstTimeZooming == true)
            {
                _axisX->setRange(std::min(_mPx, _mRx), std::max(_mPx, _mRx));
                _axisY->setRange(std::min(_mPy, _mRy), std::max(_mPy, _mRy));
                _firstTimeZooming = false;
            }
            _verticalScrollBar->setValue(_axisY->min());
            _axisYLabel->setRange(0, _maxLabel + 1);

        }

    private:
        QValueAxis		*_axisX;
        QValueAxis		*_axisY;
        QValueAxis		*_axisYLabel;        
        qint64          _timeLineMin;
        qint64          _timeLineMax;
		unsigned int    _valueLineMin;
		unsigned int    _valueLineMax;
        int				_maxLabel;
        QPushButton		*_zoomToHomeButton;
        QScrollBar		*_horizontalScrollBar;
        QScrollBar		*_verticalScrollBar;
        int             _mPx;   // mouse press X
        int             _mPy;   // mouse press Y
        int             _mRx;   // mouse release X
        int             _mRy;   // mouse release Y
        
    public:
        bool			_zoomed;
        bool            _firstTimeZooming;
        int				_currentAxisXLength;
        int				_currentAxisYLength;
};

#endif
