#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include <QMainWindow>
#include "FieldDraw.h"

class MainWindow: public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
	private:
		FieldDraw *paint;
		//QWidget *wgt;
		//QPushButton *button1;
		//QPushButton *button2;
		
};

#endif // __MAIN_WINDOW__
