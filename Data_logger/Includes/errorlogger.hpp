#ifndef ERRORLOGGER_HPP
# define ERRORLOGGER_HPP

# define ERROR_LOGGER() errorLogger(__FILE__, __LINE__, __func__)

# include <QDebug>

void errorLogger(const char* fileName, int lineNumber, const char* functionName);

#endif
