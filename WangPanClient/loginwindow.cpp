#include "loginwindow.h"
#include "mainwindow.h"

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
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::Login()
{
	static MainWindow mainwindow;

	mainwindow.show();
	this->close();
}
