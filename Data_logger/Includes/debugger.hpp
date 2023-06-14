#ifndef DEBUGGER_HPP
# define DEBUGGER_HPP

# include <QDebug>

# define DEBUGGER() errorLogger(__FILE__, __LINE__, __func__)

void errorLogger(const char* fileName, int lineNumber, const char* functionName);

#endif
