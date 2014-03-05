#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include <QMainWindow>
#include "FieldDraw.h"
#include <QTextEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>

class QTextEdit;
class QTcpServer;
class QTcpSocket;

class MainWindow: public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
	private:
		FieldDraw *paint;
		QTextEdit * txt;
		QTcpServer * server;
		QTcpSocket * socket;
		quint16	next_block_size;

		void sendToClient(QTcpSocket* socket, const QString& str);
	public slots:
		void slotNewConnection();
		void slotReadClient();

		void SendPoints(QPoint&);
		
	private slots:
		void slotReadyRead();
		void slotError(QAbstractSocket::SocketError);
		void slotSendToServer();
		void slotConnected();

		void GoConnectClient();
		void GoConnectServer();
};

#endif // __MAIN_WINDOW__
