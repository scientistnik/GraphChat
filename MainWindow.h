#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include <QMainWindow>
#include "FieldDraw.h"
#include "NetWork.h"
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
		~MainWindow();
		
	private:
		FieldDraw *paint;
		NetWork * net;
		QTextEdit * txt;

	signals:
		void createServer(QString, int);
		void createClient(QString, QString, int);
		void blackList(quint8);

	public slots:
		//void txtMessage(const QString&);
		//void txtClear();
		void menuExit();
		void menuBlackList();
		void serverDialog();
		void clientDialog();
};

#endif // __MAIN_WINDOW__
