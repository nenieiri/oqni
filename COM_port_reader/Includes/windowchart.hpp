#ifndef WINDOWCHART_HPP
# define WINDOWCHART_HPP

# include <QDialog>
# include <QString>
# include <QtCharts>
# include <QFile>
# include <QTextStream>
# include "mainwindow.h"

class MainWindow;

class WindowChart : public QDialog
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
        
        QChart      		*_chart;
        QChartView		*_chartView;
        QValueAxis		*_axisX;
        QValueAxis		*_axisY;
        QLineSeries		*_series;
        QGridLayout		*_gridLayout;
        QVBoxLayout		*_vBoxLayout;
        QCheckBox		*_checkBoxChannels;
        bool            	*_checkBoxChannelsValue;
        
        char				_numOfCH;
        qint64          _timeLineMin;
        qint64          _timeLineMax;
		unsigned int		_valueLineMin;
		unsigned int		_valueLineMax;
};

#endif
