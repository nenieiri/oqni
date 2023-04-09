#ifndef THREADRUNER_HPP
# define THREADRUNER_HPP

# include <QObject>
# include <QThread>
# include <QDebug>

# include <string>

# include "comport.hpp"
# include "all.hpp"

class ThreadRuner : public QThread
{
	Q_OBJECT
    
	public:
		ThreadRuner(ComPort *comPort, const std::string &fileName);
		~ThreadRuner();

    public:
		void		run() override;
        
    private:
		void		reader(const ComPort *comPort, const std::string &pathFileName);
		void		parserUno(std::string &line, const std::string &pathFileName);
    
    private:
		ComPort				*_comPort;
        const std::string	_fileName;
};

#endif // THREADRUNER_HPP
