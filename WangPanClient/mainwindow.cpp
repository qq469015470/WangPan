#include "mainwindow.h"
#include "loginwindow.h"

#include <iostream>

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
	daohang(this)
{
	this->setWindowTitle(tr("网盘"));

	const QSize IMAGE_SIZE(64, 64);
	const QSize ITEM_SIZE(100, 100);

	this->listWidget.setIconSize(IMAGE_SIZE);
	this->listWidget.setResizeMode(QListView::Adjust);
	this->listWidget.setViewMode(QListView::IconMode);
	this->listWidget.setMovement(QListView::Static);
	this->listWidget.setSpacing(8);
	this->listWidget.setSelectionMode(QAbstractItemView::ContiguousSelection);

	for(int i = 0; i <= 10; i++)
	{
		std::string temp("文本"); 
		temp += std::to_string(i);

		this->listWidget.insertItem(i, tr(temp.c_str()));
		QListWidgetItem* const listWidgetItem = this->listWidget.item(this->listWidget.count() - 1);
	       	listWidgetItem->setIcon(QIcon("/home/administrator/Downloads/chat.ico"));	
        	listWidgetItem->setSizeHint(ITEM_SIZE);	
	}

	//this->connect(&this->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(ClickItem(QListWidgetItem*)));
	this->connect(&this->listWidget, &QListWidget::itemClicked, this, &MainWindow::ClickItem);

	this->gridLayout.addWidget(&this->daohang, 0, 0);
	this->gridLayout.addWidget(&this->listWidget, 0, 1);
	//this->setLayout(&this->gridLayout);
}

MainWindow::~MainWindow()
{

}

void MainWindow::ClickItem(QListWidgetItem* item)
{
	std::cout << "clickitem" << item << std::endl;
}

void MainWindow::Logout()
{
	static LoginWindow loginwindow;	       
	loginwindow.show();
	this->close();
}
