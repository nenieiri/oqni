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
        
        const QPushButton	*getButtonStart() const;
        const QPushButton	*getButtonStop() const;
        const QPushButton	*getButtonClose() const;
        
    private:
        const QPushButton	*_buttonStart;
        const QPushButton	*_buttonStop;
        const QPushButton	*_buttonClose;
        QLineEdit			*_lineEdit;
        QLabel              *_finishMsgLabel;
};

#endif
