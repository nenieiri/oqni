#include "windowchart.hpp"
#include "mainwindow.h"
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
    this->_series_OPT = nullptr;
    this->_series_IMU = nullptr;
    this->_maxLabel = 0;
    this->_timeLineMin = 0;
    this->_timeLineMax_OPT = 0;
    this->_timeLineMax_IMU = 0;
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
            
            _axisX->setRange(_timeLineMin, _timeLineMax_OPT);
			_axisY->setRange(_valueLineMin, _valueLineMax);
            _axisYLabel->setRange(0, _maxLabel + 1);
            _chartView->_zoomed = false;
			_zoomToHomeButton->setEnabled(false);
            _chartView->_currentAxisXLength = _timeLineMax_OPT - _timeLineMin;
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
    
    this->_numOfCH_OPT = 0;
    this->_numOfCH_IMU = 0;

    this->_checkedFilesCount_OPT = 0;
    this->_checkedFilesCount_IMU = 0;
    for	(int i = 0; i < _filesCount; ++i)
    {
        if (_filesList[i].isChecked() == true)
        {
            if (_filesList[i].text().mid(14,3) == "IMU")
                ++_checkedFilesCount_IMU;
            else if (_filesList[i].text().mid(14,3) == "OPT")
                ++_checkedFilesCount_OPT;
        }
    }
    
    this->readFromFile();
    this->execChartDialog();
    
    DEBUGGER();
}

WindowChart::~WindowChart()
{
    DEBUGGER();
    
    delete _axisX;
    _axisX = nullptr;
	delete _axisY;
    _axisY = nullptr;
    for (int i = 0; i < _numOfCH_OPT; ++i)
        if (_chart->series().contains(&_series_OPT[i]))
            _chart->removeSeries(&_series_OPT[i]);
    delete[] _series_OPT;
    _series_OPT = nullptr;
    delete[] _series_IMU;
    _series_IMU = nullptr;
    delete _chart;
    _chart = nullptr;
	delete _chartView;
    _chartView = nullptr;
	delete _horizontalScrollBar;
    _horizontalScrollBar = nullptr;
	delete _verticalScrollBar;
    _verticalScrollBar = nullptr;
	delete[] _checkBoxChannelsValue;
    _checkBoxChannelsValue = nullptr;
	delete[] _checkBoxChannels;
    _checkBoxChannels = nullptr;
	delete _hBoxLayout;
    _hBoxLayout = nullptr;
	delete _gridLayout;
    _gridLayout = nullptr;
	delete _zoomToHomeButton;
    _zoomToHomeButton = nullptr;
    
    DEBUGGER();
}

