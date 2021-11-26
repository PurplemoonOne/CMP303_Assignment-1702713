#include "Connection.h"
#include <memory>
#include "../Log/ServerLog.h"

sf::Packet& operator <<(sf::Packet& packet, const GameData& data)
{
	return packet << data.time << data.objectID << data.x << data.y;
}

sf::Packet& operator >>(sf::Packet& packet, GameData& data)
{
	return packet >> data.time >> data.objectID >> data.x >> data.y;
}

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
	return packet >> data.time >> data.privelage >> data.UdpPort >> data.type >> data.count >> data.sizeX >> data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectionData& data)
{
	return packet << data.time << data.privelage << data.UdpPort << data.type << data.count << data.sizeX << data.sizeY;
}

sf::Packet& operator >>(sf::Packet& packet, ClientPortAndIP& data)
{
	return packet >> data.udpPort >> data.ip;
}

sf::Packet& operator <<(sf::Packet& packet, const ClientPortAndIP& data)
{
	return packet << data.udpPort << data.ip;
}


Connection::Connection(sf::TcpSocket* socket)
	:
	mConnPrivelage(ClientPrivelage::None)
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

	if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Failed to send message.");
	}
}

bool Connection::SendTCP(ConnectionData& message)
{
	sf::Packet packet;

	if (!(packet << message))
	{
		APP_ERROR("Could not pack assets message.");
		return false;
	}

	if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("TCP send failed : SendTCP() ~ ConnectionData& ");
		return false;
	}
	
	APP_TRACE("Sent asset descriptions to clients.");
	return true;
}

void Connection::SendTCP(ClientPortAndIP& data)
{
	sf::Packet packet;

	if (!(packet << data))
	{
		APP_ERROR("TCP packing failed : SendTCP() ~ ClientPortAndIP& ");
		return;
	}

	if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("TCP send failed : SendTCP() ~ ClientPortAndIP& ");
	}
}

void Connection::RecieveTCP(DisconnectPCKT& message)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("TCP recieve failed : RecieveTCP() ~ DisconnectPCKT& ");
	}
	
	if (!(packet >> message))
	{
		APP_ERROR("TCP packing failed : RecieveTCP() ~ DisconnectPCKT& ");
	}

	APP_TRACE("Connection has requested to quit.");
}

void Connection::RecieveTCP(ConnectionData& data)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("TCP recieve failed : RecieveTCP() ~ ConnectionData& ");
	}

	if (!(packet >> data))
	{
		APP_WARNING("Unpacking connection data failed...");
	}

	APP_TRACE("Successfully recieved connection data.");
}

void Connection::RecieveTCP(ClientPortAndIP& data)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Failed to recieve!");
	}

	if (!(sizeof(packet) == sizeof(ClientPortAndIP)))
	{
		APP_ERROR("");
	}

	if (!(packet >> data))
	{
		APP_WARNING("Unpacking connection data failed...");
	}
}

