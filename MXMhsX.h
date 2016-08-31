#ifndef MXMHSX_H
#define MXMHSX_H

#include <QtWidgets/QMainWindow>
#include "ui_MXMhsX.h"

class CMhsX : public QMainWindow
{
	Q_OBJECT

public:
	CMhsX(QWidget *parent = 0);
	~CMhsX();

private:
	Ui::CMhsXClass ui;
};

#endif // MXMHSX_H
