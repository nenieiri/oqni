#ifndef ALL_HPP
# define ALL_HPP

# include <QApplication>
# include <QMainWindow>
# include <QScreen>
# include <QPushButton>
# include <QComboBox>
# include <QLabel>
# include <QMovie>
# include <QTimer>
# include <QGroupBox>
# include <QCheckBox>
# include <QVector>
# include <QDebug>
# include <QScrollBar>
# include <QDialog>
# include <QSerialPortInfo>
# include <QSerialPort>
# include <QWidget>
# include <QDateTime>
# include <QFileDialog>
# include <QDir>
# include <QString>
# include <QStringList>
# include <QRadioButton>

# include <functional>
# include <iostream>
# include <fstream>
# include <string>

# include "comport.hpp"

void reader_win(const ComPort *comPort, const std::string &pathFileName);

#endif
