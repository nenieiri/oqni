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
# include <QSerialPortInfo>

# include <functional>

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
        void                putWindowOnScreen(int windowWidth, int windowHeight);
        QPushButton         *createButton(const QString &name, int x, int y, int width, int height, std::function<void(void)> action);
        void                addLoadingAnimation(QPushButton *button, int x, int y, int width, int height);
        void                updateCheckbox(void);
        void                createGroupBox();

    private:
        Ui::MainWindow      *ui;
        QPushButton         *_buttonCheck;
        QPushButton         *_buttonStart;
        QLabel              *_gifLabel;
        QMovie              *_gifMovie;
        QGroupBox           *_groupBox;
        QVector<ComPort *>  _comPorts;
        int                 _portCount;
        QScrollBar          *_liftVertical;
};

#endif