void    WindowChart::readFromFile(void)
{
    DEBUGGER();
    
    QStringList splitList;
    qint64      time;
    
    QFile		*files = new QFile[_checkedFilesCount_OPT + _checkedFilesCount_IMU];
    QTextStream *ins = new QTextStream[_checkedFilesCount_OPT + _checkedFilesCount_IMU];

    // open files and count the number of channels (series) for OPT and IMU
    DEBUGGER();
    for (int i = 0, j = -1; i < _filesCount; ++i)
    {
        if (_filesList[i].isChecked())
        {
			files[++j].setFileName(_pathToFiles + _filesList[i].text());
			files[j].open(QIODevice::ReadOnly | QIODevice::Text);
            ins[j].setDevice(&(files[j]));
            if (_filesList[i].text().mid(14,3) == "IMU")
                _numOfCH_IMU += ins[j].readLine().count("led"); // counting number of IMU channels and omitting first line in file
            else if (_filesList[i].text().mid(14,3) == "OPT")
                _numOfCH_OPT += ins[j].readLine().count("led"); // counting number of OPT channels and omitting first line in file
        }
    }

    // creating series if necessary
    DEBUGGER();
    if (_checkedFilesCount_OPT)
        _series_OPT = new QLineSeries[_numOfCH_OPT + 1]; // +1 for label
    if (_checkedFilesCount_IMU)
        _series_IMU = new QLineSeries[_numOfCH_IMU + 1]; // +1 for label

    // reading data from files to series
    DEBUGGER();
    bool labelIsRead_OPT = false;
    for (int i = 0, j = 0; i < _filesCount; ++i)
    {
        if (_filesList[i].isChecked())
        {
            while (!ins[j].atEnd())
            {
                splitList = ins[j].readLine().split(',');
                time = splitList[0].toLongLong();

                if (_filesList[i].text().mid(14,3) == "IMU")
                {
                    int k = 0;
                    if (_checkedFilesCount_IMU)
                        for (; k < _numOfCH_IMU / _checkedFilesCount_IMU; ++k)
                            _series_IMU[k + j * _numOfCH_IMU / _checkedFilesCount_IMU].append(time, splitList[k + 1].toUInt()); // k+1, because at index 0 is the time in millisec
                    _series_IMU[_numOfCH_IMU].append(time, splitList[k + 1].toUInt()); // label

                }
                else if (_filesList[i].text().mid(14,3) == "OPT")
                {
                    int k = 0;
                    if (_checkedFilesCount_OPT)
                        for (; k < _numOfCH_OPT / _checkedFilesCount_OPT; ++k)
                            _series_OPT[k + (j - _checkedFilesCount_IMU) * _numOfCH_OPT / _checkedFilesCount_OPT].append(time, splitList[k + 1].toUInt()); // k+1, because at index 0 is the time in millisec
                    if (!labelIsRead_OPT)
                        _series_OPT[_numOfCH_OPT].append(time, splitList[k + 1].toUInt()); // label
                }
            }
            files[j++].close();
            if (_filesList[i].text().mid(14,3) == "OPT")
                labelIsRead_OPT = true;
        }
    }

    DEBUGGER();
    if (_checkedFilesCount_OPT)
        _timeLineMax_OPT = _series_OPT[0].at(_series_OPT[0].count() - 1).x();
    if (_checkedFilesCount_IMU)
        _timeLineMax_IMU = _series_IMU[0].at(_series_IMU[0].count() - 1).x();

    delete [] files;
    delete [] ins;
    
    DEBUGGER();
}

