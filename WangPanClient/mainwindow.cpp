#include "mainwindow.h"
#include "loginwindow.h"

#include <QFileDialog>
#include <QTcpSocket>
#include <QHostAddress>

#include <iostream>
#include <fstream>

DaoHang::DaoHang(QWidget* _parent):
	QWidget(_parent),
	mainLayout(this),
	menu(this)
{
	this->menu.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	this->menu.setFixedWidth(100);

	this->setStyleSheet
	(
		" \
		QListWidget { \
			border: 0; \
		} \
		\
		QListWidget::item { \
			border: 0; \
		} \
		QListWidget::item::selected { \
			background: lightblue; \
			color: white; \
		} \
		"
	);

	this->mainLayout.setSpacing(0);
	this->mainLayout.setContentsMargins(0,0,0,0);
	this->mainLayout.addWidget(&this->menu);

	this->AddMenu(tr("文件列表"));
	this->AddMenu(tr("上传列表"));
}

void DaoHang::OnClicked(QPushButton* _button)
{

}

void DaoHang::AddMenu(QString _text)
{
	this->menu.addItem(_text);
	this->menu.item(this->menu.count() - 1)->setTextAlignment(Qt::AlignHCenter);
}

MainWindow::MainWindow():
	gridLayout(this),
	listWidget(this),
	daohang(this),
	hBoxLayout(),
	uploadBtn(tr("上传"), this)
{
	this->setWindowTitle(tr("网盘"));

	const QSize IMAGE_SIZE(64, 64);

	this->listWidget.setIconSize(IMAGE_SIZE);
	this->listWidget.setResizeMode(QListView::Adjust);
	this->listWidget.setViewMode(QListView::IconMode);
	this->listWidget.setMovement(QListView::Static);
	this->listWidget.setSpacing(8);
	this->listWidget.setSelectionMode(QAbstractItemView::ContiguousSelection);

	this->hBoxLayout.addWidget(&this->uploadBtn);
	this->hBoxLayout.addStretch();

	for(int i = 0; i <= 10; i++)
	{
		std::string temp("文本");

		temp += std::to_string(i);
		this->AddFile(QIcon("/home/administrator/Downloads/chat.ico"), temp.c_str());
	}

	//this->connect(&this->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(ClickItem(QListWidgetItem*)));
	this->connect(&this->listWidget, &QListWidget::itemClicked, this, &MainWindow::ClickItem);
	this->connect(&this->uploadBtn, &QPushButton::clicked, this, &MainWindow::UploadFile);

	this->gridLayout.addLayout(&this->hBoxLayout, 0, 1);
	this->gridLayout.addWidget(&this->daohang, 1, 0);
	this->gridLayout.addWidget(&this->listWidget, 1, 1);
	//this->setLayout(&this->gridLayout);	
}

MainWindow::~MainWindow()
{

}

void MainWindow::ClickItem(QListWidgetItem* item)
{
	std::cout << "clickitem" << item << std::endl;
}

void MainWindow::AddFile(QIcon _icon, QString _filename)
{
	const QSize ITEM_SIZE(100, 100);

	//this->listWidget.insertItem(0, _filename);
	this->listWidget.addItem(_filename);
	QListWidgetItem* const listWidgetItem = this->listWidget.item(this->listWidget.count() - 1);
	listWidgetItem->setIcon(_icon);	
        listWidgetItem->setSizeHint(ITEM_SIZE);	
}

void MainWindow::UploadFile()
{
	std::string filename(QFileDialog::getOpenFileName(this, tr("上传文件"), "/", tr("所有文件 (*)")).toStdString());
	if(filename == "")
		return;

	std::string::size_type pos(filename.find_last_of('/'));
	if(pos != std::string::npos)
		pos += 1;

	std::string path(filename.substr(0, pos));
	std::string name(filename.substr(pos));

	QTcpSocket qSock;

	qSock.connectToHost(QHostAddress("127.0.0.1"), 9999);

	std::ifstream file(path + name, std::ios::in| std::ios::binary);
	file.seekg(0, std::ios::end);
	const size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::string cmd("upload ");
	cmd += this->token;
	cmd += ' ';
	cmd += name;
	cmd += ' ';
	cmd += '/';
	cmd += ' ';
	cmd += std::to_string(fileSize);
	
	if(!file.is_open())
	{
		QMessageBox::critical(this, tr("错误"), tr("打开文件失败"));	
		return;
	}

	if(qSock.waitForConnected() == false)
	{
		QMessageBox::critical(this, tr("错误"), tr("连接服务器失败"));
		return;
	}

	if(qSock.write(cmd.data(), cmd.size()) == -1)
	{
		QMessageBox::critical(this, tr("错误"), tr("发送信息失败"));
		return;
	}

	qSock.waitForBytesWritten();
	qSock.waitForReadyRead();

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
	if(recvLen <= 0)
	{
		QMessageBox::critical(this, tr("错误"), tr("接收信息失败"));
		return;
	}

	std::string temp(buffer, recvLen);
	if(temp != "upload ready")
	{
		QMessageBox::critical(this, tr("错误"), buffer);
		return;
	}

	size_t hadRead(0);

	while(hadRead < fileSize)
	{
		const size_t unReadSize = fileSize - hadRead;
		const size_t readSize = std::min(sizeof(buffer), unReadSize);
			
		memset(buffer, 0, sizeof(buffer));
		file.read(buffer, readSize);
		hadRead += readSize; 
		qSock.write(buffer, readSize);
		qSock.waitForBytesWritten();
	}

	qSock.waitForReadyRead();
	recvLen = qSock.read(buffer, sizeof(buffer));	
	temp = std::string(buffer, recvLen);
	if(temp != "upload success")
	{
		QMessageBox::critical(this, tr("错误"), tr("上传异常"));
		return;
	}

	file.close();
}

void MainWindow::RefreshList(const char* _path)
{
	this->listWidget.clear();

	QTcpSocket qSock;

	qSock.connectToHost(QHostAddress("127.0.0.1"), 9999);
	
	if(qSock.waitForConnected() == false)
	{
		QMessageBox::critical(this, tr("错误"), tr("连接服务器失败"));
		return;
	}

	std::string cmd("dir ");
	cmd += this->token;
	cmd += ' ';
	cmd += "/";

	if(qSock.write(cmd.data(), cmd.size()) == -1)
	{
		QMessageBox::critical(this, tr("错误"), tr("发送信息失败"));
		return;
	}

	qSock.waitForBytesWritten();
	qSock.waitForReadyRead();

	char buffer[1024];
	qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
	if(recvLen <= 0)
	{
		QMessageBox::critical(this, tr("错误"), tr("接收信息失败"));
		return;
	}

	std::string temp(buffer, recvLen);
	std::string::size_type left(0);
	std::string::size_type right(temp.find(' '));
	size_t fileCount(std::stoll(temp.substr(left, right - left)));

	std::cout << "server:" << temp << std::endl;
	if(right != std::string::npos)
		temp = temp.substr(right + 1);
	do
	{
		left = 0;
		right = temp.find(' ', left);

		std::string filename(temp.substr(left, right - left));
		std::cout << filename << std::endl;
		temp = temp.substr(right + 1);
		
		this->AddFile(QIcon("/home/administrator/Downloads/chat.ico"), filename.c_str());

		fileCount--;
	}while(fileCount > 0 && right != std::string::npos);
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
