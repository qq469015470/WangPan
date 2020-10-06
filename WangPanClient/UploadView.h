#pragma once

#include <QWidget>
#include <QListView>
#include <QTableView>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QAbstractTableModel>
#include <QProgressBar>
#include <QStyledItemDelegate>
#include <QPainter>

#include <memory>
#include <string>
#include <vector>

class UploadViewModel: public QAbstractTableModel
{
public:
	struct Info
	{
		std::string filename;
		int progress;	
		std::string status;
	};

private:
	std::vector<std::unique_ptr<Info>> values;

public:	
	explicit UploadViewModel(QObject* _parent);
	virtual int rowCount(const QModelIndex& _parent) const override;
	virtual int columnCount(const QModelIndex& _parent) const override;
	virtual QVariant data(const QModelIndex& _index, int _role) const override;
	std::vector<std::unique_ptr<Info>>& getData();
	void setData(std::vector<std::unique_ptr<Info>>&& _values);
};

class ProgressBarDelegate: public QStyledItemDelegate 
{
	Q_OBJECT

public:
	explicit ProgressBarDelegate(QObject* _parent);
	virtual void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;
};

class UploadView: public QWidget
{
	Q_OBJECT
	
private:
	QGridLayout mainLayout;

	UploadViewModel model;
	QTableView list;

	ProgressBarDelegate delegate;

public:
	UploadView(QWidget* _parent);

	const UploadViewModel::Info* AddItem(std::string _filename, int _progress, std::string _status);
	void SetItem(const UploadViewModel::Info* _info, int _progress, std::string _status);
	void RemoveItem(const UploadViewModel::Info* _info);
};
