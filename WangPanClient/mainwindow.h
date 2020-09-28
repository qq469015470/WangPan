#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSize>
#include <QGridLayout>
#include <QVBoxLayout>

class DaoHang: public QWidget
{
	Q_OBJECT

private:
	QVBoxLayout mainLayout;

	QListWidget menu;

	void OnClicked(QPushButton* _button);

public:
	DaoHang(QWidget* _parent);

	void AddMenu(QString _text);
};

class MainWindow: public QWidget 
{
	Q_OBJECT

private:
	std::string token;

	QGridLayout gridLayout;	

	QListWidget listWidget;

	DaoHang daohang;

	QHBoxLayout hBoxLayout;
	QPushButton uploadBtn;

//private slots:
	void ClickItem(QListWidgetItem* item);
	void AddFile(QIcon _icon, QString _name);
	void UploadFile();

public:
	MainWindow();
	~MainWindow();

	void SetToken(std::string _token);
	void RefreshList(const char* _path);

	void Logout();
};
