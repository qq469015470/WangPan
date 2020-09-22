#include "mainwindow.h"
#include "loginwindow.h"

#include <QApplication>

class WangPan
{
private:
	MainWindow mainwindow;
	LoginWindow loginwindow;

public:
	WangPan():
		mainwindow(),
		loginwindow()
	{

	}

	MainWindow& GetMainWindow()
	{
		return this->mainwindow;
	}

	LoginWindow& GetLoginWindow()
	{
		return this->loginwindow;
	}
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WangPan wangpan;
	LoginWindow& w = wangpan.GetLoginWindow();
	w.show();
	return a.exec();
}
