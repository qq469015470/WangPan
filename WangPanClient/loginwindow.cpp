#include "loginwindow.h"
#include "mainwindow.h"

#include <QTcpSocket>
#include <QHostAddress>

LoginWindow::LoginWindow():
	userLabel(tr("账号:"), this),
	username(this),
	passLabel(tr("密码:"), this),
	password(this),
	loginBtn(tr("登录"), this),
	registerBtn(tr("注册"), this)
{
	this->setWindowTitle(tr("网盘"));

	this->password.setEchoMode(QLineEdit::Password);

	this->vBoxLayout.addWidget(&this->userLabel);
	this->vBoxLayout.addWidget(&this->username, 1);
	this->vBoxLayout.addWidget(&this->passLabel);
	this->vBoxLayout.addWidget(&this->password, 1);
	this->vBoxLayout.addStretch(2);
	this->vBoxLayout.addWidget(&this->loginBtn, 2);
	this->vBoxLayout.addWidget(&this->registerBtn, 2);

	this->username.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	this->userLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	this->password.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	this->passLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	this->loginBtn.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	this->registerBtn.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	this->setLayout(&this->vBoxLayout);
	
	this->connect(&this->loginBtn, &QPushButton::clicked, this, &LoginWindow::Login);
	this->connect(&this->registerBtn, &QPushButton::clicked, this, &LoginWindow::Register);
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::Login()
{
	QTcpSocket qSock;

	qSock.connectToHost(QHostAddress("127.0.0.1"), 9999);
	if(qSock.waitForConnected() == false)
	{
		QMessageBox::critical(NULL, tr("错误"), tr("连接服务器失败"));
		return;
	}

	std::string temp("login ");
	temp += this->username.text().toStdString();
	temp += " ";
	temp += this->password.text().toStdString();

	if(qSock.write(temp.data(), temp.size()) == -1)
	{
		QMessageBox::critical(NULL, tr("错误"), tr("发送信息失败"));
		return;
	}

	qSock.waitForBytesWritten();
	qSock.waitForReadyRead();

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	const qint64 recvLen = qSock.read(buffer, sizeof(buffer));
	if(recvLen <= 0)
	{
		QMessageBox::critical(NULL, tr("错误"), tr("接收信息失败"));
		return;
	}

	temp = std::string(buffer, recvLen);
	
	QMessageBox::information(NULL, "", temp.c_str());

	if(temp != "login error")
	{
		QMessageBox::information(NULL, tr("信息"), tr("登录成功!"));

		static MainWindow mainwindow;

		mainwindow.show();
		this->close();
	}
}

void LoginWindow::Register()
{
	QTcpSocket qSock;

	qSock.connectToHost(QHostAddress("127.0.0.1"), 9999);
	if(qSock.waitForConnected() == false)
	{
		QMessageBox::critical(NULL, tr("错误"), tr("连接服务器失败"));
		return;
	}

	std::string temp("register ");
	temp += this->username.text().toStdString();
	temp += " ";
	temp += this->password.text().toStdString();

	if(qSock.write(temp.data(), temp.size()) == -1)
	{
		QMessageBox::critical(NULL, tr("错误"), tr("发送信息失败"));
		return;
	}

	qSock.waitForBytesWritten();
	qSock.waitForReadyRead();
	
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	const qint64 recvLen = qSock.read(buffer, sizeof(buffer));
	if(recvLen <= 0)
	{
		QMessageBox::critical(NULL, tr("错误"), tr("接收信息失败"));
	}
	
	QMessageBox::information(NULL, "", buffer);
}
