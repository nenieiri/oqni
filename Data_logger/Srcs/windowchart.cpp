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
    
    this->_chart_OPT = nullptr;
    this->_chart_IMU = nullptr;
    this->_chartView_OPT = nullptr;
    this->_chartView_IMU = nullptr;
    this->_axisX_OPT = nullptr;
    this->_axisX_IMU = nullptr;
    this->_axisY_OPT = nullptr;
    this->_axisY_IMU = nullptr;
    this->_axisYLabel_OPT = nullptr;
    this->_axisYLabel_IMU = nullptr;
    this->_series_OPT = nullptr;
    this->_series_IMU = nullptr;
    this->_maxLabel_OPT = 0;
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
            
            _axisX_OPT->setRange(_timeLineMin, _timeLineMax_OPT);
            _axisY_OPT->setRange(_valueLineMin_OPT, _valueLineMax_OPT);
            _axisYLabel_OPT->setRange(0, _maxLabel_OPT + 1);

            _chartView_OPT->_zoomed = false;
            _chartView_OPT->_currentAxisXLength = _timeLineMax_OPT - _timeLineMin;
            _chartView_OPT->_currentAxisYLength = _valueLineMax_OPT - _valueLineMin_OPT;
            for (int i = 0; i < _chartView_IMU->size().height(); ++i)
            {
                _chartView_IMU[i]._zoomed = false;
                _chartView_IMU[i]._currentAxisXLength = _timeLineMax_IMU - _timeLineMin;
                _chartView_IMU[i]._currentAxisYLength = _valueLineMax_IMU[i] - _valueLineMin_IMU[i];
            }

			_zoomToHomeButton->setEnabled(false);

            _horizontalScrollBar_OPT->setRange(_timeLineMin, _timeLineMin);
            _horizontalScrollBar_OPT->setValue(_timeLineMin);

            _verticalScrollBar_OPT->setRange(_valueLineMin_OPT, _valueLineMin_OPT);
            _verticalScrollBar_OPT->setValue(_valueLineMin_OPT);
            
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
    
    delete _axisX_OPT;
    _axisX_OPT = nullptr;
    delete [] _axisX_IMU;
    _axisX_IMU = nullptr;
    delete _axisY_OPT;
    _axisY_OPT = nullptr;
    delete [] _axisY_IMU;
    _axisY_IMU = nullptr;
    for (int i = 0; i < _numOfCH_OPT + 1; ++i) // +1 for label
        if (_chart_OPT->series().contains(&_series_OPT[i]))
            _chart_OPT->removeSeries(&_series_OPT[i]);
    delete[] _series_OPT;
    _series_OPT = nullptr;
    for (int i = 0; i < _numOfCH_IMU + 1; ++i) // +1 for label
    {
        switch (i / 3) {
        case 0:
        case 1:
        case 2:
            _chart_IMU[i / 3].removeSeries(&_series_IMU[i]); // removing sersor's series
            break;
        case 3:
            for (int j = 0; j < _numOfCH_IMU / 3; ++j) // removing label series
                _chart_IMU[j].removeSeries(&_series_IMU[i]);
            break;
        }
        _chart_IMU[i].removeSeries(&_series_OPT[i]);
    }
    delete[] _series_IMU;
    _series_IMU = nullptr;
    delete _chart_OPT;
    _chart_OPT = nullptr;
    delete [] _chart_IMU;
    _chart_IMU = nullptr;
    delete _chartView_OPT;
    _chartView_OPT = nullptr;
    delete [] _chartView_IMU;
    _chartView_IMU = nullptr;
    delete _horizontalScrollBar_OPT;
    _horizontalScrollBar_OPT = nullptr;
    delete _verticalScrollBar_OPT;
    _verticalScrollBar_OPT = nullptr;
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

    // creating series
    this->_series_OPT = new QLineSeries[_numOfCH_OPT + 1]; // +1 for label
    this->_series_IMU = new QLineSeries[_numOfCH_IMU + 1]; // +1 for label

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
    
    if (this->_chartView_OPT != nullptr && this->_chartView_OPT->_zoomed == true)
        return ;
    
    bool flag = false;
    
    this->_valueLineMin_OPT = -1;
    this->_valueLineMax_OPT = 0;

    for (int i = 0; i < _numOfCH_OPT; i++)
	{
        if (_checkBoxChannelsValue[i] == false)
            continue ;
        flag = true;
        for(int j = 0; j < _series_OPT[i].count(); j++)
		{
            if (_series_OPT[i].at(j).x() >= _timeLineMin && _series_OPT[i].at(j).x() <= _timeLineMax_OPT)
            {
                _valueLineMax_OPT = std::max((unsigned)_series_OPT[i].at(j).y(), _valueLineMax_OPT);
                _valueLineMin_OPT = std::min((unsigned)_series_OPT[i].at(j).y(), _valueLineMin_OPT);
            }
		}
	}
    if (flag == false)
    {
        this->_valueLineMin_OPT = 0;
        this->_valueLineMax_OPT = 1;
    }
    _axisY_OPT->setRange(_valueLineMin_OPT, _valueLineMax_OPT);
    
    DEBUGGER();
}

