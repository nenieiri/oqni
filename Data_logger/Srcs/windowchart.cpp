#include "windowchart.hpp"
#include "debugger.hpp"

WindowChart::WindowChart(MainWindow *parent, const QString &pathToFiles, \
                        QCheckBox *filesList, int filesCount)
    : QDialog(parent)
    , _pathToFiles(pathToFiles) \
    , _filesList(filesList) \
    , _filesCount(filesCount)
{
    DEBUGGER();
    
    int screenWidth = QApplication::primaryScreen()->size().width();
	int screenHeight = QApplication::primaryScreen()->size().height();
    int windowWidth = screenWidth * 9 / 10;
    int windowHeight = screenHeight * 9 / 10;
    
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
            DEBUGGER();
            
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
            
            DEBUGGER();
    	});
    
    this->setModal(true);
    this->setStyleSheet("background: #e6e6e6;");
    this->raise();
	this->show();
    
    _numOfCH = 0;
    
    _checkedFilesCount = 0;
    for	(int i = 0; i < _filesCount; ++i)
        if (_filesList[i].isChecked() == true)
            ++_checkedFilesCount;
    if (_checkedFilesCount == 0) // for prevent warning in line 114
        _checkedFilesCount = 1;
    
    this->readFromFile();
    this->execChartDialog();
    
    DEBUGGER();
}

WindowChart::~WindowChart()
{
    DEBUGGER();
    
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
    
    DEBUGGER();
}

void    WindowChart::readFromFile(void)
{
    DEBUGGER();
    
    QStringList splitList;
    qint64      time;
    
    QFile		*files = new QFile[_checkedFilesCount];
    QTextStream *ins = new QTextStream[_checkedFilesCount];
    for (int i = 0, j = -1; i < _filesCount; ++i)
    {
        if (_filesList[i].isChecked() == true)
        {
			files[++j].setFileName(_pathToFiles + _filesList[i].text());
			files[j].open(QIODevice::ReadOnly | QIODevice::Text);
			ins[j].setDevice(&(files[j]));
			_numOfCH += ins[j].readLine().count("led"); // counting sum of _numofCh and omitting first line
        }
    }

    _series = new QLineSeries[_numOfCH + 1];
    
    for (int i = 0, j = -1; i < _filesCount; ++i)
    {
        if (_filesList[i].isChecked() == true)
        {
            ++j;
			while (!ins[j].atEnd())
			{
				splitList = ins[j].readLine().split(',');
				time = splitList[0].toLongLong();
                int k = 0;
				for (; k < _numOfCH / _checkedFilesCount; ++k)
					_series[k + j * _numOfCH / _checkedFilesCount].append(time, splitList[k + 1].toUInt());
                if (j == 0)
					_series[_numOfCH].append(time, splitList[k + 1].toUInt());
			}
			files[j].close();
        }
    }

    _timeLineMax = _series[0].at(_series[0].count() - 1).x();
    delete [] files;
    delete [] ins;
    
    DEBUGGER();
}

void    WindowChart::updateValueLineAxis(void)
{
    DEBUGGER();
    
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
    
    DEBUGGER();
}

