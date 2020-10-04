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
	std::vector<Info> values;

public:	
	explicit UploadViewModel(QObject* _parent);
	virtual int rowCount(const QModelIndex& _parent) const override;
	virtual int columnCount(const QModelIndex& _parent) const override;
	virtual QVariant data(const QModelIndex& _index, int _role) const override;
	const std::vector<Info>& getData() const;
	void setData(std::vector<Info> _values);
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

	size_t AddItem(std::string _filename, int _progress);
	void SetItemProgress(int _index, int _progress);
	void RemoveItem(int _index);
};
