#ifndef WINDOWNEXT_HPP
# define WINDOWNEXT_HPP

# include <QDialog>
# include <QWidget>
# include <QPushButton>
# include <QLineEdit>
# include <QLabel>
# include <QString>
# include <QScreen>
# include <QApplication>
# include <QCoreApplication>

# include "threadreader.hpp"
# include "threaddisplaytimer.hpp"
# include "mainwindow.h"

class MainWindow;

class WindowNext : public QDialog
{
	public:
		WindowNext(MainWindow *parent);
		~WindowNext();
        
    public:
        void				setButtonStart(QPushButton *buttonStart);
        void				setButtonStop(QPushButton *buttonStop);
        void				setButtonClose(QPushButton *buttonClose);
        void				setButtonBrowse(QPushButton *buttonBrowse);
        
        QLineEdit			*getLineEdit() const;
        QLabel				*getFinishMsgLabel() const;
        QPushButton			*getButtonStart() const;
        QPushButton			*getButtonStop() const;
        QPushButton			*getButtonClose() const;
        QLabel				*getShowReadingPort2() const;
        
    private:
		void				setParametersDesign(void);
        void                createDirectory(const QString &path);
        
    private slots:
		void            		onThreadDisplayTimerFinished(void);
        
    private:
        QPushButton			*_buttonStart;
        QPushButton			*_buttonStop;
        QPushButton			*_buttonClose;
		QPushButton			*_buttonBrowse;

        QLineEdit			*_lineEdit;
        QLabel				*_finishMsgLabel;

		QLabel				*_showReadingPort1;
		QLabel				*_showReadingPort2;
        QString				_selectedDirectory;
        
		QLabel				*_showSelectedDir1;
		QLineEdit			*_showSelectedDir2;
        
		QLabel				*_timer1;
		QLabel				*_timer2;

        ComPort				*_selectedComPort;
        
        ThreadDisplayTimer  *_threadDisplayTimer;
        ThreadReader        *_threadReader;
		int					_durationTimerValue;
};

#endif
