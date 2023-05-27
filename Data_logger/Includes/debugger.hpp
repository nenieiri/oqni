#ifndef DEBUGGER_HPP
# define DEBUGGER_HPP

# define DEBUGGER() errorLogger(__FILE__, __LINE__, __func__)

# include <QDebug>

void errorLogger(const char* fileName, int lineNumber, const char* functionName);

#endif
