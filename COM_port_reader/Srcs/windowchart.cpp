#include "windowchart.hpp"

WindowChart::WindowChart(MainWindow *parent, const QString &selectedFile)
    : QDialog(parent)
    , _selectedFile(selectedFile)
{
	int screenWidth = QApplication::primaryScreen()->size().width();
	int screenHeight = QApplication::primaryScreen()->size().height();
	int windowWidth = screenWidth - screenWidth / 4;
	int windowHeight = screenHeight - screenHeight / 4;
    
	this->setGeometry((screenWidth - windowWidth) / 2, \
                      (screenHeight - windowHeight) / 2, windowWidth, windowHeight);
	this->setMinimumHeight(windowHeight / 2);
	this->setMinimumWidth(windowWidth / 2);
    
    this->setModal(true);
    this->setStyleSheet("background: #e6e6e6;");
    this->raise();
	this->show();
    
    this->execChartDialog();
}

WindowChart::~WindowChart()
{
	delete _axisX;
	this->_axisX = nullptr;
	delete _axisY;
	this->_axisY = nullptr;
	for (int i = 0; i < _numOfCH; ++i)
		this->_chart->removeSeries(&_series[i]);
	delete [] _series;
	this->_series = nullptr;
	delete this->_chart;
	this->_chart = nullptr;
	delete _chartView;
	this->_chartView = nullptr;
//	delete _sliderHorizontal;
//	this->_sliderHorizontal = nullptr;
	delete [] _checkBoxChannelsValue;
	this->_checkBoxChannelsValue = nullptr;
	delete [] _checkBoxChannels;
	this->_checkBoxChannels = nullptr;
	delete _vBoxLayout;
	this->_vBoxLayout = nullptr;
	delete _gridLayout;
	this->_gridLayout = nullptr;
}

void    WindowChart::execChartDialog(void)
{
        this->_chart = new QChart();
        _chart->setTitle("Static Line Chart");
        _chart->legend()->hide();
        
        _series = new QLineSeries[_numOfCH];
        for (int i = 0; i < _numOfCH; ++i)
        {
            _chart->addSeries(&_series[i]);
            if (i % _numOfCH == 0)
				_series[i].setColor(Qt::blue); // infraRed
            else if (i % _numOfCH == 1)
				_series[i].setColor(Qt::red);
            else if (i % _numOfCH == 2)
				_series[i].setColor(Qt::green);
            else
				_series[i].setColor(Qt::gray);
        }
        
        this->_axisX = new QValueAxis();
        _axisX->setTitleText("Time (milliseconds)");
        _chart->addAxis(_axisX, Qt::AlignBottom);
        for (int i = 0; i < _numOfCH; ++i)
            _series[i].attachAxis(_axisX);
    
        this->_axisY = new QValueAxis();
        _axisY->setTitleText("Values");
        _chart->addAxis(_axisY, Qt::AlignLeft);
        for (int i = 0; i < _numOfCH; ++i)
            _series[i].attachAxis(_axisY);
        
        this->_checkBoxChannelsValue = new bool[_numOfCH];
        for (int i = 0; i < _numOfCH; ++i)
            this->_checkBoxChannelsValue[i] = true;
/*
	unsigned int    value;
	int             ledID;
	unsigned int    minY = -1;
	unsigned int    maxY = 0;
	char            id = qFromBigEndian<unsigned char>(data.mid(_bytesPA, _bytesID).constData());
	qint64          time = qFromLittleEndian<qint64>(data.mid(_totalBytes - 8 - 1, 8).constData()) - _startTime;
	qint64          minX = time;
*/
/*
	for (int j = 0; j < _numOfCH; ++j)
	{
		value = qFromLittleEndian<unsigned int>(data.mid(_bytesPA + _bytesID + _bytesCO + _bytesCH + \
												_bytesOCH + j * _sizeOfCH, _sizeOfCH).constData());
		ledID = j + id * id - 1;

		if (_checkBoxChannelsValue[ledID] == true)
		{
			_series[ledID].append(time, value);
			while (_series[ledID].count() > _chartDuration / 10)
				_series[ledID].remove(0);
		}

		for (int i = 0; i < _numOfCH; i++)
		{
			for(int j = 0; j < _series[i].count(); j++)
			{
				if(_series[i].at(j).y() > maxY)
					maxY = _series[i].at(j).y();
				if(_series[i].at(j).y() < minY)
					minY = _series[i].at(j).y();
			}
		}
		_axisY->setRange(minY, maxY);

		for (int k = 0; k < _numOfCH; ++k)
		{
			if (_series[k].count() == 0)
				continue ;
			if (_series[k].at(0).x() < minX)
				minX = _series[k].at(0).x();
		}
		_axisX->setRange(minX, minX + _chartDuration);
	}
*/
        
        this->_chartView = new QChartView(_chart);
        this->_chartView->setRenderHint(QPainter::Antialiasing);
/*        
        this->_sliderHorizontal = new MySlider(Qt::Horizontal, _chartDialog);
        this->_sliderHorizontal->setRange(2, 10);
        this->_sliderHorizontal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        this->_sliderHorizontal->setFixedWidth(300);
        this->_sliderHorizontal->setTickInterval(1);
        this->_sliderHorizontal->setValue(this->_chartDuration / 1000);
        this->_sliderHorizontal->setFixedHeight(40);
                
        connect(this->_sliderHorizontal, &QSlider::valueChanged, this,
            [=]()
        	{
            	this->_chartDuration = this->_sliderHorizontal->value() * 1000;
        	});
*/        
        this->_gridLayout = new QGridLayout;
        
        this->_vBoxLayout = new QVBoxLayout;
        this->_checkBoxChannels = new QCheckBox[_numOfCH];
        int j = -1;
        int n = 1;
        for (int i = 0; i < _numOfCH; ++i)
        {
            if (++j >= _numOfCH)
            {
                j = 0;
                n++;
            }
            if (i % _numOfCH == 0)
            {
				this->_checkBoxChannels[i].setText("Infrared " + QString::number(n));
				this->_checkBoxChannels[i].setStyleSheet("color: blue;");
            }
            else if (i % _numOfCH == 1)
            {
				this->_checkBoxChannels[i].setText("Red " + QString::number(n));
				this->_checkBoxChannels[i].setStyleSheet("color: red;");
            }
            else if (i % _numOfCH == 2)
            {
				this->_checkBoxChannels[i].setText("Green " + QString::number(n));
				this->_checkBoxChannels[i].setStyleSheet("color: green;");
            }
            else
            {
				this->_checkBoxChannels[i].setText("Other " + QString::number(n));
				this->_checkBoxChannels[i].setStyleSheet("color: gray;");
            }
            this->_checkBoxChannels[i].setChecked(true);
			connect(&this->_checkBoxChannels[i], &QCheckBox::clicked, this,
				[=]()
				{
					if (this->_checkBoxChannels[i].isChecked() == true)
                    {
                        // fill series
                        this->_checkBoxChannelsValue[i] = true;
                    }
                    else
                    {
                        _series[i].clear();
                        this->_checkBoxChannelsValue[i] = false;
                    }
				});
			this->_vBoxLayout->addWidget(&_checkBoxChannels[i]); 
        }
        
        this->_gridLayout->addLayout(_vBoxLayout, 0, 0);
        this->_gridLayout->addWidget(this->_chartView, 0, 1);
//        this->_gridLayout->addWidget(this->_sliderHorizontal, 1, 0, 1, 2, Qt::AlignCenter);
        this->setLayout(this->_gridLayout);
}
