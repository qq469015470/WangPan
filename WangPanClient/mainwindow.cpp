#include "mainwindow.h"
#include "loginwindow.h"

#include <QFileDialog>
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
			border: 1px solid rgb(184, 184, 184); \
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

Content::Content(QWidget* _parent):
	QWidget(_parent),
	location("/"),
	mainLayout(this),
	listWidget(this),
	uploadBtn(tr("上传"), this),
	createDirBtn(tr("新建文件夹"), this),
	removeBtn(tr("删除所选"), this),
	backBtn(tr("返回上级"), this),
	locationLabel(tr("当前路径: ") + this->location.c_str(), this),
	fileIcon("/home/administrator/Downloads/file.ico"),
	folderIcon("/home/administrator/Downloads/folder.ico"),
	uploadBtnCallback([](){}),
	createDirBtnCallback([](){}),
	removeBtnCallback([](){}),
	locationCallback([](std::string _location){})
{
	this->removeBtn.hide();
	this->removeBtn.setStyleSheet("QPushButton{color: red;}");

	this->setStyleSheet
	(
		" \
		QLabel { \
			background: white; \
			border: 1px solid rgb(184, 184, 184); \
			border-bottom: 0; \
			padding: 10px; \
		} \
		  \
		QPushButton { \
			padding: 10px; \
		} \
		"
	);

	this->btnLayout.addWidget(&this->uploadBtn);
	this->btnLayout.addWidget(&this->createDirBtn);
	this->btnLayout.addWidget(&this->removeBtn);
	this->btnLayout.addStretch();
	this->btnLayout.setContentsMargins(0, 0, 0, 8);
	this->btnLayout.setSpacing(5);

	this->addressLayout.addWidget(&this->backBtn);
	this->addressLayout.addWidget(&this->locationLabel);

	this->mainLayout.addLayout(&this->btnLayout, 0, 0);
	this->mainLayout.addLayout(&this->addressLayout, 1, 0);
	//this->mainLayout.addWidget(&this->backBtn, 0, 0);
	//this->mainLayout.addWidget(&this->locationLabel, 0, 1);
	this->mainLayout.addWidget(&this->listWidget, 2, 0);
	
	const QSize IMAGE_SIZE(64, 64);

	this->listWidget.setIconSize(IMAGE_SIZE);
	this->listWidget.setResizeMode(QListView::Adjust);
	this->listWidget.setViewMode(QListView::IconMode);
	this->listWidget.setMovement(QListView::Static);
	this->listWidget.setSpacing(8);
	this->listWidget.setSelectionMode(QAbstractItemView::ContiguousSelection);

	this->backBtn.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	this->locationLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	this->connect(&this->uploadBtn, &QPushButton::clicked, this, [this](){this->uploadBtnCallback();});
	this->connect(&this->createDirBtn, &QPushButton::clicked, this, [this](){this->createDirBtnCallback();});
	this->connect(&this->removeBtn, &QPushButton::clicked, this, [this](){this->removeBtnCallback();});
	this->connect(&this->listWidget, &QListWidget::itemDoubleClicked, this, &Content::DoubleClickItem);
	this->connect(&this->listWidget, &QListWidget::itemSelectionChanged, this, [this]()
	{
		if(this->listWidget.selectedItems().count() > 0)
			this->removeBtn.show();
		else
			this->removeBtn.hide();
	});

	this->connect(&this->backBtn, &QPushButton::clicked, this, &Content::BackPath);
	
	this->mainLayout.setSpacing(0);
	this->mainLayout.setContentsMargins(0,0,0,0);
}

void Content::BackPath()
{
	std::string::size_type pos(this->location.substr(0, this->location.size() - 1).find_last_of('/'));

	if(pos == std::string::npos)
		return;

	this->SetLocation(this->location.substr(0, pos) + '/');
}

void Content::AddFile(QString _filename)
{
	//this->listWidget.insertItem(0, _filename);
	this->listWidget.addItem(_filename);
	QListWidgetItem* const listWidgetItem = this->listWidget.item(this->listWidget.count() - 1);
	listWidgetItem->setIcon(this->fileIcon);	
        listWidgetItem->setSizeHint(this->ITEM_SIZE);	
}

void Content::AddDirectory(QString _filename)
{
	this->dirs.insert(_filename.toStdString());

	this->listWidget.addItem(_filename);
	QListWidgetItem* const listWidgetItem = this-> listWidget.item(this->listWidget.count() - 1);
	listWidgetItem->setIcon(this->folderIcon);
	listWidgetItem->setSizeHint(this->ITEM_SIZE);
}