void    WindowChart::updateValueLineAxis(void)
{
    DEBUGGER();
    
    if (this->_chartView != nullptr && this->_chartView->_zoomed == true)
        return ;
    
    bool flag = false;
    
    this->_valueLineMin = -1;
    this->_valueLineMax = 0;

    for (int i = 0; i < _numOfCH_OPT; i++)
	{
        if (_checkBoxChannelsValue[i] == false)
            continue ;
        flag = true;
        for(int j = 0; j < _series_OPT[i].count(); j++)
		{
            if (_series_OPT[i].at(j).x() >= _timeLineMin && _series_OPT[i].at(j).x() <= _timeLineMax_OPT)
            {
                _valueLineMax = std::max((unsigned)_series_OPT[i].at(j).y(), _valueLineMax);
                _valueLineMin = std::min((unsigned)_series_OPT[i].at(j).y(), _valueLineMin);
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

    for (int i = 0; i < _numOfCH_OPT + 1; ++i)
	{
        _chart->addSeries(&_series_OPT[i]);
        if (i == _numOfCH_OPT)
            _series_OPT[i].setColor(Qt::black);
        else
        {
            switch (i % (_numOfCH_OPT / _checkedFilesCount_OPT)) {
            case 0:
                _series_OPT[i].setColor(Qt::green);
                break;
            case 1:
                _series_OPT[i].setColor(Qt::red);
                break;
            case 2:
                _series_OPT[i].setColor(Qt::blue); // infraRed
                break;
            }
        }
	}

	this->_axisX = new QValueAxis();
	_axisX->setTitleText("Time (milliseconds)");
	_chart->addAxis(_axisX, Qt::AlignBottom);
    for (int i = 0; i < _numOfCH_OPT + 1; ++i)
        _series_OPT[i].attachAxis(_axisX);

	this->_axisY = new QValueAxis();
	_axisY->setTitleText("Values");
	_chart->addAxis(_axisY, Qt::AlignLeft);
    for (int i = 0; i < _numOfCH_OPT; ++i)
        _series_OPT[i].attachAxis(_axisY);

	this->_axisYLabel = new QValueAxis();
	_axisYLabel->setTitleText("Label");
	_chart->addAxis(_axisYLabel, Qt::AlignRight);
    _series_OPT[_numOfCH_OPT].attachAxis(_axisYLabel);
    for (int i = 0; i < _series_OPT[_numOfCH_OPT].count(); ++i)
        _maxLabel = std::max((int)_series_OPT[_numOfCH_OPT].at(i).y(), _maxLabel);
    _axisYLabel->setRange(0, _maxLabel + 1);
	
    this->_checkBoxChannelsValue = new bool[_numOfCH_OPT + 1];
    for (int i = 0; i < _numOfCH_OPT + 1; ++i)
		this->_checkBoxChannelsValue[i] = true;
    
	this->updateValueLineAxis();
    _axisX->setRange(_timeLineMin, _timeLineMax_OPT);
    
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

    this->_chartView = new MyChartView(_chart, _timeLineMin, _timeLineMax_OPT, _valueLineMin, _valueLineMax, \
                                       _axisX, _axisY, _axisYLabel, _maxLabel, \
                                       _zoomToHomeButton, _horizontalScrollBar, _verticalScrollBar);
	this->_chartView->setRenderHint(QPainter::Antialiasing);
    this->_chartView->setRubberBand(QChartView::RectangleRubberBand);
    
    this->_horizontalScrollBar->setParent(this->_chartView);
    this->_verticalScrollBar->setParent(this->_chartView);
    this->_verticalScrollBar->setInvertedAppearance(true); // reverse the direction
    
	this->_gridLayout = new QGridLayout;
	
	this->_hBoxLayout = new QHBoxLayout;
    this->_checkBoxChannels = new QCheckBox[_numOfCH_OPT + 1];

    for (int k = 0, j = -1; k < _filesCount; ++k)
    {
        if (_filesList[k].isChecked() == false || _filesList[k].text().mid(14,3) == "IMU")
            continue ;
        ++j;        
        for (int i = 0; i < (_numOfCH_OPT / _checkedFilesCount_OPT); ++i)
        {
            switch (i % (_numOfCH_OPT / _checkedFilesCount_OPT)){
            case 0:
                this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setText("OPT" + QString::number(k) + "green  ");
                this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setStyleSheet("color: green; font-size: 14px;");
                break;
            case 1:
                this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setText("OPT" + QString::number(k) + "red  ");
                this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setStyleSheet("color: red; font-size: 14px;");
                break;
            case 2:
                this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setText("OPT" + QString::number(k) + "infrared          ");
                this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setStyleSheet("color: blue; font-size: 14px;");
                break;
            }
            
            this->_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)].setChecked(true);
            this->connectStaticChatCheckBox(i + j * (_numOfCH_OPT / _checkedFilesCount_OPT));
            this->_hBoxLayout->addWidget(&_checkBoxChannels[i + j * (_numOfCH_OPT / _checkedFilesCount_OPT)]);
        }
        if (j + 1 == _checkedFilesCount_OPT)
        {
            this->_checkBoxChannels[_numOfCH_OPT].setText("Label");
            this->_checkBoxChannels[_numOfCH_OPT].setStyleSheet("color: black; font-size: 14px;");
            this->_checkBoxChannels[_numOfCH_OPT].setChecked(true);
            this->connectStaticChatCheckBox(_numOfCH_OPT);
            this->_hBoxLayout->addWidget(&_checkBoxChannels[_numOfCH_OPT]);
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
                _chart->addSeries(&_series_OPT[i]);
                _series_OPT[i].attachAxis(_axisX);
                if (i && i % _numOfCH_OPT == 0)
                    _series_OPT[i].attachAxis(_axisYLabel);
                else
                    _series_OPT[i].attachAxis(_axisY);
                this->_checkBoxChannelsValue[i] = true;
            }
            else
            {
                _chart->removeSeries(&_series_OPT[i]);
                this->_checkBoxChannelsValue[i] = false;
            }
            this->updateValueLineAxis();
            _chart->update();
        });
    
    DEBUGGER();
}
