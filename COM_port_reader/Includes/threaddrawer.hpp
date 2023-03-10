#ifndef THREADDRAWER_HPP
# define THREADDRAWER_HPP

# include <QThread>
# include <QObject>

class ThreadDrawer : public QThread
{
	Q_OBJECT
    
	public:
		ThreadDrawer(QObject *parent = nullptr);
        ~ThreadDrawer();
};

#endif
