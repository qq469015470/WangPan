#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

#include <unordered_set>

class FileView: public QWidget
{
	Q_OBJECT

private:
	const QSize ITEM_SIZE = QSize(100, 100);

	std::string location;

	QGridLayout mainLayout;
	QHBoxLayout btnLayout;
	QHBoxLayout addressLayout;
	QListWidget listWidget;

	QPushButton uploadBtn;
	QPushButton createDirBtn;
	QPushButton removeBtn;

	QPushButton backBtn;
	QLabel locationLabel;

	QIcon fileIcon;
	QIcon folderIcon;

	//用于判断是否属于文件夹
	std::unordered_set<std::string> dirs;

	std::function<void()> uploadBtnCallback;
	std::function<void()> createDirBtnCallback;
	std::function<void()> removeBtnCallback;
	std::function<void(std::string)> locationCallback;

	void BackPath();

public:
	FileView(QWidget* _parent = nullptr);

	void AddFile(QString _filename);
	void AddDirectory(QString _filename);
	void ClearFiles();
	void DoubleClickItem(QListWidgetItem* item);
	void SetLocation(std::string _location);
	const std::string& GetLocation() const;
	const QList<QListWidgetItem*> GetSelectedItems() const;

	//目录变更回调
	void LocationChangeCallback(std::function<void(std::string)> _callback);
	void UploadBtnClickedCallback(std::function<void()> _callback);
	void CreateDirBtnClickedCallback(std::function<void()> _callback);
	void RemoveBtnClickedCallback(std::function<void()> _callback);
};
