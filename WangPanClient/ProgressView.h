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

class ProgressViewModel: public QAbstractTableModel
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
	explicit ProgressViewModel(QObject* _parent);
	virtual int rowCount(const QModelIndex& _parent) const override;
	virtual int columnCount(const QModelIndex& _parent) const override;
	virtual QVariant data(const QModelIndex& _index, int _role) const override;
	virtual QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const override;
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

class ProgressView: public QWidget
{
	Q_OBJECT
	
private:
	QGridLayout mainLayout;

	ProgressViewModel model;
	QTableView list;

	ProgressBarDelegate delegate;

public:
	ProgressView(QWidget* _parent);

	ProgressViewModel::Info* const AddItem(std::string _filename, int _progress, std::string _status);
	void SetItem(const ProgressViewModel::Info* _info, int _progress, std::string _status);
	void RemoveItem(const ProgressViewModel::Info* _info);
};
