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

    this->_numOfChart_OPT = 1;
    this->_numOfChart_IMU = 3;

    this->_chart_OPT = nullptr;
    this->_chart_IMU = nullptr;
    this->_chartView_OPT = nullptr;
    for (int i = 0; i < _numOfChart_IMU; ++i)
        this->_chartView_IMU[i] = nullptr;
    this->_axisX_OPT = nullptr;
    this->_axisX_IMU = nullptr;
    this->_axisY_OPT = nullptr;
    this->_axisY_IMU = nullptr;
    this->_axisYLabel_OPT = nullptr;
    this->_axisYLabel_IMU = nullptr;
    this->_series_OPT = nullptr;
    this->_series_IMU = nullptr;
    this->_checkBoxSensors = nullptr;
    this->_numOfSeries_OPT = 0; // initial value
    this->_numOfSeries_IMU = 0; // initial value
    this->_maxLabel_OPT = 0;
    this->_maxLabel_IMU = 0;
    this->_timeLineMin = 0;
    this->_timeLineMax_OPT = 0;
    this->_timeLineMax_IMU = 0;

    this->_snapshotButton = new QPushButton;
    this->_snapshotButton->setEnabled(true);
    QPixmap SnapshotPixmap(":/Imgs/iconSnapshot.png");
    this->_iconCamera = new QIcon(SnapshotPixmap);
    this->_snapshotButton->setIcon(*_iconCamera);
    this->_snapshotButton->setIconSize(SnapshotPixmap.size());
    this->_snapshotButton->setFixedSize(SnapshotPixmap.size());
    this->_snapshotButton->setStyleSheet("QPushButton { border: none; }");
    this->_snapshotButton->setMask(SnapshotPixmap.mask());
    this->_snapshotButton->setToolTip("Take a snapshot.");
    connect(this->_snapshotButton, &QPushButton::clicked, this,
        [=]()
        {
            qDebug() << "snapshot button needs functionality";
        });

    this->_zoomToHomeButton = new QPushButton;
    this->_zoomToHomeButton->setEnabled(false);
    QPixmap ZoomPixmap(":/Imgs/iconHome.png");
    this->_iconHome = new QIcon(ZoomPixmap);
    this->_zoomToHomeButton->setIcon(*_iconHome);
    this->_zoomToHomeButton->setIconSize(ZoomPixmap.size());
    this->_zoomToHomeButton->setFixedSize(ZoomPixmap.size());
    this->_zoomToHomeButton->setStyleSheet("QPushButton { border: none; }");
    this->_zoomToHomeButton->setMask(ZoomPixmap.mask());
    this->_zoomToHomeButton->setToolTip("Resize to original dimensions.");
    connect(this->_zoomToHomeButton, &QPushButton::clicked, this,
        [=]()
        {
            DEBUGGER();
            
            _axisX_OPT->setRange(_timeLineMin, _timeLineMax_OPT);
            _axisY_OPT->setRange(_valueLineMin_OPT, _valueLineMax_OPT);
            _axisYLabel_OPT->setRange(0, _maxLabel_OPT + 1);

            for (int i = 0; i < _numOfChart_IMU; ++i)
            {
                _axisX_IMU[i].setRange(_timeLineMin, _timeLineMax_IMU);
                _axisY_IMU[i].setRange(_valueLineMin_IMU[i], _valueLineMax_IMU[i]);
                _axisYLabel_IMU[i].setRange(0, _maxLabel_IMU + 1);
            }

            _chartView_OPT->_zoomed = false;
            _chartView_OPT->_currentAxisXLength = _timeLineMax_OPT - _timeLineMin;
            _chartView_OPT->_currentAxisYLength = _valueLineMax_OPT - _valueLineMin_OPT;
            for (int i = 0; i < _numOfChart_IMU; ++i)
            {
                _chartView_IMU[i]->_zoomed = false;
                _chartView_IMU[i]->_currentAxisXLength = _timeLineMax_IMU - _timeLineMin;
                _chartView_IMU[i]->_currentAxisYLength = _valueLineMax_IMU[i] - _valueLineMin_IMU[i];
            }

			_zoomToHomeButton->setEnabled(false);

            _horizontalScrollBar_OPT->setRange(_timeLineMin, _timeLineMin);
            _horizontalScrollBar_OPT->setValue(_timeLineMin);
            _verticalScrollBar_OPT->setRange(_valueLineMin_OPT, _valueLineMin_OPT);
            _verticalScrollBar_OPT->setValue(_valueLineMin_OPT);

            for (int i = 0; i < _numOfChart_IMU; ++i)
            {
                _horizontalScrollBar_IMU[i]->setRange(_timeLineMin, _timeLineMin);
                _horizontalScrollBar_IMU[i]->setValue(_timeLineMin);
                _verticalScrollBar_IMU[i]->setRange(_valueLineMin_IMU[i], _valueLineMin_IMU[i]);
                _verticalScrollBar_IMU[i]->setValue(_valueLineMin_IMU[i]);
            }
            
            DEBUGGER();
    	});
    
    this->setModal(true);
    this->setStyleSheet("background: #e6e6e6;");
    this->raise();
	this->show();
    
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

    delete _axisYLabel_OPT;
    _axisYLabel_OPT = nullptr;
    delete [] _axisYLabel_IMU;
    _axisYLabel_IMU = nullptr;

    for (int i = 0; i < _numOfSeries_OPT; ++i)
        if (_chart_OPT->series().contains(&_series_OPT[i]))
            _chart_OPT->removeSeries(&_series_OPT[i]);
    delete[] _series_OPT;
    _series_OPT = nullptr;
    for (int i = 0; i < _numOfSeries_IMU; ++i)
        _chart_IMU[i / 4].removeSeries(&_series_IMU[i]); // in each _chart_IMU[i] are 4 series (x, y, z and label)
    delete[] _series_IMU;
    _series_IMU = nullptr;

    delete _chart_OPT;
    _chart_OPT = nullptr;
    delete [] _chart_IMU;
    _chart_IMU = nullptr;

    delete [] _checkBoxSensors;
    this->_checkBoxSensors = nullptr;

    delete _horizontalScrollBar_OPT;
    _horizontalScrollBar_OPT = nullptr;
    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        delete _horizontalScrollBar_IMU[i];
        _horizontalScrollBar_IMU[i] = nullptr;
    }

    delete _verticalScrollBar_OPT;
    _verticalScrollBar_OPT = nullptr;
    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        delete _verticalScrollBar_IMU[i];
        _verticalScrollBar_IMU[i] = nullptr;
    }

    delete _chartView_OPT;
    _chartView_OPT = nullptr;
    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        delete _chartView_IMU[i];
        _chartView_IMU[i] = nullptr;
    }

	delete[] _checkBoxChannels;
    _checkBoxChannels = nullptr;
    delete _hBoxLayoutLegends;
    _hBoxLayoutLegends = nullptr;
    delete _hBoxLayoutOptions;
    _hBoxLayoutOptions = nullptr;
	delete _gridLayout;
    _gridLayout = nullptr;
	delete _zoomToHomeButton;
    _zoomToHomeButton = nullptr;    
    delete _iconHome;
    _iconHome = nullptr;
    delete _snapshotButton;
    _snapshotButton = nullptr;
    delete _iconCamera;
    _iconCamera = nullptr;
    
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
                _numOfSeries_IMU += ins[j].readLine().count("led") + 3; // counting number of IMU channels (+labels) and omitting first line in file
            else if (_filesList[i].text().mid(14,3) == "OPT")
                _numOfSeries_OPT += ins[j].readLine().count("led") + 1; // counting number of OPT channels (+labels) and omitting first line in file
        }
    }

    // creating series
    this->_series_OPT = new QLineSeries[_numOfSeries_OPT]; // labels in indexes 3 and 7
    this->_series_IMU = new QLineSeries[_numOfSeries_IMU]; // labels in indexes 3, 7 and 11

    // reading data from files to series
    DEBUGGER();
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
                    int l = -1; // for label series tracking
                    // loop over data, except label (label is the last element in splitList)
                    for (int k = 0; k < 9; ++k) // 9 - number of IMU 9 channals
                    {
                        if (k % _numOfChart_IMU == 0)
                            ++l;
                        // we want omit series at indexes 3, 7 and 11 for labels, so we add increase 'l' and add it to 'k'
                        _series_IMU[k + l].append(time, splitList[k + 1].toInt()); // k+1, because at index 0 is the time in millisec
                    }

                    // loop over label series at indexes 3, 7 and 11
                    for (int k = 3; k < 12; k += 4) // 12 - number of IMU series
                        _series_IMU[k].append(time, splitList[10].toUInt()); // label is the 10th element in splitList
                }
                else if (_filesList[i].text().mid(14,3) == "OPT")
                    if (_checkedFilesCount_OPT)
                        for (int k = 0; k < 4; ++k) // green, red, infrared and label for each OPT sensor
                            _series_OPT[k + (j - _checkedFilesCount_IMU) * _numOfSeries_OPT / _checkedFilesCount_OPT].append(time, splitList[k + 1].toUInt()); // k+1, because at index 0 is the time in millisec
            }
            files[j++].close();
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
    
    bool flag = false;
    if (!(this->_chartView_OPT != nullptr && this->_chartView_OPT->_zoomed == true))
    {
        this->_valueLineMin_OPT = -1;
        this->_valueLineMax_OPT = 0;

        for (int i = 0; i < _numOfSeries_OPT; i++)
        {
            DEBUGGER();
            if (i == 3 || i == 7) // skip label
                continue ;
            if (_checkBoxChannels[i].isChecked() == false)
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
    }

    // for IMU

    for (int k = 0; k < _numOfChart_IMU; ++k)
    {
        if (this->_chartView_IMU[k] != nullptr && this->_chartView_IMU[k]->_zoomed == true)
            continue ;
        this->_valueLineMin_IMU[k] = SHRT_MAX;
        this->_valueLineMax_IMU[k] = SHRT_MIN;
    }

    for (int i = 0; i < _numOfSeries_IMU; i++)
    {
        if (this->_chartView_IMU[i / 4] != nullptr && this->_chartView_IMU[i / 4]->_zoomed == true)
            continue ;
        if (i == 3 || i == 7 || i == 11) // skip label
            continue ;
        for(int j = 0; j < _series_IMU[i].count(); j++)
        {
            if (_series_IMU[i].at(j).x() >= _timeLineMin && _series_IMU[i].at(j).x() <= _timeLineMax_IMU)
            {
                _valueLineMax_IMU[i / 4] = std::max((short)_series_IMU[i].at(j).y(), _valueLineMax_IMU[i / 4]);
                _valueLineMin_IMU[i / 4] = std::min((short)_series_IMU[i].at(j).y(), _valueLineMin_IMU[i / 4]);
            }
        }
    }
    for (int i = 0; i < _numOfChart_IMU; i++)
    {
        if (this->_chartView_IMU[i] != nullptr && this->_chartView_IMU[i]->_zoomed == true)
            continue ;
        _axisY_IMU[i].setRange(_valueLineMin_IMU[i], _valueLineMax_IMU[i]);
    }
    
    DEBUGGER();
}

