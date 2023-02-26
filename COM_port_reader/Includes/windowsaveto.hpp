#ifndef WINDOWSAVETO_HPP
# define WINDOWSAVETO_HPP

# include <QDialog>
# include <QWidget>
# include <QPushButton>
# include <QLineEdit>
# include <QLabel>
# include <QScreen>
# include <QApplication>

# include "threadreader.hpp"
# include "threaddisplaytimer.hpp"
# include "mainwindow.h"

class MainWindow;

class WindowSaveTo : public QDialog
{
	public:
		WindowSaveTo(MainWindow *parent);
		~WindowSaveTo();
        
    public:
        void				setButtonStart(QPushButton *buttonStart);
        void				setButtonStop(QPushButton *buttonStop);
        void				setButtonClose(QPushButton *buttonClose);
        
        QLineEdit			*getLineEdit() const;
        QLabel				*getFinishMsgLabel() const;
        QPushButton			*getButtonStart() const;
        QPushButton			*getButtonStop() const;
        QPushButton			*getButtonClose() const;
        QLabel				*getShowReadingPort1() const;
        QLabel				*getShowReadingPort2() const;
        QLabel				*getShowSelectedDir1() const;
        QLabel				*getShowSelectedDir2() const;
        QLabel				*getTimer1() const;
        QLabel				*getTimer2() const;
        
    private:
		void				onThreadDisplayTimerFinished(void);
		void				setParametersDesign(void);
        
    private:
        QPushButton			*_buttonStart;
        QPushButton			*_buttonStop;
        QPushButton			*_buttonClose;

        QLineEdit			*_lineEdit;
        QLabel				*_finishMsgLabel;

		QLabel				*_showReadingPort1;
		QLabel				*_showReadingPort2;
		QLabel				*_showSelectedDir1;
		QLabel				*_showSelectedDir2;
		QLabel				*_timer1;
		QLabel				*_timer2;

        ComPort				*_selectedComPort;
        
        ThreadDisplayTimer  *_threadDisplayTimer;
        ThreadReader        *_threadReader;
		int					_durationTimerValue;
};

#endif
