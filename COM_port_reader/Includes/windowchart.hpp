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
        QVBoxLayout		*_vBoxLayout;
        QCheckBox		*_checkBoxChannels;
        bool            *_checkBoxChannelsValue;
        QSlider 		*_sliderLower;
        QSlider 		*_sliderUpper;
        QLabel          *_sliderLowerName;
        QLabel          *_sliderUpperName;

        char            _numOfCH;
        qint64          _timeLineMin;
        qint64          _timeLineMax;
		unsigned int    _valueLineMin;
		unsigned int    _valueLineMax;
};

class	MyChartView : public QChartView
{
	public:
		MyChartView(QChart *parent = nullptr)
            : QChartView(parent)
        {}
	
	protected:
        void mousePressEvent(QMouseEvent *event) override
        {
            if (event->button() == Qt::RightButton)
            {
                event->ignore();
                return;
            }
            QChartView::mousePressEvent(event);
        }
};

#endif
