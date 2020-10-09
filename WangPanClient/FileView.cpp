#include "FileView.h"

FileView::FileView(QWidget* _parent):
	QWidget(_parent),
	location("/"),
	mainLayout(this),
	listWidget(this),
	uploadBtn(tr("上传"), this),
	createDirBtn(tr("新建文件夹"), this),
	downloadBtn(tr("下载"), this),
	removeBtn(tr("删除所选"), this),
	backBtn(tr("返回上级"), this),
	locationLabel(tr("当前路径: ") + this->location.c_str(), this),
	fileIcon("/home/administrator/Downloads/file.ico"),
	folderIcon("/home/administrator/Downloads/folder.ico"),
	uploadBtnCallback([](){}),
	downloadBtnCallback([](){}),
	createDirBtnCallback([](){}),
	removeBtnCallback([](){}),
	locationCallback([](std::string _location){})
{
	this->downloadBtn.hide();
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
	this->btnLayout.addWidget(&this->downloadBtn);
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
	this->connect(&this->downloadBtn, &QPushButton::clicked, this, [this](){this->downloadBtnCallback();});
	this->connect(&this->createDirBtn, &QPushButton::clicked, this, [this](){this->createDirBtnCallback();});
	this->connect(&this->removeBtn, &QPushButton::clicked, this, [this](){this->removeBtnCallback();});
	this->connect(&this->listWidget, &QListWidget::itemDoubleClicked, this, &FileView::DoubleClickItem);
	this->connect(&this->listWidget, &QListWidget::itemSelectionChanged, this, [this]()
	{
		if(this->listWidget.selectedItems().count() == 1
		&& this->dirs.find(this->listWidget.selectedItems()[0]->text().toStdString()) == this->dirs.end())
			this->downloadBtn.show();
		else
			this->downloadBtn.hide();

		if(this->listWidget.selectedItems().count() > 0)
			this->removeBtn.show();
		else
			this->removeBtn.hide();
	});

	this->connect(&this->backBtn, &QPushButton::clicked, this, &FileView::BackPath);
	
	this->mainLayout.setSpacing(0);
	this->mainLayout.setContentsMargins(0,0,0,0);
}

void FileView::BackPath()
{
	std::string::size_type pos(this->location.substr(0, this->location.size() - 1).find_last_of('/'));

	if(pos == std::string::npos)
		return;

	this->SetLocation(this->location.substr(0, pos) + '/');
}

void FileView::AddFile(QString _filename)
{
	//this->listWidget.insertItem(0, _filename);
	this->listWidget.addItem(_filename);
	QListWidgetItem* const listWidgetItem = this->listWidget.item(this->listWidget.count() - 1);
	listWidgetItem->setIcon(this->fileIcon);	
        listWidgetItem->setSizeHint(this->ITEM_SIZE);	
}

void FileView::AddDirectory(QString _filename)
{
	this->dirs.insert(_filename.toStdString());

	this->listWidget.addItem(_filename);
	QListWidgetItem* const listWidgetItem = this-> listWidget.item(this->listWidget.count() - 1);
	listWidgetItem->setIcon(this->folderIcon);
	listWidgetItem->setSizeHint(this->ITEM_SIZE);
}

void FileView::ClearFiles()
{
	this->listWidget.clear();
	this->dirs.clear();
}

void FileView::DoubleClickItem(QListWidgetItem* _item)
{
	if(this->dirs.find(_item->text().toStdString()) != this->dirs.end())
	{
		std::string temp(this->GetLocation());
		temp += _item->text().toStdString();
		temp += '/';
		this->SetLocation(temp);
	}
}

void FileView::SetLocation(std::string _location)
{
	this->location = _location;
	this->locationLabel.setText(tr("当前路径:") + this->location.c_str());
	this->locationCallback(this->location);
}

void FileView::LocationChangeCallback(std::function<void(std::string)> _callback)
{
	this->locationCallback = _callback;
}

void FileView::UploadBtnClickedCallback(std::function<void()> _callback)
{
	this->uploadBtnCallback = _callback;
}

void FileView::DownloadBtnClickedCallback(std::function<void()> _callback)
{
	this->downloadBtnCallback = _callback;
}

void FileView::CreateDirBtnClickedCallback(std::function<void()> _callback)
{
	this->createDirBtnCallback = _callback;
}

void FileView::RemoveBtnClickedCallback(std::function<void()> _callback)
{
	this->removeBtnCallback = _callback;
}

const std::string& FileView::GetLocation() const
{
	return this->location;
}

const QList<QListWidgetItem*> FileView::GetSelectedItems() const
{
	return this->listWidget.selectedItems();
}
