#include "NetWork.h"
#include "FieldDraw.h"
#include <QMessageBox>
#include <QDataStream>
#include <QTime>

#define SIGNATURE	27

NetWork::NetWork()
{
	server = NULL;
	socket = NULL;
}

void NetWork::CreateServer(QString str, int port)
{
	server = new QTcpServer(this);
	server_name = str;

	if( !server->listen(QHostAddress::Any, port)) 
	{
		QMessageBox::critical(0, "Server Error", 
													"Unable to start the server:",
													server->errorString()
													);
		server->close();
		return;
	}
	connect(server, SIGNAL(newConnection()), SLOT(slotNewConnection()));
	emit sentMessage("Server on...");
}

void NetWork::CreateClient(QString name, QString ip, int port)
{
	local_name = name;
	socket = new QTcpSocket(this);
	socket->connectToHost(ip, port);
	connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(clientReadData()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
					SLOT(slotError(QAbstractSocket::SocketError))
					);
	emit sentMessage("Client on...");
}

void NetWork::slotNewConnection()
{
	QTcpSocket * client_socket = server->nextPendingConnection();
	map_clients[client_socket->socketDescriptor()] = client_socket;

	connect(client_socket, SIGNAL(disconnected()), client_socket, SLOT(deleteLater()));
	connect(client_socket, SIGNAL(readyRead()), this,					 SLOT(serverReadData()));

	//sendData("Hello");
	emit sentMessage("New Client connect...");
}

void NetWork::serverReadData() // Server read message
{
	QTcpSocket * client_socket = (QTcpSocket *)sender();
	
	receiveData(client_socket);
}

void NetWork::clientReadData() // Client read message
{
	receiveData(socket);
}

void NetWork::slotError(QAbstractSocket::SocketError)
{
	QString strError = "Error: ";
	emit sentMessage(strError);
}

void NetWork::slotConnected()
{
	emit sentMessage("Client: connected");
	
}

void NetWork::SendPoints(QPoint& p)
{
	QByteArray block = "";

	//emit sentMessage("send...");
	//emit sentMessage(QString::number(p.x()));
	//emit sentMessage(QString::number(p.y()));

	block.append('p');
	block.append(QString::number(p.x()));
	block.append('x');
	block.append(QString::number(p.y()));

	sendData(block);
}

void NetWork::sendData(const QByteArray& data)
{
	NetMessage_t net;

	net.signature = SIGNATURE;
	net.data = EncodePack(data);
	net.length_data = net.data.size();
	net.crc = qChecksum(net.data, net.length_data);

	QByteArray block;
	block.append(net.signature);
	block.append(net.length_data);
	block.append(net.data);
	block.append(net.crc&0xFF);
	block.append((net.crc&0xFF00)>>8);

	if( (server != NULL) && !map_clients.empty())
	{
		foreach(int i, map_clients.keys())
		{
			map_clients[i]->write(block);
		}
	}

	if(socket != NULL)
	{
		socket->write(block);
	}


}	

void NetWork::receiveData(QTcpSocket * socket)
{
	QByteArray buf;

	while( socket->bytesAvailable()) 
	{
		buf = "";
		buf = socket->read(1);
		buffer_block.signature = (quint8)buf[0];

		buf = socket->read(1);
		buffer_block.length_data = (quint8)buf[0];

		buf = socket->read(buffer_block.length_data);
		buffer_block.data = buf;

		buf = socket->read(1);
		buffer_block.crc = buf[0];
		buf = socket->read(1);
		buffer_block.crc += buf[0]<<8;

		if(buffer_block.crc != qChecksum(buffer_block.data, buffer_block.length_data))
			continue;
		else
		{
			buffer_block.data = DecodePack(buffer_block.data);
			if(server != NULL) sendData(buffer_block.data);
			switch(buffer_block.data[0])
			{
				case 'p': 
					QString str = buffer_block.data;
					int i = str.indexOf("x");
					quint16 x = str.mid(1,i-1).toInt();
					quint16 y = str.mid(i+1,str.size()).toInt();
					QPoint p(x,y);
					emit drawPoint(p);
					break;
				//default: break;
			}
		}
	}
}

QByteArray NetWork::EncodePack(const QByteArray& data)
{
	QByteArray pack = data;
	for(int i=0; i< pack.size(); i+=2)
	{
		char ch = pack[i];
		pack[i] = pack[i+1];
		pack[i+1] = ch;
	}
	return pack;
}

QByteArray NetWork::DecodePack(const QByteArray& data)
{
	QByteArray pack = data;
	for(int i=0; i< pack.size(); i+=2)
	{
		char ch = pack[i+1];
		pack[i+1] = pack[i];
		pack[i] = ch;
	}
	return pack;
}
