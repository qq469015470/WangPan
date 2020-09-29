#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSize>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

class DaoHang: public QWidget
{
	Q_OBJECT

private:
	QVBoxLayout mainLayout;

	QListWidget menu;

	void OnClicked(QPushButton* _button);

public:
	DaoHang(QWidget* _parent = nullptr);

	void AddMenu(QString _text);
};

class Content: public QWidget
{
	Q_OBJECT

private:
	std::string location;

	QGridLayout mainLayout;
	QHBoxLayout vboxLayout;
	QListWidget listWidget;

	QPushButton backBtn;
	QLabel locationLabel;


public:
	Content(QWidget* _parent = nullptr);

	void AddFile(QIcon _icon, QString _filename);
	void ClearFiles();
	void ClickItem(QListWidgetItem* item);
	const std::string& GetLocation() const;
};

class MainWindow: public QWidget 
{
	Q_OBJECT

private:
	std::string token;

	QGridLayout gridLayout;	


	DaoHang daohang;
	Content content;

	QHBoxLayout hBoxLayout;
	QPushButton uploadBtn;
	QPushButton createDirBtn;

//private slots:
	void UploadFile();

public:
	MainWindow();
	~MainWindow();

	void SetToken(std::string _token);
	void RefreshList(const char* _path);

	void Logout();
};
