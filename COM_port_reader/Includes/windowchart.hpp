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
# include "mainwindow.h"

class	MainWindow;
class	MyChartView;

class	WindowChart : public QDialog
{
	public:
		WindowChart(MainWindow *parent, const QString &selectedFile);
		~WindowChart();
    
    private:
        void			execChartDialog(void);
        void    		readFromFile(void);
		void			updateValueLineAxis(void);
        QString         staticChartTitle(const QString &selectedFile);
    
    private:
        const QString   _selectedFile;
        
        QChart          *_chart;
        MyChartView   	*_chartView;
        QValueAxis		*_axisX;
        QValueAxis		*_axisY;
        QValueAxis		*_axisYLabel;
        QLineSeries		*_series;
        QGridLayout		*_gridLayout;
        QHBoxLayout		*_hBoxLayout;
        QCheckBox		*_checkBoxChannels;
        bool            *_checkBoxChannelsValue;
        QPushButton		*_zoomToHomeButton;
		QIcon			*_iconHome;
        QScrollBar		*_horizontalScrollBar;
        QScrollBar		*_verticalScrollBar;

        char            _numOfCH;
        qint64          _timeLineMin;
        qint64          _timeLineMax;
		unsigned int    _valueLineMin;
		unsigned int    _valueLineMax;
		int             _maxLabel;
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
        void mouseReleaseEvent(QMouseEvent *event) override
        {
            _mRx = this->chart()->mapToValue(event->pos()).x();
            _mRy = this->chart()->mapToValue(event->pos()).y();
            _mRx = (_mRx < _axisX->min()) ? _axisX->min() : _mRx;
            _mRx = (_mRx > _axisX->max()) ? _axisX->max() : _mRx;
            _mRy = (_mRy < _axisY->min()) ? _axisY->min() : _mRy;
            _mRy = (_mRy > _axisY->max()) ? _axisY->max() : _mRy;

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
                _axisX->setRange((_mPx < _mRx) ? _mPx : _mRx, (_mPx > _mRx) ? _mPx : _mRx);
                _axisY->setRange((_mPy < _mRy) ? _mPy : _mRy, (_mPy > _mRy) ? _mPy : _mRy);
                _firstTimeZooming = false;
            }
            _verticalScrollBar->setValue(_axisY->min());

        }
        void mousePressEvent(QMouseEvent *event) override
        {
            _mPx = this->chart()->mapToValue(event->pos()).x();
            _mPy = this->chart()->mapToValue(event->pos()).y();
            _mPx = (_mPx < _axisX->min()) ? _axisX->min() : _mPx;
            _mPx = (_mPx > _axisX->max()) ? _axisX->max() : _mPx;
            _mPy = (_mPy < _axisY->min()) ? _axisY->min() : _mPy;
            _mPy = (_mPy > _axisY->max()) ? _axisY->max() : _mPy;
            QChartView::mousePressEvent(event);
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