void    WindowChart::execChartDialog(void)
{
    DEBUGGER();

    this->_chart_OPT = new QChart();
    this->_chart_IMU = new QChart[_numOfChart_IMU];
    
    for (int i = 0; i < _numOfChart_IMU; ++i)
        _chart_IMU[i].setTitle(this->staticChartTitle(i));
    _chart_OPT->setTitle(this->staticChartTitle(_numOfChart_IMU));

    QFont font;
    font.setBold(true);
    font.setPointSize(12);
    _chart_OPT->setTitleFont(font);
    _chart_OPT->setBackgroundBrush(QBrush(QColor::fromRgb(235, 255, 255)));
    _chart_OPT->legend()->hide();
    DEBUGGER();

    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        _chart_IMU[i].setTitleFont(font);
        _chart_IMU[i].setBackgroundBrush(QBrush(QColor::fromRgb(255, 245, 255)));
        _chart_IMU[i].legend()->hide();
    }
    DEBUGGER();

    for (int i = 0; i < _numOfSeries_OPT; ++i)
	{
        _chart_OPT->addSeries(&_series_OPT[i]);
        if (_checkedFilesCount_OPT) {
            switch (i % (_numOfSeries_OPT / _checkedFilesCount_OPT)) {
            case 0:
                _series_OPT[i].setColor(Qt::green);
                break;
            case 1:
                _series_OPT[i].setColor(Qt::red);
                break;
            case 2:
                _series_OPT[i].setColor(Qt::blue); // infraRed
                break;
            case 3:
                _series_OPT[i].setColor(Qt::black); // label
            }
        }
    }
    DEBUGGER();

    for (int i = 0; i < _numOfSeries_IMU; ++i)
    {
        _chart_IMU[i / 4].addSeries(&_series_IMU[i]);
        switch (i % 4) {
        case 0:
            _series_IMU[i].setColor(Qt::red); // X
            break;
        case 1:
            _series_IMU[i].setColor(Qt::green); // Y
            break;
        case 2:
            _series_IMU[i].setColor(Qt::blue); // Z
            break;
        case 3:
            _series_IMU[i].setColor(Qt::black); // label
        }
    }
    DEBUGGER();

    // creating axis X for OPT sensors
    this->_axisX_OPT = new QValueAxis();
    _axisX_OPT->setTitleText("Time (milliseconds)");
    _chart_OPT->addAxis(_axisX_OPT, Qt::AlignBottom);
    for (int i = 0; i < _numOfSeries_OPT; ++i)
        _series_OPT[i].attachAxis(_axisX_OPT);
    DEBUGGER();

    // creating axis X for IMU sensors
    // for 3 charts: accelerometer, gyroscope, magnetometer
    this->_axisX_IMU = new QValueAxis[_numOfChart_IMU];
    for (int i = 0; i < _numOfChart_IMU; ++i)
        _chart_IMU[i].addAxis(&_axisX_IMU[i], Qt::AlignBottom);
    for (int i = 0; i < _numOfSeries_IMU; ++i)
        _series_IMU[i].attachAxis(&_axisX_IMU[i / 4]);
    DEBUGGER();

    // creating axis Y for OPT sensors and labels
    this->_axisY_OPT = new QValueAxis();
    this->_axisYLabel_OPT = new QValueAxis();
    _axisY_OPT->setTitleText("Values");
    _axisYLabel_OPT->setTitleText("Label");
    _chart_OPT->addAxis(_axisY_OPT, Qt::AlignLeft);
    _chart_OPT->addAxis(_axisYLabel_OPT, Qt::AlignRight);

    for (int i = 0; i < _numOfSeries_OPT; ++i)
    {
        if (_checkedFilesCount_OPT) {
            switch (i % (_numOfSeries_OPT / _checkedFilesCount_OPT)) {
            case 0:
            case 1:
            case 2:
                _series_OPT[i].attachAxis(_axisY_OPT);
                break;
            case 3: // series at indexes 3 and 7 are labels
                _series_OPT[i].attachAxis(_axisYLabel_OPT);
                for (int j = 0; j < _series_OPT[i].count(); ++j)
                    _maxLabel_OPT = std::max((int)_series_OPT[i].at(j).y(), _maxLabel_OPT);
                break;
            }
        }
    }
    _axisYLabel_OPT->setRange(0, _maxLabel_OPT + 1);
    DEBUGGER();

    // creating axis Y for IMU sensors and labels
    // for 3 charts: accelerometer, gyroscope, magnetometer
    this->_axisY_IMU = new QValueAxis[_numOfChart_IMU];
    this->_axisYLabel_IMU = new QValueAxis[_numOfChart_IMU];
    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        _axisY_IMU[i].setTitleText("Values");
        _axisYLabel_IMU[i].setTitleText("Label");
        _chart_IMU[i].addAxis(&_axisY_IMU[i], Qt::AlignLeft);
        _chart_IMU[i].addAxis(&_axisYLabel_IMU[i], Qt::AlignRight);
    }

    for (int i = 0; i < _numOfSeries_IMU; ++i)
    {
        switch (i % 4) {
        case 0:
        case 1:
        case 2:
            _series_IMU[i].attachAxis(&_axisY_IMU[i / 4]);
            break;
        case 3: // series at indexes 3, 7 and 11 are labels
            _series_IMU[i].attachAxis(&_axisYLabel_IMU[i / 4]);
            for (int j = 0; j < _series_IMU[i].count(); ++j)
                _maxLabel_IMU = std::max((int)_series_IMU[i].at(j).y(), _maxLabel_IMU);
        }
    }
    for (int i = 0; i < _numOfChart_IMU; ++i)
        _axisYLabel_IMU[i].setRange(0, _maxLabel_IMU + 1);
    DEBUGGER();
    
    this->_checkBoxChannels = new QCheckBox[_numOfSeries_OPT];
    for (int i = 0; i < _numOfSeries_OPT; ++i)
        this->_checkBoxChannels[i].setChecked(true);

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
    DEBUGGER();

    this->_verticalScrollBar_OPT = new QScrollBar(Qt::Vertical, this);
    this->_verticalScrollBar_OPT->setRange(0, 0);
    connect(this->_verticalScrollBar_OPT, &QScrollBar::valueChanged, this,
            [=](int value)
            {
                DEBUGGER();
                this->_axisY_OPT->setRange(value, value + this->_chartView_OPT->_currentAxisYLength);
                DEBUGGER();
            });

    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        this->_horizontalScrollBar_IMU[i] = new QScrollBar(Qt::Horizontal, this);
        this->_horizontalScrollBar_IMU[i]->setRange(0, 0);
        connect(_horizontalScrollBar_IMU[i], &QScrollBar::valueChanged, this,
                [=](int value)
                {
                    DEBUGGER();
                    this->_axisX_IMU[i].setRange(value, value + this->_chartView_IMU[i]->_currentAxisXLength);
                    DEBUGGER();
                });
        DEBUGGER();

        this->_verticalScrollBar_IMU[i] = new QScrollBar(Qt::Vertical, this);
        this->_verticalScrollBar_IMU[i]->setRange(0, 0);
        connect(_verticalScrollBar_IMU[i], &QScrollBar::valueChanged, this,
                [=](int value)
                {
                    DEBUGGER();
                    this->_axisY_IMU[i].setRange(value, value + this->_chartView_IMU[i]->_currentAxisYLength);
                    DEBUGGER();
                });
    }

    this->_chartView_OPT = new MyChartView(_chart_OPT, _timeLineMin, _timeLineMax_OPT, _valueLineMin_OPT, _valueLineMax_OPT, \
                                       _axisX_OPT, _axisY_OPT, _axisYLabel_OPT, _maxLabel_OPT, \
                                       _zoomToHomeButton, _horizontalScrollBar_OPT, _verticalScrollBar_OPT);
    this->_chartView_OPT->setRenderHint(QPainter::Antialiasing);
    this->_chartView_OPT->setRubberBand(QChartView::RectangleRubberBand);

    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        this->_chartView_IMU[i] = new MyChartView(&_chart_IMU[i], _timeLineMin, _timeLineMax_IMU, _valueLineMin_IMU[i], _valueLineMax_IMU[i], \
                                               &_axisX_IMU[i], &_axisY_IMU[i], &_axisYLabel_IMU[i], _maxLabel_IMU, \
                                               _zoomToHomeButton, _horizontalScrollBar_IMU[i], _verticalScrollBar_IMU[i]);
        this->_chartView_IMU[i]->setRenderHint(QPainter::Antialiasing);
        this->_chartView_IMU[i]->setRubberBand(QChartView::RectangleRubberBand);
    }

    // checking right click on chart views and enable/disable zooom button
    connect(_chartView_OPT, &MyChartView::rightClickAction, this,
    [=](void)
    {
        int zoomedCount{};
        for (int i = 0; i < _numOfChart_IMU; ++i)
            zoomedCount += _chartView_IMU[i]->_zoomed;
        zoomedCount += _chartView_OPT->_zoomed;
        _zoomToHomeButton->setEnabled(zoomedCount != 0);
    });
    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        connect(_chartView_IMU[i], &MyChartView::rightClickAction, this,
        [=](void)
        {
                int zoomedCount{};
            for (int i = 0; i < _numOfChart_IMU; ++i)
                zoomedCount += _chartView_IMU[i]->_zoomed;
            zoomedCount += _chartView_OPT->_zoomed;
            _zoomToHomeButton->setEnabled(zoomedCount != 0);
        });
    }
    DEBUGGER();
    
    this->_horizontalScrollBar_OPT->setParent(this->_chartView_OPT);
    this->_verticalScrollBar_OPT->setParent(this->_chartView_OPT);
    this->_verticalScrollBar_OPT->setInvertedAppearance(true); // reverse the direction

    for (int i = 0; i < _numOfChart_IMU; ++i)
    {
        this->_horizontalScrollBar_IMU[i]->setParent(this->_chartView_IMU[i]);
        this->_verticalScrollBar_IMU[i]->setParent(this->_chartView_IMU[i]);
        this->_verticalScrollBar_IMU[i]->setInvertedAppearance(true); // reverse the direction
    }

    this->_gridLayout = new QGridLayout;
    this->_hBoxLayoutLegends = new QHBoxLayout;
    this->_hBoxLayoutOptions = new QHBoxLayout;

    for (int k = 0, j = -1; k < _filesCount; ++k)
    {
        if (_filesList[k].isChecked() == false || _filesList[k].text().mid(14,3) == "IMU")
            continue ;
        ++j;        
        for (int i = 0; i < _numOfSeries_OPT / _checkedFilesCount_OPT; ++i) // (_numOfSeries_OPT / _checkedFilesCount_OPT) ==> number of checkboxes in one block
        {
            int index = i + j * (_numOfSeries_OPT / _checkedFilesCount_OPT);
            switch (i){
            case 0:
                this->_checkBoxChannels[index].setText("OPT" + QString::number(k) + "green  ");
                this->_checkBoxChannels[index].setStyleSheet("color: green; font-size: 14px;");
                break;
            case 1:
                this->_checkBoxChannels[index].setText("OPT" + QString::number(k) + "red  ");
                this->_checkBoxChannels[index].setStyleSheet("color: red; font-size: 14px;");
                break;
            case 2:
                this->_checkBoxChannels[index].setText("OPT" + QString::number(k) + "infrared  ");
                this->_checkBoxChannels[index].setStyleSheet("color: blue; font-size: 14px;");
                break;
            case 3:
                this->_checkBoxChannels[index].setText("Label" + QString::number(k) + "      ");
                this->_checkBoxChannels[index].setStyleSheet("color: black; font-size: 14px;");
                break;
            }            
            this->_checkBoxChannels[index].setChecked(true);
            this->connectStaticChatCheckBox(index);
            this->_hBoxLayoutLegends->addWidget(&_checkBoxChannels[index]);
        }
    }

    int chartsCount = _numOfChart_IMU + _numOfChart_OPT;
    this->_checkBoxSensors = new QCheckBox[chartsCount];
    QStringList title = {"Accelerometer  ", "Gyroscope  ", "Magnetometer  ", "OPT sensores"};
    for (int i = 0; i < chartsCount; ++i)
    {
        this->_checkBoxSensors[i].setText(title[i]);
        this->_checkBoxSensors[i].setChecked(true);
        this->_checkBoxSensors[i].setStyleSheet("color: black; font-size: 14px;");
        if (_checkedFilesCount_OPT == 0 && i == 3)
        {
            this->_checkBoxSensors[i].setChecked(false);
            continue ;
        }
        this->_hBoxLayoutOptions->addWidget(&_checkBoxSensors[i]);
    }
    
    // initial appearance of WindowChart
    if (_checkedFilesCount_IMU == 0)
    {
        this->_gridLayout->addWidget(_chartView_OPT, 0, 0, 1, 5);
        this->_gridLayout->addWidget(_verticalScrollBar_OPT, 0, 0, 1, 5, Qt::AlignRight);
        this->_gridLayout->addWidget(_horizontalScrollBar_OPT, 0, 0, 1, 5, Qt::AlignBottom);
        this->_gridLayout->addLayout(_hBoxLayoutLegends, 1, 0, 1, 3, Qt::AlignCenter);
        this->_gridLayout->addWidget(_snapshotButton, 1, 3, 1, 1, Qt::AlignVCenter);
        this->_gridLayout->addWidget(_zoomToHomeButton, 1, 4, 1, 1, Qt::AlignVCenter);
    }
    else if (_checkedFilesCount_OPT == 0)
    {
        int i;
        for (i = 0; i < _numOfChart_IMU; ++i)
        {
            this->_gridLayout->addWidget(_chartView_IMU[i], i, 0, 1, 5);
            this->_gridLayout->addWidget(_verticalScrollBar_IMU[i], i, 0, 1, 5, Qt::AlignRight);
            this->_gridLayout->addWidget(_horizontalScrollBar_IMU[i], i, 0, 1, 5, Qt::AlignBottom);
            this->_gridLayout->setRowStretch(i, true);
        }
        this->_gridLayout->addLayout(_hBoxLayoutOptions, i, 0, 1, 3, Qt::AlignCenter);
        this->_gridLayout->addWidget(_snapshotButton, i, 3, 1, 1, Qt::AlignVCenter);
        this->_gridLayout->addWidget(_zoomToHomeButton, i, 4, 1, 1, Qt::AlignVCenter);
    }
    else
    {
        int i;
        for (i = 0; i < _numOfChart_IMU; ++i)
        {
            this->_gridLayout->addWidget(_chartView_IMU[i], i, 0, 1, 5);
            this->_gridLayout->addWidget(_verticalScrollBar_IMU[i], i, 0, 1, 5, Qt::AlignRight);
            this->_gridLayout->addWidget(_horizontalScrollBar_IMU[i], i, 0, 1, 5, Qt::AlignBottom);
            this->_gridLayout->setRowStretch(i, true);
        }
        this->_gridLayout->addWidget(_chartView_OPT, i, 0, 1, 5);
        this->_gridLayout->addWidget(_verticalScrollBar_OPT, i, 0, 1, 5, Qt::AlignRight);
        this->_gridLayout->addWidget(_horizontalScrollBar_OPT, i, 0, 1, 5, Qt::AlignBottom);
        this->_gridLayout->setRowStretch(i, true);
        this->_gridLayout->addLayout(_hBoxLayoutLegends, i + 1, 0, 1, 4, Qt::AlignCenter);
        this->_gridLayout->addLayout(_hBoxLayoutOptions, i + 2, 0, 1, 3, Qt::AlignCenter);
        this->_gridLayout->addWidget(_snapshotButton, i + 2, 3, 1, 1, Qt::AlignVCenter);
        this->_gridLayout->addWidget(_zoomToHomeButton, i + 2, 4, 1, 1, Qt::AlignVCenter);
    }
    this->setLayout(_gridLayout);

    for (int i = 0; i < chartsCount; ++i)
    {
        connect(&this->_checkBoxSensors[i], &QCheckBox::clicked, this,
        [=]()
        {
            DEBUGGER();

            // we calculate how many boxes are checked
            int checkedSum = 0;
            for (int j = 0; j < chartsCount; ++j)
                checkedSum += _checkBoxSensors[j].isChecked();

            // if only one box is checked, we disable that box
            // so at least one box must be checked
            for (int j = 0; j < chartsCount; ++j)
                _checkBoxSensors[j].setEnabled(checkedSum != 1 || !_checkBoxSensors[j].isChecked());

            // fist we removed all items from the _gridLayout to add in new order
            for (int j = 0; j < _numOfChart_IMU; ++j)
            {
                _gridLayout->removeWidget(_chartView_IMU[j]);
                _chartView_IMU[j]->hide();
                _gridLayout->removeWidget(_verticalScrollBar_IMU[j]);
                _verticalScrollBar_IMU[j]->hide();
                _gridLayout->removeWidget(_horizontalScrollBar_IMU[j]);
                _horizontalScrollBar_IMU[j]->hide();
            }
            _gridLayout->removeWidget(_chartView_OPT);
            _chartView_OPT->hide();
            _gridLayout->removeWidget(_verticalScrollBar_OPT);
            _verticalScrollBar_OPT->hide();
            _gridLayout->removeWidget(_horizontalScrollBar_OPT);
            _horizontalScrollBar_OPT->hide();

            _gridLayout->removeItem(_hBoxLayoutLegends);
            _gridLayout->removeItem(_hBoxLayoutOptions);
            _gridLayout->removeWidget(_snapshotButton);
            _gridLayout->removeWidget(_zoomToHomeButton);
            _gridLayout->update();

            // than we add items to the _gridLayout in new order
            int offset = 0;
            for (int j = 0; j < _numOfChart_IMU; ++j)
            {
                if (_checkBoxSensors[j].isChecked())
                {
                    _gridLayout->addWidget(_chartView_IMU[j], offset, 0, 1, 5);
                    _gridLayout->addWidget(_verticalScrollBar_IMU[j], offset, 0, 1, 5, Qt::AlignRight);
                    _gridLayout->addWidget(_horizontalScrollBar_IMU[j], offset, 0, 1, 5, Qt::AlignBottom);
                    _gridLayout->setRowStretch(offset, true); // set the stretch factor for the rows
                    _chartView_IMU[j]->show();
                    _horizontalScrollBar_IMU[j]->show();
                    _verticalScrollBar_IMU[j]->show();
                    ++offset;
                }
            }
            if (_checkBoxSensors[_numOfChart_IMU].isChecked())
            {
                _gridLayout->addWidget(_chartView_OPT, offset, 0, 1, 5);
                _gridLayout->addWidget(_verticalScrollBar_OPT, offset, 0, 1, 5, Qt::AlignRight);
                _gridLayout->addWidget(_horizontalScrollBar_OPT, offset, 0, 1, 5, Qt::AlignBottom);
                _gridLayout->setRowStretch(offset, true); // set the stretch factor for the rows
                _chartView_OPT->show();
                _horizontalScrollBar_OPT->show();
                _verticalScrollBar_OPT->show();
                ++offset;
            }
            _gridLayout->addLayout(_hBoxLayoutLegends, offset, 0, 1, 4, Qt::AlignCenter);
            _gridLayout->setRowStretch(offset, false); // UNset the stretch factor for the rows
            _gridLayout->addLayout(_hBoxLayoutOptions, ++offset, 0, 1, 3, Qt::AlignCenter); // increasing offset (i.e. go to the next ров)
            _gridLayout->addWidget(_snapshotButton, offset, 3, 1, 1, Qt::AlignVCenter);
            _gridLayout->addWidget(_zoomToHomeButton, offset, 4, 1, 1, Qt::AlignVCenter);
            _gridLayout->setRowStretch(offset, false); // UNset the stretch factor for the rows

            // if OPT _checkBoxSensors is checked/unchecked enable/disable all _checkBoxChannels
            QStringList format = {"color: green; font-size: 14px;", "color: red; font-size: 14px;", "color: blue; font-size: 14px;", "color: black; font-size: 14px;"};
            if (_checkBoxSensors[chartsCount - 1].isChecked())
            {
                for (int j = 0; j < _numOfSeries_OPT; ++j)
                    _checkBoxChannels[j].setEnabled(true),
                        _checkBoxChannels[j].setStyleSheet(format[j % 4]);
            }
            else
            {
                for (int j = 0; j < _numOfSeries_OPT; ++j)
                    _checkBoxChannels[j].setEnabled(false),
                        _checkBoxChannels[j].setStyleSheet("color: gray; font-size: 14px;");
            }

            _gridLayout->update();

            DEBUGGER();
        });
    }
    DEBUGGER();
}

