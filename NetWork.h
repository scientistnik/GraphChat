#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>

typedef struct NetMessage {
	quint8 signature;
	quint8 length_data;
	QByteArray data;
	quint16 crc;
}NetMessage_t;

class NetWork : public QObject
{
	Q_OBJECT

public:
	NetWork();
	QList<quint8> getBlackList();

private:
	QTcpSocket * socket;
	QTcpServer * server;
	QMap<int,QTcpSocket *> map_clients;
	QMap<int,QString> clients_name;
	quint8 my_number;
	QList<quint8> black_list;

	quint16	next_block_size;
	NetMessage_t buffer_block;
	QString msg;
	QString server_name;
	QString local_name;

	QByteArray EncodePack(const QByteArray&);
	QByteArray DecodePack(const QByteArray&);
	void sendData(const QByteArray&);
	void receiveData(QTcpSocket *);
	void updateList();

signals:
	void sentMessage(const QString&);
	void drawPoint(QPoint&);
	void clearMessage();

public slots:
	void SendPoints(QPoint&);
	void slotNewConnection();
	void serverReadData();

	void CreateServer(QString, int);
	void CreateClient(QString, QString, int);
	void addBlackList(quint8);

private slots:
	void clientReadData();
	void slotError(QAbstractSocket::SocketError);
	void slotConnected();
};

#endif // __NETWORK_H__
