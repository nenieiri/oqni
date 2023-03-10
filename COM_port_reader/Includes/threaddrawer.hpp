#ifndef THREADDRAWER_HPP
# define THREADDRAWER_HPP

# include <QThread>
# include <QObject>
# include <QDialog>

class ThreadDrawer : public QThread
{
	Q_OBJECT
    
	public:
		ThreadDrawer(QWidget *parent = nullptr);
        ~ThreadDrawer();

    public:
		void		run() override;
        QDialog		*getChartDialog();

    signals:
        void		chartDialogIsRejected();
        void        chartDialogReadyToStart();
        
    private:
    	QDialog     *_chartDialog;
};

#endif