void    WindowChart::execChartDialog(void)
{
    DEBUGGER();
    
    this->_chart = new QChart();
    
    int i = 0;
    for (; i < _filesCount; ++i)
        if (_filesList[i].isChecked() == true)
            break ;
    _chart->setTitle(this->staticChartTitle(_pathToFiles + _filesList[i].text()));
    
    QFont font;
    font.setBold(true);
    font.setPointSize(14);
    _chart->setTitleFont(font);
    _chart->setBackgroundBrush(QBrush(QColor::fromRgb(235, 255, 255)));
    _chart->legend()->hide();

	for (int i = 0; i < _numOfCH + 1; ++i)
	{
        _chart->addSeries(&_series[i]);
        if (i == _numOfCH)
            _series[i].setColor(Qt::black);
        else
        {
            switch (i % (_numOfCH / _checkedFilesCount)) {
            case 0:
                _series[i].setColor(Qt::green);
                break;
            case 1:
                _series[i].setColor(Qt::red);
                break;
            case 2:
                _series[i].setColor(Qt::blue); // infraRed
                break;
            }
        }
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
            DEBUGGER();
            this->_axisX->setRange(value, value + this->_chartView->_currentAxisXLength);
            DEBUGGER();
		});
    
    this->_verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    this->_verticalScrollBar->setRange(0, 0);
    connect(this->_verticalScrollBar, &QScrollBar::valueChanged, this,
        [=](int value)
    	{
            DEBUGGER();
            this->_axisY->setRange(value, value + this->_chartView->_currentAxisYLength);
            DEBUGGER();
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
    QString text;
    for (int k = 0, j = -1; k < _filesCount; ++k)
    {
        if (_filesList[k].isChecked() == false)
            continue ;
        ++j;        
        for (int i = 0; i < (_numOfCH / _checkedFilesCount); ++i)
        {
            switch (i % (_numOfCH / _checkedFilesCount)){
            case 0:
                text = "green" + QString::number(k + 1) + "  ";
                this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setText(text);
                this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setStyleSheet("color: green; font-size: 14px;");
                break;
            case 1:
                text = "Red" + QString::number(k + 1) + "  ";
                this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setText(text);
                this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setStyleSheet("color: red; font-size: 14px;");
                break;
            case 2:
                text = "infrared" + QString::number(k + 1) + "          ";
                this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setText(text);
                this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setStyleSheet("color: blue; font-size: 14px;");
                break;
            }
            
            this->_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)].setChecked(true);
            this->connectStaticChatCheckBox(i + j * (_numOfCH / _checkedFilesCount));
            this->_hBoxLayout->addWidget(&_checkBoxChannels[i + j * (_numOfCH / _checkedFilesCount)]); 
        }
        if (j + 1 == _checkedFilesCount)
        {
            this->_checkBoxChannels[_numOfCH].setText("Label");
            this->_checkBoxChannels[_numOfCH].setStyleSheet("color: black; font-size: 14px;");
            this->_checkBoxChannels[_numOfCH].setChecked(true);
            this->connectStaticChatCheckBox(_numOfCH);
            this->_hBoxLayout->addWidget(&_checkBoxChannels[_numOfCH]);
        }
    }
    
	this->_gridLayout->addWidget(this->_chartView, 0, 0, 1, 5);
    this->_gridLayout->addWidget(this->_verticalScrollBar, 0, 0, 1, 5, Qt::AlignRight); 
    this->_gridLayout->addWidget(this->_horizontalScrollBar, 0, 0, 1, 5, Qt::AlignBottom); 
    this->_gridLayout->addLayout(_hBoxLayout, 1, 0, 1, 4, Qt::AlignCenter);
    this->_gridLayout->addWidget(this->_zoomToHomeButton, 1, 4, 1, 1, Qt::AlignVCenter); 
    
	this->setLayout(this->_gridLayout);
    
    DEBUGGER();
}

QString WindowChart::staticChartTitle(const QString &selectedFile)
{
    DEBUGGER();
    
    QString tmp = "Unknown file";
    //the Unicode non-breaking space character (\u00A0)
    QString title = "---\u00A0\u00A0\u00A0\u00A0#---\u00A0\u00A0\u00A0\u00A0";

    int lastDot = selectedFile.lastIndexOf('.');
    int lastUnderscoreLine = selectedFile.lastIndexOf('_');
    int lastSlash = selectedFile.lastIndexOf('/');
//    if (lastSlash == -1)
//        lastSlash = selectedFile.lastIndexOf('\\');
    if (lastDot == -1 || lastUnderscoreLine == -1 || lastSlash == -1)
    {
        DEBUGGER();
        return tmp;
    }

    title += selectedFile.mid(lastSlash + 1, lastUnderscoreLine - lastSlash - 1) + \
            "\u00A0\u00A0\u00A0\u00A0";
    for (int i = 0; i < _filesCount; ++i)
    {
        if (_filesList[i].isChecked() == true)
        {
			int lastDot_tmp = _filesList[i].text().lastIndexOf('.');
			int lastUnderscoreLine_tmp = _filesList[i].text().lastIndexOf('_');
			title += _filesList[i].text().mid(lastUnderscoreLine_tmp + 1, lastDot_tmp - lastUnderscoreLine_tmp - 1);
            title += "\u00A0\u00A0\u00A0";
        }
    }

    tmp = selectedFile.left(lastSlash);
    lastSlash = tmp.lastIndexOf('/');
//    if (lastSlash == -1)
//        lastSlash = tmp.lastIndexOf('\\');
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
    DEBUGGER();
    return title;
}

void WindowChart::connectStaticChatCheckBox(int i)
{
    DEBUGGER();
    
    connect(&this->_checkBoxChannels[i], &QCheckBox::clicked, this,
        [=]()
        {
            DEBUGGER();
            
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
    
    DEBUGGER();
}