void Content::ClearFiles()
{
	this->listWidget.clear();
	this->dirs.clear();
}

void Content::DoubleClickItem(QListWidgetItem* _item)
{
	if(this->dirs.find(_item->text().toStdString()) != this->dirs.end())
	{
		std::cout << "folder" << std::endl;
		std::string temp(this->GetLocation());
		temp += _item->text().toStdString();
		temp += '/';
		this->SetLocation(temp);
	}

	std::cout << "clickitem" << _item << std::endl;
}

void Content::SetLocation(std::string _location)
{
	this->location = _location;
	this->locationLabel.setText(tr("当前路径:") + this->location.c_str());
	this->locationCallback(this->location);
}

void Content::LocationChangeCallback(std::function<void(std::string)> _callback)
{
	this->locationCallback = _callback;
}

void Content::UploadBtnClickedCallback(std::function<void()> _callback)
{
	this->uploadBtnCallback = _callback;
}

void Content::CreateDirBtnClickedCallback(std::function<void()> _callback)
{
	this->createDirBtnCallback = _callback;
}

void Content::RemoveBtnClickedCallback(std::function<void()> _callback)
{
	this->removeBtnCallback = _callback;
}

const std::string& Content::GetLocation() const
{
	return this->location;
}

const QList<QListWidgetItem*> Content::GetSelectedItems() const
{
	return this->listWidget.selectedItems();
}

CreateDirectoryDialog::CreateDirectoryDialog(QWidget* _parent):
	QDialog(_parent),
	mainLayout(this),
	dirLabel(tr("文件夹名称:"), this),
	dirEdit(this),
	addBtn(tr("添加"), this),
	celBtn(tr("取消"), this)
{

	this->formLayout.addRow(&this->dirLabel, &this->dirEdit);
	this->formLayout.setFormAlignment(Qt::AlignVCenter);

	this->mainLayout.addLayout(&this->formLayout, 0, 0, 1, 2);
	this->mainLayout.addWidget(&this->addBtn, 1, 0);
	this->mainLayout.addWidget(&this->celBtn, 1, 1);

	this->addBtn.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	this->celBtn.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	this->connect(&this->addBtn, &QPushButton::clicked, this, &std::remove_pointer<decltype(this)>::type::CreateDirectory);
	this->connect(&this->celBtn, &QPushButton::clicked, this, &QDialog::close);
}

void CreateDirectoryDialog::CreateDirectory()
{
	this->accept();
}

QLineEdit& CreateDirectoryDialog::GetDirectoryEdit()
{
	return this->dirEdit;
}

MainWindow::MainWindow():
	gridLayout(this),
	daohang(this),
	content(this)
{	
	this->setWindowTitle(tr("网盘"));

	//this->connect(&this->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(ClickItem(QListWidgetItem*)));
	//this->connect(&this->listWidget, &QListWidget::itemClicked, this, &MainWindow::ClickItem);
	//this->connect(&this->uploadBtn, &QPushButton::clicked, this, &MainWindow::UploadFile);
	//this->connect(&this->createDirBtn, &QPushButton::clicked, this, &MainWindow::CreateDirectory);
	

	this->gridLayout.addWidget(&this->daohang, 0, 0);
	this->gridLayout.addWidget(&this->content, 0, 1);
	//this->gridLayout.addWidget(&this->listWidget, 1, 1);
	//this->setLayout(&this->gridLayout);	
	
	this->content.LocationChangeCallback([this](std::string _location)
	{
		this->RefreshList();
	});

	this->content.UploadBtnClickedCallback(std::bind(&MainWindow::UploadFile, this));
	this->content.CreateDirBtnClickedCallback(std::bind(&MainWindow::CreateDirectory, this));
	this->content.RemoveBtnClickedCallback([this]()
	{
		std::string location(this->content.GetLocation());
		for(const auto& item: this->content.GetSelectedItems())
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
}

MainWindow::~MainWindow()
{

}

void MainWindow::UploadFile()
{
	std::string filename(QFileDialog::getOpenFileName(this, tr("上传文件"), "/", tr("所有文件 (*)")).toStdString());
	if(filename == "")
		return;
	
	try
	{	
		this->request.UploadFile(this->token, filename, this->content.GetLocation().c_str());
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
			this->request.CreateDirectory(this->token, this->content.GetLocation() + '/' + dialog.GetDirectoryEdit().text().toStdString());
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
	this->content.ClearFiles();

	std::vector<std::string> files;
	try
	{
		files = this->request.GetFilesList(this->token, this->content.GetLocation());
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
			this->content.AddDirectory(filename.c_str());
		}
		else
		{
			this->content.AddFile(filename.c_str());
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
