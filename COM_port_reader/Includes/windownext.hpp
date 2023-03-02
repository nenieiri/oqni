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
        
    private:
		void				setParametersDesign(void);
        void				createDirectory(const QString &path);
        
    private slots:
		void				onThreadDisplayTimerFinished(void);
        
    private:
        QPushButton			*_buttonStart;
        QPushButton			*_buttonStop;
        QPushButton			*_buttonClose;
		QPushButton			*_buttonBrowse;

		QLabel				*_showReadingPort1;
		QLabel				*_showReadingPort2;
        QString				_selectedDirectory;
        
		QLabel				*_showSelectedDir1;
		QLineEdit			*_showSelectedDir2;
        
		QLabel				*_recordingFolder1;
		QLineEdit			*_recordingFolder2;
		QLineEdit			*_recordingFolder3;
		QLineEdit			*_recordingFolder4;
		QLineEdit			*_recordingFolder5;
        
		QLabel				*_placement1;
		QLabel				*_placement3;
        
		QLabel				*_protocol1;
		QLabel				*_protocol3;
        
		QLabel				*_timer1;
        QLineEdit			*_lineEdit;
        QLabel				*_finishMsgLabel;

        ComPort				*_selectedComPort;
        
        ThreadDisplayTimer  *_threadDisplayTimer;
        ThreadReader        *_threadReader;
		int					_durationTimerValue;
};

#endif
