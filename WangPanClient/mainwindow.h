#pragma once

#include "ClientRequest.hpp"
#include "DaoHang.h"
#include "FileView.h"
#include "CreateDirectoryDialog.h"
#include "ProgressView.h"

#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>

#include <unordered_set>
#include <functional>

class MainWindow: public QWidget 
{
	Q_OBJECT

private:	
	std::string token;

	QGridLayout gridLayout;	
	QStackedWidget content;

	DaoHang daohang;
	FileView fileView;
	ProgressView uploadView;
	ProgressView downloadView;

	ClientRequest request;	

//private slots:
	void UploadFile();
	void DownloadFile();
	void CreateDirectory();

public:
	MainWindow();
	~MainWindow();

	void SetToken(std::string _token);
	void RefreshList();

	void Logout();
};
