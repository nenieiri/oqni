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
    this->_maxLabel = 0;
    this->_timeLineMin = 0;
    this->_zoomToHomeButton = new QPushButton;
    this->_zoomToHomeButton->setEnabled(false);
    QPixmap pixmap(":/Imgs/iconHome.png");
    this->_iconHome = new QIcon(pixmap);
    this->_zoomToHomeButton->setIcon(*_iconHome);
    this->_zoomToHomeButton->setIconSize(pixmap.size());
    this->_zoomToHomeButton->setFixedSize(pixmap.size());
    this->_zoomToHomeButton->setStyleSheet("QPushButton { border: none; }");
    this->_zoomToHomeButton->setMask(pixmap.mask());
    connect(this->_zoomToHomeButton, &QPushButton::clicked, this,
        [=]()
        {
			_axisX->setRange(_timeLineMin, _timeLineMax);
			_axisY->setRange(_valueLineMin, _valueLineMax);
			_axisYLabel->setRange(0, _maxLabel + 1);
            _chartView->_zoomed = false;
			_zoomToHomeButton->setEnabled(false);
            _chartView->_currentAxisXLength = _timeLineMax - _timeLineMin;
			_horizontalScrollBar->setRange(_timeLineMin, _timeLineMin);
			_horizontalScrollBar->setValue(_timeLineMin);
            _chartView->_currentAxisYLength = _valueLineMax - _valueLineMin;
			_verticalScrollBar->setRange(_valueLineMin, _valueLineMin);
			_verticalScrollBar->setValue(_valueLineMin);
    	});
    
    this->setModal(true);
    this->setStyleSheet("background: #e6e6e6;");
    this->raise();
	this->show();
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
	delete[] _series;
	delete this->_chart;
	delete _chartView;
	delete _horizontalScrollBar;
	delete _verticalScrollBar;
	delete[] _checkBoxChannelsValue;
	delete[] _checkBoxChannels;
	delete _hBoxLayout;
	delete _gridLayout;
	delete _zoomToHomeButton;
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
        for (int i = 0; i < _numOfCH + 1; ++i)
            _series[i].append(time, splitList[i + 1].toUInt());
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
    _chart->setTitle(this->staticChartTitle(_selectedFile));
    QFont font;
    font.setBold(true);
    font.setPointSize(14);
    _chart->setTitleFont(font);
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
    for (int i = 0; i < _series[_numOfCH].count(); ++i)
        if (_maxLabel < _series[_numOfCH].at(i).y())
            _maxLabel = _series[_numOfCH].at(i).y();
    _axisYLabel->setRange(0, _maxLabel + 1);
	
	this->_checkBoxChannelsValue = new bool[_numOfCH + 1];
	for (int i = 0; i < _numOfCH + 1; ++i)
		this->_checkBoxChannelsValue[i] = true;
    
	this->updateValueLineAxis();
	_axisX->setRange(_timeLineMin, _timeLineMax);
    
    this->_horizontalScrollBar = new QScrollBar(Qt::Horizontal, this);
    this->_horizontalScrollBar->setRange(0, 0);
    connect(this->_horizontalScrollBar, &QScrollBar::valueChanged, this,
        [=](int value)
    	{
			this->_axisX->setRange(value, value + this->_chartView->_currentAxisXLength);
		});
    
    this->_verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    this->_verticalScrollBar->setRange(0, 0);
    connect(this->_verticalScrollBar, &QScrollBar::valueChanged, this,
        [=](int value)
    	{
			this->_axisY->setRange(value, value + this->_chartView->_currentAxisYLength);
		});

	this->_chartView = new MyChartView(_chart, _timeLineMin, _timeLineMax, _valueLineMin, _valueLineMax, \
                                       _axisX, _axisY, _axisYLabel, _maxLabel, \
                                       _zoomToHomeButton, _horizontalScrollBar, _verticalScrollBar);
	this->_chartView->setRenderHint(QPainter::Antialiasing);
    this->_chartView->setRubberBand(QChartView::RectangleRubberBand);
    
    this->_horizontalScrollBar->setParent(this->_chartView);
    this->_verticalScrollBar->setParent(this->_chartView);
    this->_verticalScrollBar->setInvertedAppearance(true); // reverse the direction
    
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
			this->_checkBoxChannels[i].setText("Green         ");
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

	this->_gridLayout->addWidget(this->_chartView, 0, 0, 1, 5);
    this->_gridLayout->addWidget(this->_verticalScrollBar, 0, 0, 1, 5, Qt::AlignRight); 
    this->_gridLayout->addWidget(this->_horizontalScrollBar, 0, 0, 1, 5, Qt::AlignBottom); 
    this->_gridLayout->addLayout(_hBoxLayout, 1, 0, 1, 4, Qt::AlignCenter);
    this->_gridLayout->addWidget(this->_zoomToHomeButton, 1, 4, 1, 1, Qt::AlignVCenter); 
    
	this->setLayout(this->_gridLayout);
}

QString WindowChart::staticChartTitle(const QString &selectedFile)
{
    QString tmp = "Unknown file";
    //the Unicode non-breaking space character (\u00A0)
    QString title = "---\u00A0\u00A0\u00A0\u00A0#---\u00A0\u00A0\u00A0\u00A0";

    int lastDot = selectedFile.lastIndexOf('.');
    int lastUnderscoreLine = selectedFile.lastIndexOf('_');
    int lastSlash = selectedFile.lastIndexOf('/');
    if (lastSlash == -1)
        lastSlash = selectedFile.lastIndexOf('\\');
    if (lastDot == -1 || lastUnderscoreLine == -1 || lastSlash == -1)
        return tmp;

    title += selectedFile.mid(lastSlash + 1, lastUnderscoreLine - lastSlash - 1) + \
            "\u00A0\u00A0\u00A0\u00A0" + \
            selectedFile.mid(lastUnderscoreLine + 1, lastDot - lastUnderscoreLine - 1);

    tmp = selectedFile.left(lastSlash);
    lastSlash = tmp.lastIndexOf('/');
    if (lastSlash == -1)
        lastSlash = tmp.lastIndexOf('\\');
    tmp = tmp.mid(lastSlash + 1);

    lastUnderscoreLine = tmp.lastIndexOf('_');
    if (lastUnderscoreLine != -1)
    {
        tmp = tmp.left(lastUnderscoreLine);
        lastUnderscoreLine = tmp.lastIndexOf('_');
        if (lastUnderscoreLine != -1)
            title = tmp.left(lastUnderscoreLine) + "\u00A0\u00A0\u00A0\u00A0#" + \
                    tmp.mid(lastUnderscoreLine + 1) + title.mid(11);
    }
    return title;
}