void    WindowChart::execChartDialog(void)
{
    DEBUGGER();
    
    this->_chart_OPT = new QChart();
    
    int i = 0;
    for (; i < _filesCount; ++i)
        if (_filesList[i].isChecked() == true)
            break ;
    _chart_OPT->setTitle(this->staticChartTitle(_pathToFiles + _filesList[i].text()));
    
    QFont font;
    font.setBold(true);
    font.setPointSize(14);
    _chart_OPT->setTitleFont(font);
    _chart_OPT->setBackgroundBrush(QBrush(QColor::fromRgb(235, 255, 255)));
    _chart_OPT->legend()->hide();

    for (int i = 0; i < _numOfCH_OPT + 1; ++i) // +1 for series
	{
        _chart_OPT->addSeries(&_series_OPT[i]);
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

    this->_axisX_OPT = new QValueAxis();
    _axisX_OPT->setTitleText("Time (milliseconds)");
    _chart_OPT->addAxis(_axisX_OPT, Qt::AlignBottom);
    for (int i = 0; i < _numOfCH_OPT + 1; ++i) // +1 for label
        _series_OPT[i].attachAxis(_axisX_OPT);

    this->_axisX_OPT = new QValueAxis[_numOfCH_IMU / 3];
//    for (int i = 0; i < _numOfCH_IMU / 3; ++i)


    this->_axisY_OPT = new QValueAxis();
    _axisY_OPT->setTitleText("Values");
    _chart_OPT->addAxis(_axisY_OPT, Qt::AlignLeft);
    for (int i = 0; i < _numOfCH_OPT; ++i) // +1 for label
        _series_OPT[i].attachAxis(_axisY_OPT);

    this->_axisYLabel_OPT = new QValueAxis();
    _axisYLabel_OPT->setTitleText("Label");
    _chart_OPT->addAxis(_axisYLabel_OPT, Qt::AlignRight);
    _series_OPT[_numOfCH_OPT].attachAxis(_axisYLabel_OPT);
    for (int i = 0; i < _series_OPT[_numOfCH_OPT].count(); ++i)
        _maxLabel_OPT = std::max((int)_series_OPT[_numOfCH_OPT].at(i).y(), _maxLabel_OPT);
    _axisYLabel_OPT->setRange(0, _maxLabel_OPT + 1);
	
    this->_checkBoxChannelsValue = new bool[_numOfCH_OPT + 1];
    for (int i = 0; i < _numOfCH_OPT + 1; ++i)
		this->_checkBoxChannelsValue[i] = true;
    
	this->updateValueLineAxis();
    _axisX_OPT->setRange(_timeLineMin, _timeLineMax_OPT);
    
    this->_horizontalScrollBar_OPT = new QScrollBar(Qt::Horizontal, this);
    this->_horizontalScrollBar_OPT->setRange(0, 0);
    connect(this->_horizontalScrollBar_OPT, &QScrollBar::valueChanged, this,
        [=](int value)
    	{
            DEBUGGER();
            this->_axisX_OPT->setRange(value, value + this->_chartView_OPT->_currentAxisXLength);
            DEBUGGER();
		});
    
    this->_verticalScrollBar_OPT = new QScrollBar(Qt::Vertical, this);
    this->_verticalScrollBar_OPT->setRange(0, 0);
    connect(this->_verticalScrollBar_OPT, &QScrollBar::valueChanged, this,
        [=](int value)
    	{
            DEBUGGER();
            this->_axisY_OPT->setRange(value, value + this->_chartView_OPT->_currentAxisYLength);
            DEBUGGER();
		});

    this->_chartView_OPT = new MyChartView(_chart_OPT, _timeLineMin, _timeLineMax_OPT, _valueLineMin_OPT, _valueLineMax_OPT, \
                                       _axisX_OPT, _axisY_OPT, _axisYLabel_OPT, _maxLabel_OPT, \
                                       _zoomToHomeButton, _horizontalScrollBar_OPT, _verticalScrollBar_OPT);
    this->_chartView_OPT->setRenderHint(QPainter::Antialiasing);
    this->_chartView_OPT->setRubberBand(QChartView::RectangleRubberBand);
    
    this->_horizontalScrollBar_OPT->setParent(this->_chartView_OPT);
    this->_verticalScrollBar_OPT->setParent(this->_chartView_OPT);
    this->_verticalScrollBar_OPT->setInvertedAppearance(true); // reverse the direction
    
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
    
    this->_gridLayout->addWidget(this->_chartView_OPT, 0, 0, 1, 5);
    this->_gridLayout->addWidget(this->_verticalScrollBar_OPT, 0, 0, 1, 5, Qt::AlignRight);
    this->_gridLayout->addWidget(this->_horizontalScrollBar_OPT, 0, 0, 1, 5, Qt::AlignBottom);
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
                _chart_OPT->addSeries(&_series_OPT[i]);
                _series_OPT[i].attachAxis(_axisX_OPT);
                if (i && i % _numOfCH_OPT == 0)
                    _series_OPT[i].attachAxis(_axisYLabel_OPT);
                else
                    _series_OPT[i].attachAxis(_axisY_OPT);
                this->_checkBoxChannelsValue[i] = true;
            }
            else
            {
                _chart_OPT->removeSeries(&_series_OPT[i]);
                this->_checkBoxChannelsValue[i] = false;
            }
            this->updateValueLineAxis();
            _chart_OPT->update();
        });
    
    DEBUGGER();
}
