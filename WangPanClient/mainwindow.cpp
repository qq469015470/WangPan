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
	uploadView(this),
	downloadView(this)
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
	this->content.addWidget(&this->downloadView);

	//this->gridLayout.addWidget(&this->fileView, 0, 1);
	//this->gridLayout.addWidget(&this->listWidget, 1, 1);
	//this->setLayout(&this->gridLayout);	
	
	this->fileView.LocationChangeCallback([this](std::string _location)
	{
		this->RefreshList();
	});

	this->fileView.UploadBtnClickedCallback(std::bind(&MainWindow::UploadFile, this));
	this->fileView.DownloadBtnClickedCallback(std::bind(&MainWindow::DownloadFile, this));
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

	std::thread thread([this](std::string _filename, std::string _path)
	{
		//std::vector<const UploadViewModel::Info*> temp;
		//for(int i = 0; i < 20;i++)
		//{
		//	temp.push_back(this->uploadView.AddItem(std::to_string(i), i));
		//}
		//
		//for(size_t i = 0; i < temp.size(); i++)
		//{
		//	this->uploadView.SetItemProgress(temp[i], 100 - i);
		//	//this->uploadView.RemoveItem(temp[i]);
		//}

		ProgressViewModel::Info* const info = this->uploadView.AddItem(_filename, 0, "上传中");
		try
		{	
			auto func = std::bind([this](float _progress, ProgressViewModel::Info* const _info)
			{
				std::string temp;
				if(_progress < 1.0f)
					temp = "上传中";
				else
					temp = "上传完毕";

				this->uploadView.SetItem(_info, _progress * 100, temp);
			}, std::placeholders::_1, info);

			this->request.UploadFile(this->token, _path, this->fileView.GetLocation().c_str(), func);
		}
		catch(std::runtime_error& _ex)
		{
			//**由于线程不同QT信息框会引发异常
			//TODO
			//该函数只创建任务
			//立即弹出信息框提示上传是否成功
			//后台线程再处理下载
			QMessageBox::critical(this, tr("错误"), _ex.what());
			this->uploadView.RemoveItem(info);
		}

		this->RefreshList();
	}, filename, path);

	thread.detach();	
	//QMessageBox::information(this, tr("信息"), tr("上传完毕"));
}

void MainWindow::DownloadFile()
{
	const std::string downloadName = this->fileView.GetSelectedItems()[0]->text().toStdString();
	const std::string savePath = QFileDialog::getSaveFileName(this, tr("下载文件"), downloadName.c_str()).toStdString();

	if(savePath == "")
		return;

	std::string saveName(savePath);
	std::string::size_type pos(savePath.find_last_of('/'));
	if(pos != std::string::npos)
		saveName = saveName.substr(pos + 1);
	
	const std::string downloadPath = this->fileView.GetLocation() + downloadName;
	
	const ProgressViewModel::Info* const info = this->downloadView.AddItem(saveName, 0, "下载中");
	std::thread thread([this](std::string _savePath, std::string _downloadPath, const ProgressViewModel::Info* const  _info) 
	{
		size_t hadRead(0);
		std::function<void(char*, size_t, size_t)> func = 
		(	
			std::bind([this, &hadRead](char* _bytes, size_t _byteSize, size_t _fileSize, const ProgressViewModel::Info* const _info)
			{
				hadRead += _byteSize;
				const float progress = static_cast<float>(hadRead) / _fileSize;

				if(progress < 1.0f)
					this->downloadView.SetItem(_info, progress * 100, "下载中");
				else
					this->downloadView.SetItem(_info, progress * 100, "下载完毕");

			}, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, _info)
		);

		try
		{
			this->request.DownloadFile(this->token, _downloadPath, _savePath, func);
		}
		catch(std::runtime_error& _ex)
		{
			QMessageBox::critical(nullptr, tr("错误"), _ex.what());
			this->downloadView.RemoveItem(_info);
		}

	}, savePath, downloadPath, info);

	thread.detach();
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
