#include "NetWork.h"
#include "FieldDraw.h"
#include <QMessageBox>
#include <QDataStream>
#include <QTime>

#define SIGNATURE	27

quint8 number;

NetWork::NetWork()
{
	server = NULL;
	socket = NULL;
	black_list << 0;
}

void NetWork::CreateServer(QString str, int port)
{
	server = new QTcpServer(this);
	server_name = str;
	my_number = 1;
	number = 1;
	black_list << 1;

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
	emit sentMessage(QString::number(number) + ". "+server_name);
}

void NetWork::CreateClient(QString name, QString ip, int port)
{
	local_name = name;
	my_number = 0;
	socket = new QTcpSocket(this);
	socket->connectToHost(ip, port);
	connect(socket, SIGNAL(connected()), SLOT(slotConnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(clientReadData()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
					SLOT(slotError(QAbstractSocket::SocketError))
					);
}

void NetWork::slotNewConnection()
{
	QTcpSocket * client_socket = server->nextPendingConnection();
	map_clients[client_socket->socketDescriptor()] = client_socket;

	connect(client_socket, SIGNAL(disconnected()), client_socket, SLOT(deleteLater()));
	connect(client_socket, SIGNAL(readyRead()), this,					 SLOT(serverReadData()));

	updateList();
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
	QByteArray data;
	data.append("N");
	data.append(local_name);
	sendData(data);
}

void NetWork::SendPoints(QPoint& p)
{
	QByteArray block = "";

	block.append('p');
	block.append(my_number);
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
	//emit sentMessage("cod:"+QString::number(net.crc));

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
		//emit sentMessage("dec:"+QString::number(buffer_block.crc));

		if(buffer_block.crc != qChecksum(buffer_block.data, buffer_block.length_data)&&(0))
			continue;
		else
		{
			buffer_block.data = DecodePack(buffer_block.data);
			if(server != NULL) sendData(buffer_block.data);
			switch(buffer_block.data[0])
			{
				case 'p': // Point draw
				{
					QString str = buffer_block.data.mid(2,buffer_block.data.size());

					quint8 client_number = buffer_block.data[1];
					//emit sentMessage(QString::number(client_number));
					if( (black_list.indexOf(client_number) != -1) 
							|| ( client_number == my_number)) break;
					//emit sentMessage(QString::number(my_number));

					int i = str.indexOf("x");
					quint16 x = str.mid(0,i).toInt();
					quint16 y = str.mid(i+1,str.size()).toInt();
					QPoint p(x,y);
					emit drawPoint(p);
					break;
				}
				case 'N': // New Name
				{
					number++;
					if( server !=NULL) 
					{
						clients_name[socket->socketDescriptor()] = 
									buffer_block.data.mid(1,buffer_block.data.size());
						//updateList();
					}
					else
					{
						if(local_name == buffer_block.data.mid(1,buffer_block.data.size()))
							my_number = number;
					}
					emit sentMessage(QString::number(number) + ". "  
													+ buffer_block.data.mid(1,buffer_block.data.size()));
					break;
				}
				case 'C': // Clear
				{
					emit clearMessage();
					number = 0;
					break;
				}
				case 'B': // Debag
				{
					emit sentMessage(buffer_block.data.mid(1,buffer_block.data.size()));
					break;
				}
				default: break;
			}
		}
	}
}

QByteArray NetWork::EncodePack(const QByteArray& data)
{
	QByteArray pack = data;
	for(int i=0; i< pack.size(); i+=2)
	{
		pack[i] = pack[i]+2;
		pack[i+1] = pack[i+1]+5;
	}
	return pack;
}

QByteArray NetWork::DecodePack(const QByteArray& data)
{
	QByteArray pack = data;
	for(int i=0; i< pack.size(); i+=2)
	{
		pack[i] = pack[i]-2;
		pack[i+1] = pack[i+1]-5;
	}
	return pack;
}

void NetWork::updateList()
{
	QByteArray buf;
	buf.append("Clear");
	sendData(buf);
	buf.clear();

	buf.append("N");
	buf.append(server_name);
	sendData(buf);
	buf.clear();

	if( (server != NULL) && !map_clients.empty())
	{
		foreach(int i, clients_name.keys())
		{
			buf.append("N");
			buf.append(clients_name[i]);
			sendData(buf);
			buf.clear();
		}
	}
}

void NetWork::addBlackList(quint8 num)
{
	black_list << num;
	//emit sentMessage(QString::number(num));
}

QList<quint8> NetWork::getBlackList()
{
	return black_list;
}

