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

    this->_timeLineMin = 0;

    this->readFromFile();    
    this->execChartDialog();
}

WindowChart::~WindowChart()
{
	delete _axisX;
	delete _axisY;
	for (int i = 0; i < _numOfCH; ++i)
        if (_chart->series().contains(&_series[i]))
            this->_chart->removeSeries(&_series[i]);
	delete [] _series;
	delete this->_chart;
	delete _chartView;
	delete _sliderHorizontal;
	delete [] _checkBoxChannelsValue;
	delete [] _checkBoxChannels;
	delete _vBoxLayout;
	delete _gridLayout;
}

void    WindowChart::readFromFile(void)
{
    QStringList splitList;
    QFile       file(_selectedFile);
    qint64      time;
    
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);
    
    _numOfCH = in.readLine().count("led"); // counting _numofCh and omitting first line
    
    _series = new QLineSeries[_numOfCH];
    
    while (!in.atEnd())
    {
        splitList = in.readLine().split(',');
        time = splitList[0].toLongLong();
        for (int i = 1; i <= _numOfCH; ++i)
            _series[i - 1].append(time, splitList[i].toUInt());
    }
	file.close();
    _timeLineMax = _series[0].at(_series[0].count() - 1).x();
}

void    WindowChart::updateValueLineAxis(void)
{
    bool		flag = false;
    
    this->_valueLineMin = -1;
    this->_valueLineMax = 0;

	for (int i = 0; i < _numOfCH; i++)
	{
        if (_checkBoxChannelsValue[i] == false)
            continue ;
        flag = true;
		for(int j = 0; j < _series[i].count(); j++)
		{
            if (_series[i].at(j).x() >= _timeLineMin && _series[i].at(j).x() <= _timeLineMax)
            {
                if(_series[i].at(j).y() > _valueLineMax)
                    _valueLineMax = _series[i].at(j).y();
				if(_series[i].at(j).y() < _valueLineMin)
					_valueLineMin = _series[i].at(j).y();
            }
		}
	}
    if (flag == false)
    {
        this->_valueLineMin = 0;
        this->_valueLineMax = 1;
    }
	_axisY->setRange(_valueLineMin, _valueLineMax);
}

void    WindowChart::execChartDialog(void)
{
	this->_chart = new QChart();
	_chart->setTitle("Static Line Chart");
	_chart->legend()->hide();
	
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
    
	this->updateValueLineAxis();
	_axisX->setRange(_timeLineMin, _timeLineMax);

	this->_chartView = new QChartView(_chart);
	this->_chartView->setRenderHint(QPainter::Antialiasing);

	this->_sliderHorizontal = new QSlider(Qt::Horizontal, this);
	this->_sliderHorizontal->setRange(2, 10);
	this->_sliderHorizontal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	this->_sliderHorizontal->setFixedWidth(300);
	this->_sliderHorizontal->setTickInterval(1);
//	this->_sliderHorizontal->setValue(this->_chartDuration / 1000);
	this->_sliderHorizontal->setFixedHeight(40);
/*
	connect(this->_sliderHorizontal, &QSlider::valueChanged, this,
		[=]()
		{
			this->_chartDuration = this->_sliderHorizontal->value() * 1000;
		});
*/        
	this->_gridLayout = new QGridLayout;
	
	this->_vBoxLayout = new QVBoxLayout;
	this->_checkBoxChannels = new QCheckBox[_numOfCH];
	for (int i = 0; i < _numOfCH; ++i)
	{
		if (i % _numOfCH == 0)
		{
			this->_checkBoxChannels[i].setText("Infrared");
			this->_checkBoxChannels[i].setStyleSheet("color: blue;");
		}
		else if (i % _numOfCH == 1)
		{
			this->_checkBoxChannels[i].setText("Red");
			this->_checkBoxChannels[i].setStyleSheet("color: red;");
		}
		else if (i % _numOfCH == 2)
		{
			this->_checkBoxChannels[i].setText("Green");
			this->_checkBoxChannels[i].setStyleSheet("color: green;");
		}
		else
		{
			this->_checkBoxChannels[i].setText("Other");
			this->_checkBoxChannels[i].setStyleSheet("color: gray;");
		}
		this->_checkBoxChannels[i].setChecked(true);
		connect(&this->_checkBoxChannels[i], &QCheckBox::clicked, this,
			[=]()
			{
				if (this->_checkBoxChannels[i].isChecked() == true)
				{
					_chart->addSeries(&_series[i]);
					_series[i].attachAxis(_axisX);
					_series[i].attachAxis(_axisY);
					this->_checkBoxChannelsValue[i] = true;
				}
				else
				{
					_chart->removeSeries(&_series[i]);
					this->_checkBoxChannelsValue[i] = false;
				}
				this->updateValueLineAxis();
				_chart->update();
			});
		this->_vBoxLayout->addWidget(&_checkBoxChannels[i]); 
	}
	
	this->_gridLayout->addLayout(_vBoxLayout, 0, 0);
	this->_gridLayout->addWidget(this->_chartView, 0, 1);
	this->_gridLayout->addWidget(this->_sliderHorizontal, 1, 0, 1, 2, Qt::AlignCenter);
	this->setLayout(this->_gridLayout);
}
