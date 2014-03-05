#include "MainWindow.h"
#include <QWidget>
#include <QPainter>
//#include <QTextEdit>
#include <QVBoxLayout>
//#include <QPushButton>
#include <QMessageBox>
#include <QDataStream>
#include <QTime>
//#include <QTextLine>
#include <QMenuBar>
#include <QMenu>


#define SIZE_Y	500
#define SIZE_X	600

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setFixedHeight(SIZE_Y);
	setFixedWidth(SIZE_X);

	paint = new FieldDraw;//(wdt);
	paint->setAutoFillBackground(true);

	connect(paint, SIGNAL(sendPoints(QPoint&)), this, SLOT(SendPoints(QPoint& )));

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
	connect(menu_server, SIGNAL(triggered(QAction*)),SLOT(GoConnectServer()));
	QMenu * menu_client = new QMenu("&Client");
	menu_client->addAction("&Connected");
	connect(menu_client, SIGNAL(triggered(QAction*)),SLOT(GoConnectClient()));
	menu->addMenu(menu_exit);
	menu->addMenu(menu_server);
	menu->addMenu(menu_client);

	setCentralWidget(paint/*central*/);
	setMenuBar(menu);
	//this->setVisible(false);
	//this->setVisible(true);
}

void MainWindow::slotNewConnection()
{
	QTcpSocket * client_socket = server->nextPendingConnection();

	connect(client_socket, SIGNAL(disconnected()), client_socket, SLOT(deleteLater()));
	connect(client_socket, SIGNAL(readyRead()), this,					 SLOT(slotReadClient()));
	sendToClient(client_socket, "Server Response: Connected!");
}

void MainWindow::slotReadClient()
{
	txt->append("slotReadClient");
	QTcpSocket * client_socket = (QTcpSocket *)sender();
	QDataStream in(client_socket);
	in.setVersion(QDataStream::Qt_5_2);
	
	for(;;)
	{
		if(!next_block_size) 
		{
			/*
			if (client_socket->bytesAvailable() < sizeof(quint16))
			{
				break;
			}
			*/
			in >> next_block_size;
		}

		if (client_socket->bytesAvailable() < next_block_size)
		{
			break;
		}
		QTime time;
		QString str;
		in >> time >> str;

		QString strMessage = time.toString() + " " + "Client has set - " + str;
		txt->append(strMessage);
		next_block_size = 0;

		sendToClient(client_socket, "Server Response: Received\"" + str + "\"");
	}
}

void MainWindow::sendToClient(QTcpSocket * socket, const QString& str)
{
	QByteArray block;
	QDataStream out(&block,QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_2);
	out << quint16(0) << QTime::currentTime() << str;

	out.device()->seek(0);
	out << quint16(block.size() - sizeof(quint16));

	socket->write(block);
}

void MainWindow::slotReadyRead()
{
	txt->append("slotReadyRead");
	QDataStream in(socket);
	in.setVersion(QDataStream::Qt_5_2);
	for (;;)
	{
		if (!next_block_size)
		{
			/*
			if (socket->bytesAvailable() < sizeof(quint16))
			{
				break;
			}
			*/
			in >> next_block_size;
		}
		if (socket->bytesAvailable() < next_block_size)
		{
			break;
		}
		QTime time;
		QString str;
		in >> time >> str;

		txt->append(time.toString() + " " + str);
		next_block_size = 0;
	}
}

void MainWindow::slotError(QAbstractSocket::SocketError)
{
	QString strError = 
					"Error: ";
	txt->append(strError);
}

void MainWindow::slotSendToServer()
{
	txt->append("slotSendToServer");
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_5_2);
	out << quint16(0) << QTime::currentTime();

	out.device()->seek(0);
	out << quint16(block.size() - sizeof(quint16));

	socket->write(block);
}

void MainWindow::slotConnected()
{
	txt->append("Received the connected() signal");
}

void MainWindow::GoConnectClient()
{
	socket = new QTcpSocket(this);
	socket->connectToHost("localhost", 2323);
	connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
					SLOT(slotError(QAbstractSocket::SocketError))
					);
	//connect(menu_client, SIGNAL(triggered(QAction*)), SLOT(slotSendToServer()));
	//connect(cmd, SIGNAL(cliced()), SLOT(slotSendToServer()));
	//slotSendToServer();
	txt->append("Client on...");
}

void MainWindow::GoConnectServer()
{
	server = new QTcpServer(this);

	if( !server->listen(QHostAddress::Any, 2323)) 
	{
		QMessageBox::critical(0, "Server Error", 
													"Unable to start the server:",
													server->errorString()
													);
		server->close();
		return;
	}
	connect(server, SIGNAL(newConnection()), SLOT(slotNewConnection()));
	txt->append("Server on...");
}

void MainWindow::SendPoints(QPoint&)
{
	txt->append("heeelo");
	slotSendToServer();
}
