#include <QtWidgets/QApplication>
#include "MainWindow.h"

int main (int argc, char *argv[])
{
	QApplication app(argc,argv);
	MainWindow a;
	a.show();

	return app.exec();
}
