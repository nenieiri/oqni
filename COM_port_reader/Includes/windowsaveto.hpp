#ifndef WINDOWSAVETO_HPP
# define WINDOWSAVETO_HPP

# include <QDialog>
# include <QWidget>

class WindowSaveTo : public QDialog
{
	public:
		WindowSaveTo(QWidget *parent = nullptr);
		~WindowSaveTo();
};

#endif // WINDOWSAVETO_HPP
