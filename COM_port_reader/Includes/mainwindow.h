#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QScreen>
#include <QPushButton>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QGroupBox>
#include <QVector>
#include "../ui_mainwindow.h"
#include "comport.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        
    private:
        Ui::MainWindow      *ui;
        QPushButton         *_buttonCheck;
        QLabel              *_gifLabel;
        QMovie              *_gifMovie;
        QGroupBox           *_groupBox;
        QVector<ComPort *>   _comPorts;
};

#endif // MAINWINDOW_H
