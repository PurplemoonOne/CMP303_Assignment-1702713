#include "Connection.h"
#include <memory>
#include "../Log/ServerLog.h"



sf::Packet& operator >>(sf::Packet& packet, DisconnectPCKT& data)
{
	return packet >> data.time >> data.message >> data.id >> data.quit;
}

sf::Packet& operator <<(sf::Packet& packet, const DisconnectPCKT& data)
{
	return packet << data.time << data.message << data.id << data.quit;
}

sf::Packet& operator >>(sf::Packet& packet,  ConnectionData& data)
{
	return packet >> data.time >> data.privelage >> data.peerUdpRecvPort >> data.ipAddress >> data.type >> data.count >> data.sizeX >> data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectionData& data)
{
	return packet << data.time << data.privelage << data.peerUdpRecvPort << data.ipAddress << data.type << data.count << data.sizeX << data.sizeY;
}

sf::Packet& operator >>(sf::Packet& packet, ClientPortAndIP& data)
{
	return packet >> data.udpPort;
}

sf::Packet& operator <<(sf::Packet& packet, const ClientPortAndIP& data)
{
	return packet << data.udpPort;
}


Connection::Connection(sf::TcpSocket* socket)
{
	//Create a new tcp socket for the server to talk to the client on.
	mTCPSocket = socket;
	mIPAdress = mTCPSocket->getRemoteAddress();
	mTCPPort = mTCPSocket->getLocalPort();
}

void Connection::SendTCP(DisconnectPCKT& message)
{
	sf::Packet packet;

	if (!(packet << message))
	{
		APP_ERROR("Could not pack message.");
		return;
	}
	else
	{
		if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("Failed to send message.");
		}
		else
		{
			APP_TRACE("Successfully send disconnect message.");
		}
	}
}

bool Connection::SendAssets(ConnectionData& message)
{
	sf::Packet packet;
	bool status = false;

	if (!(packet << message))
	{
		APP_ERROR("Could not pack assets packet.");
	}
	else
	{
		if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("TCP send failed : SendTCP() ~ ConnectionData& ");
		}
		else
		{
			APP_TRACE("Successfully send peers asset description.");
			status = true;
		}
	}

	return status;
}

void Connection::SendTCP(ClientPortAndIP& data)
{
	sf::Packet packet;

	if (!(packet << data))
	{
		APP_ERROR("TCP packing failed : SendTCP() ~ ClientPortAndIP& ");
		return;
	}
	else
	{
		if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("TCP send failed : SendTCP() ~ ClientPortAndIP& ");
		}
		else
		{
			APP_TRACE("Successfully sent the peers port number and IP address.");
		}
	}
}

void Connection::RecieveTCP(DisconnectPCKT& message)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("TCP recieve failed : RecieveTCP() ~ DisconnectPCKT& ");

	}
	else
	{
		if (!(packet >> message))
		{
			APP_ERROR("TCP packing failed : RecieveTCP() ~ DisconnectPCKT& ");
		}
		else
		{
			APP_TRACE("Connection has requested to quit.");
		}
	}
}

void Connection::RecieveAssets(ConnectionData& data)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("TCP recieve failed : RecieveTCP() ~ ConnectionData& ");
	}
	else
	{
		if (!(packet >> data))
		{
			APP_ERROR("Unpacking connection data failed...");
		}
		else
		{
			APP_TRACE("Successfully recieved connection data.");
		}
	}
}

void Connection::RecieveTCP(ClientPortAndIP& data)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Failed to recieve!");
	}
	else
	{
		if (!(packet >> data))
		{
			APP_WARNING("Unpacking connection data failed...");
		}
		else
		{
			APP_TRACE("Successfully recieved a TCP packet from the server.")
		}
	}
}

