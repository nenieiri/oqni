#include "../Includes/threaddrawer.hpp"

ThreadDrawer::ThreadDrawer(QWidget *parent)
    : QThread{parent}
{
    this->_chartDialog = new QDialog(parent);
//    this->_chartDialog->setParent(parent);
/*    this->setGeometry((screenWidth - windowWidth) / 2 - 300, \
    						(screenHeight - windowHeight) / 2 - 200, \
                    		windowWidth, windowHeight);
*/  
    this->_chartDialog->setWindowTitle("OQNI: Chart");
    this->_chartDialog->setWindowIcon(QIcon(":/Imgs/oqni.ico"));
    this->_chartDialog->setWindowFilePath(":/Imgs/oqni.ico");
    this->_chartDialog->setStyleSheet("background: #e6e6e6;");
    
    connect(this->_chartDialog, &QDialog::rejected, this, 
        [=]()
        {
            this->_chartDialog = nullptr;
            emit chartDialogIsRejected();
        });
}

ThreadDrawer::~ThreadDrawer()
{
    delete this->_chartDialog;
}

QDialog	*ThreadDrawer::getChartDialog()
{
    return (this->_chartDialog);
}

void	ThreadDrawer::run()
{
    emit    chartDialogReadyToStart();
}


//			QChart *chart = new QChart();
//			chart->setTitle("Dynamic Line Chart");
//			chart->legend()->hide();
		
//			QLineSeries *series = new QLineSeries();
//			series->setColor(Qt::red);
//			chart->addSeries(series);
		
//			QValueAxis *axisX = new QValueAxis();
//			axisX->setTitleText("X Axis");
//			axisX->setRange(0, 10000);
//			chart->addAxis(axisX, Qt::AlignBottom);
//			series->attachAxis(axisX);
		
//			QValueAxis *axisY = new QValueAxis();
//			axisY->setTitleText("Y Axis");
//			axisY->setRange(-2500000000, 2500000000);
//			chart->addAxis(axisY, Qt::AlignLeft);
//			series->attachAxis(axisY);
            
//            connect(_threadReader, &ThreadReader::lastRowOfData, dialog,
//            	[=](QByteArray data){
//					dialog->raise();
//					dialog->show();
//                	QStringList data = _threadReader->_data[0][_threadReader->_data[0].size() - 1].split(",");
//                    if (series->count() > 90)
//                        series->remove(0);
//                	series->append(data[0].toInt(), data[1].toInt());
//            	});
		
//			QChartView *chartView = new QChartView(chart);
//			chartView->setRenderHint(QPainter::Antialiasing);
		
//			dialog->setLayout(new QVBoxLayout);
//			dialog->layout()->addWidget(chartView);
//			dialog->resize(1000, 6000);
