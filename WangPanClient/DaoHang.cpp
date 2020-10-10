#include "DaoHang.h"

DaoHang::DaoHang(QWidget* _parent):
	QWidget(_parent),
	mainLayout(this),
	menu(this),
	menuSelectedChangeCallback([](int){})
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

	this->connect(&this->menu, &QListWidget::itemSelectionChanged, this, [this]()
	{
		this->menuSelectedChangeCallback(this->menu.currentRow());
	});
}

void DaoHang::OnClicked(QPushButton* _button)
{

}

void DaoHang::AddMenu(QString _text)
{
	this->menu.addItem(_text);
	this->menu.item(this->menu.count() - 1)->setTextAlignment(Qt::AlignHCenter);
}

void DaoHang::MenuSelectedChange(std::function<void(int)> _callback)
{
	this->menuSelectedChangeCallback = _callback;
}