QString WindowChart::staticChartTitle(int index)
{
    DEBUGGER();

    const QString &selectedFile = _pathToFiles + _filesList[0].text();
    
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
            if (_filesList[i].text().mid(14,3) == "IMU" && index < 3)
            {
                QStringList names = {"<b>\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0Accelerometer:</b> \u00A0\u00A0 red-<b>X</b> \u00A0\u00A0 green-<b>Y</b> \u00A0\u00A0 blue-<b>Z</b>", \
                                     "<b>\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0Gyroscope:</b> \u00A0\u00A0 red-<b>X</b> \u00A0\u00A0 green-<b>Y</b> \u00A0\u00A0 blue-<b>Z</b>", \
                                     "<b>\u00A0\u00A0\u00A0\u00A0\u00A0\u00A0Magnetometer:</b> \u00A0\u00A0 red-<b>X</b> \u00A0\u00A0 green-<b>Y</b> \u00A0\u00A0 blue-<b>Z</b>"};
                title += names[index];
            }
            else if (_filesList[i].text().mid(14,3) == "OPT" && index >= 3)
            {
                title += _filesList[i].text().mid(lastUnderscoreLine_tmp + 1, lastDot_tmp - lastUnderscoreLine_tmp - 1);
                title += "\u00A0\u00A0\u00A0";
            }
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
                if (i % (_numOfSeries_OPT / _checkedFilesCount_OPT) == 3) // if (i % 4 == 3), it means case of label
                    _series_OPT[i].attachAxis(_axisYLabel_OPT);
                else
                    _series_OPT[i].attachAxis(_axisY_OPT);
            }
            else
                _chart_OPT->removeSeries(&_series_OPT[i]);
            this->updateValueLineAxis();
            _chart_OPT->update();
        });
    
    DEBUGGER();
}
