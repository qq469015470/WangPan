#include "UploadView.h"

#include <QHeaderView>
#include <QApplication>
#include <iostream>

UploadViewModel::UploadViewModel(QObject* _parent):
	QAbstractTableModel(_parent)	
{

}

int UploadViewModel::rowCount(const QModelIndex& _parent) const
{
	return this->values.size();
}

int UploadViewModel::columnCount(const QModelIndex& _parent) const
{
	return 3;
}

QVariant UploadViewModel::data(const QModelIndex& _index, int _role) const
{
	if(_role == Qt::DisplayRole)
	{
		const auto& item = this->values.at(_index.row());
		switch(_index.column())
		{
			case 0:
				return QString(item.filename.c_str());
				break;
			case 1:
				return QVariant(item.progress);
				break;
			case 2:
				return QString(item.status.c_str());
				break;
		}
	}

	return QVariant();
}

const std::vector<UploadViewModel::Info>& UploadViewModel::getData() const
{
	return this->values;
}

void UploadViewModel::setData(std::vector<Info> _values)
{
	this->beginInsertRows(QModelIndex(), 0, _values.size());

	this->values = _values;

	this->endInsertRows();
}

ProgressBarDelegate::ProgressBarDelegate(QObject* _parent):
	QStyledItemDelegate(_parent)
{

}

void ProgressBarDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	if(_index.column() == 1) 
	{
        	int value = _index.model()->data(_index).toInt();
        	QStyleOptionProgressBar progressBarOption;
        	progressBarOption.rect = _option.rect.adjusted(4, 4, -4, -4);
        	progressBarOption.minimum = 0;
        	progressBarOption.maximum = 100;
        	progressBarOption.textAlignment = Qt::AlignRight;
        	progressBarOption.textVisible = true;
        	progressBarOption.progress = value;
        	progressBarOption.text = tr("%1%").arg(progressBarOption.progress);

        	_painter->save();
        	if (_option.state & QStyle::State_Selected) 
		{
        		_painter->fillRect(_option.rect, _option.palette.highlight());
        		_painter->setBrush(_option.palette.highlightedText());
        	}
        	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, _painter);

        	_painter->restore();
    	}
       	else 
	{
		QStyleOptionViewItem itemOption(_option);
		if(itemOption.state & QStyle::State_HasFocus)
		{
		    itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
		}
		QStyledItemDelegate::paint(_painter, itemOption, _index);
	}
}

UploadView::UploadView(QWidget* _parent):
	QWidget(_parent),
	mainLayout(this),
	model(this),
	list(this),
	delegate(this)
{
	this->mainLayout.addWidget(&this->list, 0, 0);
	//this->mainLayout.addWidget(&this->progressBar, 1, 0);
	this->mainLayout.setContentsMargins(0, 0, 0, 0);

	//this->model.setColumnCount(3);

	this->list.setColumnWidth(0, 200);
	this->list.setModel(&this->model);

	this->list.setShowGrid(false);
	this->list.verticalHeader()->hide();
	this->list.horizontalHeader()->hide();
	this->list.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->list.horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	this->list.horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	//this->list.setSelectionMode(QAbstractItemView::NoSelection);
	this->list.setSelectionBehavior(QAbstractItemView::SelectRows);
	//this->list.horizontalHeader()->setStretchLastSection(true);
	//this->list.verticalHeader()->setDefaultSectionSize(100);

	//this->model.setHeaderData(0,Qt::Horizontal,QStringLiteral("序号"));   //设置表头内容
	//this->model.setHeaderData(1,Qt::Horizontal,QStringLiteral("名称"));
	//this->model.setHeaderData(2,Qt::Horizontal,QStringLiteral("地址"));
	//
	//this->list.resizeColumnToContents(0);

	//this->list.setIndexWidget(this->model.index(0, 1), &this->progressBar);

	this->list.setItemDelegate(&this->delegate);
}

size_t UploadView::AddItem(std::string _filename, int _progress)
{
	auto temp = this->model.getData();
	temp.push_back({_filename, _progress, "-"});
	this->model.setData(std::move(temp));

	return temp.size() - 1;
}

void UploadView::SetItemProgress(int _index, int _progress)
{
	auto temp = this->model.getData();
	temp.at(_index).progress = _progress;
	this->model.setData(std::move(temp));
}

void UploadView::RemoveItem(int _index)
{
	auto temp = this->model.getData();
	temp.erase(temp.begin() + _index);
	this->model.setData(std::move(temp));
}
