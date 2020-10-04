#include "mainwindow.h"
#include "loginwindow.h"

#include <QFileDialog>
#include <QHostAddress>

#include <iostream>
#include <fstream>
#include <thread>

MainWindow::MainWindow():
	gridLayout(this),
	content(this),
	daohang(this),
	fileView(this),
	uploadView(this)
{	
	this->setWindowTitle(tr("网盘"));

	//this->connect(&this->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(ClickItem(QListWidgetItem*)));
	//this->connect(&this->listWidget, &QListWidget::itemClicked, this, &MainWindow::ClickItem);
	//this->connect(&this->uploadBtn, &QPushButton::clicked, this, &MainWindow::UploadFile);
	//this->connect(&this->createDirBtn, &QPushButton::clicked, this, &MainWindow::CreateDirectory);
	

	this->gridLayout.addWidget(&this->daohang, 0, 0);
	this->gridLayout.addWidget(&this->content, 0, 1);

	this->content.addWidget(&this->fileView);
	this->content.addWidget(&this->uploadView);
	this->content.addWidget(&this->uploadView);

	//this->gridLayout.addWidget(&this->fileView, 0, 1);
	//this->gridLayout.addWidget(&this->listWidget, 1, 1);
	//this->setLayout(&this->gridLayout);	
	
	this->fileView.LocationChangeCallback([this](std::string _location)
	{
		this->RefreshList();
	});

	this->fileView.UploadBtnClickedCallback(std::bind(&MainWindow::UploadFile, this));
	this->fileView.CreateDirBtnClickedCallback(std::bind(&MainWindow::CreateDirectory, this));
	this->fileView.RemoveBtnClickedCallback([this]()
	{
		std::string location(this->fileView.GetLocation());
		for(const auto& item: this->fileView.GetSelectedItems())
		{
			try
			{
				this->request.RemoveFile(this->token, location + item->text().toStdString());
			}
			catch(std::runtime_error& _ex)
			{
				QMessageBox::critical(this, tr("错误"), _ex.what());
			}
		}
		this->RefreshList();
	});
	this->daohang.MenuSelectedChange([this](int _index)
	{
		this->content.setCurrentIndex(_index);
	});

	this->daohang.AddMenu(tr("我的网盘"));
	this->daohang.AddMenu(tr("上传列表"));
	this->daohang.AddMenu(tr("下载列表"));
}

MainWindow::~MainWindow()
{

}

void MainWindow::UploadFile()
{
	std::string path(QFileDialog::getOpenFileName(this, tr("上传文件"), "/", tr("所有文件 (*)")).toStdString());
	if(path == "")
		return;

	std::string filename(path);
	std::string::size_type pos(path.find_last_of('/'));
	if(pos != std::string::npos)
		filename = filename.substr(pos + 1);

	std::thread thread([this]()
	{
		for(int i = 0; i < 20;i++)
		{
			this->uploadView.AddItem(std::to_string(i), i);
		}	
	});

	thread.detach();

	return;	
	
	try
	{	
		this->request.UploadFile(this->token, path, this->fileView.GetLocation().c_str());
	}
	catch(std::runtime_error& _ex)
	{
		QMessageBox::critical(this, tr("错误"), _ex.what());
	}

	this->RefreshList();
	QMessageBox::information(this, tr("信息"), tr("上传完毕"));
}

void MainWindow::CreateDirectory()
{
	CreateDirectoryDialog dialog(this);

	if(dialog.exec() == QDialog::Accepted)
	{
		try
		{
			this->request.CreateDirectory(this->token, this->fileView.GetLocation() + '/' + dialog.GetDirectoryEdit().text().toStdString());
		}
		catch(std::runtime_error& _ex)
		{
			QMessageBox::critical(this, tr("错误"), _ex.what());
		}
		this->RefreshList();
	}
}

void MainWindow::RefreshList()
{
	this->fileView.ClearFiles();

	std::vector<std::string> files;
	try
	{
		files = this->request.GetFilesList(this->token, this->fileView.GetLocation());
	}
	catch (std::runtime_error& _ex)
	{
		QMessageBox::critical(this, tr("错误"), _ex.what());
		return;
	}

	for(const auto& item: files)
	{
		std::string filename(item);
		if(item.front() == '*')
		{
			filename = filename.substr(1);
			this->fileView.AddDirectory(filename.c_str());
		}
		else
		{
			this->fileView.AddFile(filename.c_str());
		}

	}

	return;
}

void MainWindow::SetToken(std::string _token)
{
	this->token = _token;
}

void MainWindow::Logout()
{
	static LoginWindow loginwindow;	       
	loginwindow.show();
	this->close();
}
