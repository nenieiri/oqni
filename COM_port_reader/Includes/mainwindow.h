#ifndef MAINWINDOW_H
# define MAINWINDOW_H

# include "all.hpp"
# include "../ui_mainwindow.h"
# include "comport.hpp"

# define MY_DEFINED_DEFAULT_BUTTON  "QPushButton {border-radius: 6px; color: black; border: 1px solid gray;    background: #6FD5CA;} \
                               QPushButton:hover {border-radius: 6px; color: black; border: 1px solid #0078D4; background: #B9E8E2;}"
# define MY_DEFINED_RELEASED_BUTTON "QPushButton {border-radius: 6px; color: black; border: 1px solid #0078D4; background: #6FD5CA;} \
                               QPushButton:hover {border-radius: 6px; color: black; border: 1px solid #0078D4; background: #B9E8E2;}"
# define MY_DEFINED_PRESSED_BUTTON               "border-radius: 6px; color: blue;  border: 1px solid #0078D4; background: white;"

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
        void            putWindowOnScreen(int windowWidth, int windowHeight);
        QPushButton     *createButton(const QString &name, int x, int y, int width, int height, std::function<void(void)> action, QWidget *box);
        void            addLoadingAnimation(QPushButton *button, int x, int y, int width, int height);
        void            createGroupBox(int x, int y, int width, int height);
        void            createLiftVertical(int x, int y, int width, int height);
        void            buttonCheckAction(void);
        void            buttonNextAction(void);
        const QString   createFileName(const QString &portName);


    private:
        Ui::MainWindow      *ui;
        QPushButton         *_buttonCheck;
        QPushButton         *_buttonNext;
        QLabel              *_gifLabel;
        QMovie              *_gifMovie;
        QGroupBox           *_groupBox;
        QVector<ComPort *>  _comPorts;
        int                 _portCount;
        QScrollBar          *_liftVertical;
        
        QStringList         _baudRateItems;
        QStringList         _dataBitsItems;
        QStringList         _parityItems;
        QStringList         _stopBitsItems;
        QStringList         _flowControlItems;
};

#endif
