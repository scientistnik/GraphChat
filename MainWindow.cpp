#include "MainWindow.h"
#include <QWidget>
#include <QPainter>
#include <QMenuBar>
#include <QMenu>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>

#define SIZE_Y	500
#define SIZE_X	600

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setFixedHeight(SIZE_Y);
	setFixedWidth(SIZE_X);

	paint = new FieldDraw(this);
	paint->setAutoFillBackground(true);

	net = new NetWork;
	connect(net, SIGNAL(sentMessage(const QString&)),this, SLOT(txtMessage(const QString&)));

	connect(net, SIGNAL(drawPoint(QPoint&)), paint, SLOT(slotAddPoint(QPoint&)));
	connect(paint, SIGNAL(sendPoints(QPoint&)), net, SLOT(SendPoints(QPoint& )));

	txt = new QTextEdit(paint);
	txt->setReadOnly(true);
	txt->move(SIZE_X*80/100,0);
	txt->setFixedWidth(SIZE_X*20/100);
	txt->setFixedHeight(SIZE_Y-90);

	QMenuBar * menu = new QMenuBar;
	QMenu * menu_exit = new QMenu("&Menu");
	menu_exit->addAction("&EXIT");
	connect(menu_exit, SIGNAL(triggered(QAction*)), SLOT(close()));

	QMenu * menu_server = new QMenu("&Server");
	menu_server->addAction("&Create");
	connect(menu_server, SIGNAL(triggered(QAction*)), this, SLOT(serverDialog()));
	connect(this, SIGNAL(createServer(QString, int)), net,SLOT(CreateServer(QString, int)));

	QMenu * menu_client = new QMenu("&Client");
	menu_client->addAction("&Connected");
	connect(menu_client, SIGNAL(triggered(QAction*)), this,SLOT(clientDialog()));
	connect(this, SIGNAL(createClient(QString, QString, int)), net,SLOT(CreateClient(QString, QString, int)));

	menu->addMenu(menu_exit);
	menu->addMenu(menu_server);
	menu->addMenu(menu_client);

	setCentralWidget(paint);
	setMenuBar(menu);
	//this->setVisible(false);
	//this->setVisible(true);
}

MainWindow::~MainWindow()
{
	delete net;
}

void MainWindow::txtMessage(const QString& str)
{
	txt->append(str);
}

void MainWindow::serverDialog()
{
	QDialog * dialog = new QDialog(this);
	QLineEdit * name = new QLineEdit;
	QLineEdit * port = new QLineEdit;
	QPushButton * ok = new QPushButton("&Ok");
	QPushButton * cancel = new QPushButton("&Cancel");
	QLabel * txt_name = new QLabel("&Name");
	QLabel * txt_port = new QLabel("&Port");
	QGridLayout * dialog_lay = new QGridLayout;

	connect(ok, SIGNAL(clicked()), dialog, SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), dialog, SLOT(reject()));

	dialog_lay->addWidget(txt_name, 0,0);
	dialog_lay->addWidget(txt_port, 1,0);
	dialog_lay->addWidget(name, 0,1);
	dialog_lay->addWidget(port, 1,1);
	dialog_lay->addWidget(ok,2,0);
	dialog_lay->addWidget(cancel,2,1);
	dialog->setLayout(dialog_lay);

	if(dialog->exec() == QDialog::Accepted)
	{
		emit createServer(name->text(), port->text().toInt());
	}
	delete dialog;
}

void MainWindow::clientDialog()
{
	QDialog * dialog = new QDialog(this);
	QLineEdit * name = new QLineEdit;
	QLineEdit * ip = new QLineEdit;
	QLineEdit * port = new QLineEdit;
	QPushButton * ok = new QPushButton("&Ok");
	QPushButton * cancel = new QPushButton("&Cancel");
	QLabel * txt_name = new QLabel("&Name");
	QLabel * txt_ip = new QLabel("&IP");
	QLabel * txt_port = new QLabel("&Port");
	QGridLayout * dialog_lay = new QGridLayout;

	connect(ok, SIGNAL(clicked()), dialog, SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), dialog, SLOT(reject()));

	dialog_lay->addWidget(txt_name, 0,0);
	dialog_lay->addWidget(txt_ip, 1,0);
	dialog_lay->addWidget(txt_port, 2,0);
	dialog_lay->addWidget(name, 0,1);
	dialog_lay->addWidget(ip, 1,1);
	dialog_lay->addWidget(port, 2,1);
	dialog_lay->addWidget(ok,3,0);
	dialog_lay->addWidget(cancel,3,1);
	dialog->setLayout(dialog_lay);

	if(dialog->exec() == QDialog::Accepted)
	{
		emit createClient(name->text(),ip->text(), port->text().toInt());
	}
	delete dialog;
}
