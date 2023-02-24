#ifndef THREADREADER_HPP
# define THREADREADER_HPP

# include <QObject>
# include <QThread>
# include <QDebug>

# include <string>
# include <cstdlib>

# include "comport.hpp"
# include "all.hpp"

class ThreadDisplayTimer;

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
    
    private:
		ComPort             *_comPort;
        std::string         _fileName;
        ThreadDisplayTimer *_threadDisplayTimer;
};

#endif //
