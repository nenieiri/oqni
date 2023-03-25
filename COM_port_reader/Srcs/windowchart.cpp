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
    
    this->_chartView = nullptr;
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
	delete _sliderLower;
    delete _sliderUpper;
	delete [] _checkBoxChannelsValue;
	delete [] _checkBoxChannels;
	delete _hBoxLayout;
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
    
    _series = new QLineSeries[_numOfCH + 1];
    
    while (!in.atEnd())
    {
        splitList = in.readLine().split(',');
        time = splitList[0].toLongLong();
        for (int i = 1; i <= _numOfCH + 1; ++i)
            _series[i - 1].append(time, splitList[i].toUInt());
    }
	file.close();
    _timeLineMax = _series[0].at(_series[0].count() - 1).x();
}

void    WindowChart::updateValueLineAxis(void)
{
    if (this->_chartView != nullptr && this->_chartView->_zoomed == true)
        return ;
    
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
	_chart->setTitle("Static Line Chart: " + _selectedFile);
	_chart->legend()->hide();
	
	for (int i = 0; i < _numOfCH + 1; ++i)
	{
		_chart->addSeries(&_series[i]);
		if (i % (_numOfCH + 1) == 0)
			_series[i].setColor(Qt::blue); // infraRed
		else if (i % (_numOfCH + 1) == 1)
			_series[i].setColor(Qt::red);
		else if (i % (_numOfCH + 1) == 2)
			_series[i].setColor(Qt::green);
		else if (i % (_numOfCH + 1) == 3)
			_series[i].setColor(Qt::black);
		else
			_series[i].setColor(Qt::gray);
	}

	this->_axisX = new QValueAxis();
	_axisX->setTitleText("Time (milliseconds)");
	_chart->addAxis(_axisX, Qt::AlignBottom);
	for (int i = 0; i < _numOfCH + 1; ++i)
		_series[i].attachAxis(_axisX);

	this->_axisY = new QValueAxis();
	_axisY->setTitleText("Values");
	_chart->addAxis(_axisY, Qt::AlignLeft);
	for (int i = 0; i < _numOfCH; ++i)
		_series[i].attachAxis(_axisY);

	this->_axisYLabel = new QValueAxis();
	_axisYLabel->setTitleText("Label");
	_chart->addAxis(_axisYLabel, Qt::AlignRight);
    _series[_numOfCH].attachAxis(_axisYLabel);
    int maxLabel = 0;
    for (int i = 0; i < _series[_numOfCH].count(); ++i)
        if (maxLabel < _series[_numOfCH].at(i).y())
            maxLabel = _series[_numOfCH].at(i).y();
    _axisYLabel->setRange(0, maxLabel + 1);
	
	this->_checkBoxChannelsValue = new bool[_numOfCH + 1];
	for (int i = 0; i < _numOfCH + 1; ++i)
		this->_checkBoxChannelsValue[i] = true;
    
	this->updateValueLineAxis();
	_axisX->setRange(_timeLineMin, _timeLineMax);

	this->_chartView = new MyChartView(_chart, _timeLineMin, _timeLineMax, _valueLineMin, _valueLineMax, \
                                       _axisX, _axisY, _axisYLabel, maxLabel);
	this->_chartView->setRenderHint(QPainter::Antialiasing);
    this->_chartView->setRubberBand(QChartView::RectangleRubberBand);

	this->_sliderLower = new QSlider(Qt::Horizontal, this);
	this->_sliderLower->setRange(_timeLineMin, _timeLineMax);
	this->_sliderLower->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	this->_sliderLower->setTickInterval(1);
    connect(_sliderLower, &QAbstractSlider::sliderMoved, this, [=](int position) {
        if (position >= _sliderUpper->value()) {
            _sliderLower->setValue(_sliderUpper->value());
        }
        this->_timeLineMin = _sliderLower->value();
        _axisX->setRange(_timeLineMin, _timeLineMax);
        this->updateValueLineAxis();
    });
    
    this->_sliderUpper = new QSlider(Qt::Horizontal, this);
	this->_sliderUpper->setRange(_timeLineMin, _timeLineMax);
    this->_sliderUpper->setValue(_timeLineMax);
	this->_sliderUpper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	this->_sliderUpper->setTickInterval(1);
    connect(_sliderUpper, &QAbstractSlider::sliderMoved, this, [=](int position) {
        if (position <= _sliderLower->value()) {
            _sliderUpper->setValue(_sliderUpper->value());
        }
        this->_timeLineMax = _sliderUpper->value();
        _axisX->setRange(_timeLineMin, _timeLineMax);
        this->updateValueLineAxis();
    });
    
    // Set the custom style sheet for the slider
    QString styleSheet =
        "QSlider::groove:horizontal {"
        "    background: #c0c0c0;"
        "    height: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: white;"
        "    border: 1px solid black;"
        "    width: 10px;"
        "    height: 10px;"
        "    margin: -8px 0;"
        "    border-radius: 5px;"
        "}";
    _sliderLower->setStyleSheet(styleSheet + \
                                "QSlider::add-page:horizontal {background-color: #00bfff;}" + \
                                "QSlider::sub-page:horizontal {background-color: white;}");
    _sliderUpper->setStyleSheet(styleSheet + \
                                "QSlider::add-page:horizontal {background-color: white;}" + \
                                "QSlider::sub-page:horizontal {background-color: #00bfff;}");

	this->_gridLayout = new QGridLayout;
	
	this->_hBoxLayout = new QHBoxLayout;
	this->_checkBoxChannels = new QCheckBox[_numOfCH + 1];
	for (int i = 0; i < _numOfCH + 1; ++i)
	{
		if (i % (_numOfCH + 1) == 0)
		{
			this->_checkBoxChannels[i].setText("Infrared  ");
			this->_checkBoxChannels[i].setStyleSheet("color: blue;");
		}
		else if (i % (_numOfCH + 1) == 1)
		{
			this->_checkBoxChannels[i].setText("Red  ");
			this->_checkBoxChannels[i].setStyleSheet("color: red;");
		}
		else if (i % (_numOfCH + 1) == 2)
		{
			this->_checkBoxChannels[i].setText("Green  ");
			this->_checkBoxChannels[i].setStyleSheet("color: green;");
		}
		else if (i % (_numOfCH + 1) == 3)
		{
			this->_checkBoxChannels[i].setText("Label  ");
			this->_checkBoxChannels[i].setStyleSheet("color: black;");
		}
		else
		{
			this->_checkBoxChannels[i].setText("Other  ");
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
                    if (i && i % _numOfCH == 0)
						_series[i].attachAxis(_axisYLabel);
                    else
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
		this->_hBoxLayout->addWidget(&_checkBoxChannels[i]); 
	}

	this->_gridLayout->addWidget(this->_chartView, 0, 0);
    this->_gridLayout->addLayout(_hBoxLayout, 1, 0, 1, 1, Qt::AlignCenter);
	this->_gridLayout->addWidget(this->_sliderLower, 2, 0);
    this->_gridLayout->addWidget(this->_sliderUpper, 3, 0); 
    
	this->setLayout(this->_gridLayout);
}
