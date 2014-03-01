#include "MainWindow.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	//QMainWindow::SetWindowTitle("Example");
	resize(500, 50);

	//moveWindowCenter();

	QWidget* wgt = new QWidget();
	QPushButton* cmdA = new QPushButton("A");
	QPushButton* cmdB = new QPushButton("B");
	
	QBoxLayout* boxLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	boxLayout->addWidget(cmdA,1);
	boxLayout->addWidget(cmdB,2);
	wgt->setLayout(boxLayout);
	
	QWidget *wgt1 = new QWidget();
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->addWidget(wgt,1);
	QPixmap *pixmap = new QPixmap();
	layout->addWidget(pixmap,2);
	wgt1->setLayout(layout);

	setCentralWidget(wgt1);

	this->setVisible(false);
	this->setVisible(true);
}
