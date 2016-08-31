#include "MXMhsX.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CMhsX w;
	w.show();
	return a.exec();
}
