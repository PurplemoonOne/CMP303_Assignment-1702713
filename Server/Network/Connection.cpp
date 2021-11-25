#include "Connection.h"
#include <memory>
#include "../Log/ServerLog.h"

sf::Packet& operator <<(sf::Packet& packet, const GameData& data)
{
	return packet << data.time << data.id << data.x << data.y;
}

sf::Packet& operator >>(sf::Packet& packet, GameData& data)
{
	return packet >> data.time >> data.id >> data.x >> data.y;
}

sf::Packet& operator <<(sf::Packet& packet, const AssetData& data)
{
	return packet << data.time << data.type << data.count;
}

sf::Packet& operator >>(sf::Packet& packet, AssetData& data)
{
	return packet >> data.time >> data.type >> data.count;
}

sf::Packet& operator >>(sf::Packet& packet, ChatMSG& data)
{
	return packet >> data.time >> data.message >> data.id >> data.quit;
}

sf::Packet& operator <<(sf::Packet& packet, const ChatMSG& data)
{
	return packet << data.time << data.message << data.id << data.quit;
}

sf::Packet& operator >>(sf::Packet& packet,  ConnectionData& data)
{
	return packet >> data.time >> data.privelage;
}

Connection::Connection(sf::TcpSocket* socket)
	:
	mConnPrivelage(ClientPrivelage::None)
{
	//Create a new tcp socket for the server to talk to the client on.
	mTCPSocket = socket;
	mIPAdress = mTCPSocket->getRemoteAddress();
	mTCPPort = mTCPSocket->getLocalPort();
	mUDPPort = 3333;

	APP_TRACE("Connection on port {0} : IP Address {1}", mTCPPort, mIPAdress.toString());

	if (mUDPSocket.bind(mUDPPort) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not bind socket to port ")
	}
	else
	{
		APP_TRACE("Connection on port {0} has successfully bound a UDP socket to port {1}.", mTCPPort, mUDPPort);
	}
}

void Connection::SendTCP(ChatMSG& message)
{
	sf::Packet packet;

	if (!(packet << message))
	{
		APP_ERROR("Could not pack message.");
		return;
	}

	if (!(mTCPSocket->send(packet)))
	{
		APP_ERROR("Failed to send message.");
	}
}

void Connection::SendUDP(const GameData& gameData, const sf::Uint16 toPort, const sf::IpAddress& toIpAddress)
{
	sf::Packet packet;

	if (!(packet << gameData))
	{
		APP_ERROR("Could not pack game update into packet.");
		return;
	}

	if (mUDPSocket.send(packet, toIpAddress, toPort))
	{
		APP_ERROR("Could not send update to port {0} ", toPort);
	}
}

void Connection::RecieveTCP(AssetData& assetData, ChatMSG& message)
{
	sf::Packet packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Failed to recieve!");
	}

	
	if (!(packet >> message))
	{
		APP_WARNING("Possibly wrong data... Trying again");
	}
	//else if (!(packet >> assetData))
	//{
	//	APP_ERROR("Failed to unpack data.");
	//}
}

void Connection::RecieveUDP(GameData& gameData)
{
	sf::IpAddress ipAddress;
	sf::Uint16 port;
	sf::Packet packet;

	if (mUDPSocket.receive(packet, ipAddress, port) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not retrieve game update.");
	}
	else
	{
		if (!(packet >> gameData))
		{
			APP_ERROR("Could not unpack data.");
		}
	}
}
