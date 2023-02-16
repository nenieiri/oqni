#ifndef MAINWINDOW_H
# define MAINWINDOW_H

# include <QApplication>
# include <QMainWindow>
# include <QScreen>
# include <QPushButton>
# include <QLabel>
# include <QMovie>
# include <QTimer>
# include <QGroupBox>
# include <QCheckBox>
# include <QVector>
# include <QDebug>
# include <QScrollBar>
# include "../ui_mainwindow.h"
# include "comport.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        
    public:
        void                createCheckBox();
        
    private:
        Ui::MainWindow      *ui;
        QPushButton         *_buttonCheck;
        QLabel              *_gifLabel;
        QMovie              *_gifMovie;
        QGroupBox           *_groupBox;
        QVector<ComPort *>  _comPorts;
        int                 _portCount;
        QScrollBar          *_liftVertical;
        int                 _liftRatio;
};

#endif // MAINWINDOW_H
