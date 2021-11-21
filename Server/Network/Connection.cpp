#include "Connection.h"
#include <memory>
#include "../Log/Log.h"

Connection::Connection(sf::TcpSocket* socket)
{
	//Create a new tcp socket for the server to talk to the client on.
	mTCPSocket = socket;
	mIPAdress = mTCPSocket->getRemoteAddress();
	mPort = mTCPSocket->getLocalPort();
	APP_TRACE("Connection on port {0} : IP Address {1}", mPort, mIPAdress.toString());
}


void Connection::Send()
{
	GamePacket packet;

	if (mTCPSocket->send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not send data to connection!");
	}
}

AssetList Connection::RecieveAssets()
{
	AssetPacket packet;

	if (mTCPSocket->receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not recieve the asset list from the client.");
	}




}

void Connection::Recieve()
{
}
