#pragma once

#include "ClientRequest.hpp"

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSize>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>

class MainWindow;

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

class CreateDirectoryDialog: public QDialog
{
	Q_OBJECT

private:
	QGridLayout mainLayout;
	QFormLayout formLayout;

	QLabel dirLabel;
	QLineEdit dirEdit;
	QPushButton addBtn;
	QPushButton celBtn;

	std::string basePath;
	std::string token;
	ClientRequest* request;


public:
	CreateDirectoryDialog(QWidget* _parent, std::string _basePath, std::string _token, ClientRequest* _request);

	void CreateDirectory();
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

	ClientRequest request;

//private slots:
	void UploadFile();
	void CreateDirectory();

public:
	MainWindow();
	~MainWindow();

	void SetToken(std::string _token);
	void RefreshList(const char* _path);

	void Logout();
};
