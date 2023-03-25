#ifndef WINDOWCHART_HPP
# define WINDOWCHART_HPP

# include <QDialog>
# include <QString>
# include <QtCharts>
# include <QFile>
# include <QTextStream>
# include <QSlider>
# include <QChartView>
# include "mainwindow.h"

class	MainWindow;
class	MyChartView;

class	WindowChart : public QDialog
{
	public:
		WindowChart(MainWindow *parent, const QString &selectedFile);
		~WindowChart();
    
    private:
        void				execChartDialog(void);
        void        		readFromFile(void);
		void				updateValueLineAxis(void);
    
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
        QSlider 		*_sliderLower;
        QSlider 		*_sliderUpper;
        QPushButton		*_zoomToHomeButton;
		QIcon			*_iconHome;

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
                    QPushButton *zoomToHomeButton)
            : QChartView(parent) \
            , _timeLineMin(timeLineMin) \
            , _timeLineMax(timeLineMax) \
            , _valueLineMin(valueLineMin) \
            , _valueLineMax(valueLineMax) \
            , _axisX(axisX) \
            , _axisY(axisY) \
            , _axisYLabel(axisYLabel) \
            , _maxLabel(maxLabel) \
            , _zoomed(false)
            , _zoomToHomeButton(zoomToHomeButton)
        {}
	
	protected:
        void mouseReleaseEvent(QMouseEvent *event) override
        {
            if (event->button() == Qt::RightButton)
            {
                _axisX->setRange(_timeLineMin, _timeLineMax);
                _axisY->setRange(_valueLineMin, _valueLineMax);
                _axisYLabel->setRange(0, _maxLabel + 1);
                _zoomed = false;
				_zoomToHomeButton->setEnabled(false);
                return;
            }
            QChartView::mouseReleaseEvent(event);
			_zoomed = true;
            _zoomToHomeButton->setEnabled(true);
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
        
    public:
        bool			_zoomed;
};

#endif
