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

#include <unordered_set>
#include <functional>

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
	const QSize ITEM_SIZE = QSize(100, 100);

	std::string location;

	QGridLayout mainLayout;
	QHBoxLayout vboxLayout;
	QListWidget listWidget;

	QPushButton backBtn;
	QLabel locationLabel;

	QIcon fileIcon;
	QIcon folderIcon;

	//用于判断是否属于文件夹
	std::unordered_set<std::string> dirs;

	std::function<void(std::string)> locationCallback;

	void BackPath();

public:
	Content(QWidget* _parent = nullptr);

	void AddFile(QString _filename);
	void AddDirectory(QString _filename);
	void ClearFiles();
	void ClickItem(QListWidgetItem* item);
	void SetLocation(std::string _location);
	const std::string& GetLocation() const;

	//目录变更回调
	void LocationChangeCallback(std::function<void(std::string)>);
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

public:
	CreateDirectoryDialog(QWidget* _parent);

	void CreateDirectory();
	QLineEdit& GetDirectoryEdit();
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
	void RefreshList();

	void Logout();
};
