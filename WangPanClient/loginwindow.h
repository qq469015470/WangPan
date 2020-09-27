#pragma once

#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>

class LoginWindow : public QWidget 
{
	Q_OBJECT

private:
	QVBoxLayout vBoxLayout;
	QGridLayout userGrid;
	
	QLabel userLabel;
	QLineEdit username;
	
	QLabel passLabel;
	QLineEdit password;

	QPushButton loginBtn;
	QPushButton registerBtn;
	
public:
	LoginWindow();
	~LoginWindow();
	
	void Login();
	void Register();
};
