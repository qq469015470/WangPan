#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>

#include <functional>  

class DaoHang: public QWidget
{
	Q_OBJECT

private:
	QVBoxLayout mainLayout;

	QListWidget menu;
	
	std::function<void(int)> menuSelectedChangeCallback;

	void OnClicked(QPushButton* _button);

public:
	DaoHang(QWidget* _parent = nullptr);

	void AddMenu(QString _text);
	void MenuSelectedChange(std::function<void(int)> _callback);
};
