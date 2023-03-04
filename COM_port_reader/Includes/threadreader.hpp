#ifndef THREADREADER_HPP
# define THREADREADER_HPP

# include <QObject>
# include <QThread>
# include <QMessageBox>

# include <iostream>
# include <string>
# include <fstream>
# include <cstdlib>

# include "comport.hpp"
# include "threaddisplaytimer.hpp"

class ThreadReader : public QThread
{
	Q_OBJECT
    
	public:
		ThreadReader(ComPort *comPort, const QString &selectedDirectory, ThreadDisplayTimer *threadDisplayTimer);
		~ThreadReader();

    public:
		void		run() override;
        
    private:
		void		reader(const ComPort *comPort, const std::string &pathFileName);
		void		parserUno(std::string &line, const std::string &pathFileName);
        void    stopAndClosePort(QSerialPort &port);
        int     requestPortConfig(QSerialPort &port);
    
    private:
		ComPort             *_comPort;
        std::string         _fileName;
        ThreadDisplayTimer *_threadDisplayTimer;
};

#endif
