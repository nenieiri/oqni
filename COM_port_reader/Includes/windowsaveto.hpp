#ifndef WINDOWSAVETO_HPP
# define WINDOWSAVETO_HPP

# include <QDialog>
# include <QWidget>
# include <QPushButton>
# include <QLineEdit>
# include <QLabel>

class WindowSaveTo : public QDialog
{
	public:
		WindowSaveTo(QWidget *parent = nullptr);
		~WindowSaveTo();
        
    public:
        void				setButtonStart(const QPushButton *buttonStart);
        void				setButtonStop(const QPushButton *buttonStop);
        void				setButtonClose(const QPushButton *buttonClose);
        
        QLineEdit			*getLineEdit() const;
        QLabel				*getFinishMsgLabel() const;
        
        const QPushButton	*getButtonStart() const;
        const QPushButton	*getButtonStop() const;
        const QPushButton	*getButtonClose() const;
        QLabel				*getShowReadingPort1() const;
        QLabel				*getShowReadingPort2() const;
        QLabel				*getShowSelectedDir1() const;
        QLabel				*getShowSelectedDir2() const;
        QLabel				*getTimer1() const;
        QLabel				*getTimer2() const;
        
    private:
        const QPushButton	*_buttonStart;
        const QPushButton	*_buttonStop;
        const QPushButton	*_buttonClose;

        QLineEdit			*_lineEdit;
        QLabel				*_finishMsgLabel;

		QLabel				*_showReadingPort1;
		QLabel				*_showReadingPort2;
		QLabel				*_showSelectedDir1;
		QLabel				*_showSelectedDir2;
		QLabel				*_timer1;
		QLabel				*_timer2;
};

#endif
