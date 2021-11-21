#include "Connection.h"
#include <memory>

Connection::Connection(sf::TcpSocket& socket)
{
	//Create a new tcp socket for the server to talk to the client on.
	mTCPSocket.reset(&socket);
	mIPAdress = mTCPSocket->getRemoteAddress();
	mPort = mTCPSocket->getLocalPort();
}

void Connection::Send(uint16_t hostPort, uint16_t networkID)
{
	size_t packetSize = sizeof(InstructionPacket);
	InstructionPacket ins;
	ins.hostUdpPort = hostPort;
	ins.networkID = mNetworkID = networkID;
	if (mTCPSocket->send((void*)&ins, packetSize) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not send initialisation message to client {0}");
	}


}
