#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class CreateDirectoryDialog: public QDialog
{
	Q_OBJECT

private:
	QGridLayout mainLayout;
	QFormLayout formLayout;

	QLabel dirLabel;
	QLineEdit dirEdit;
	QPushButton addBtn;
	QPushButton celBtn;

public:
	CreateDirectoryDialog(QWidget* _parent);

	void CreateDirectory();
	QLineEdit& GetDirectoryEdit();
};
