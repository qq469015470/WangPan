#include "CreateDirectoryDialog.h"

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
