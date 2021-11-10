#include "Network.h"
#include "../Log/Log.h"

Connection::Connection(const sf::IpAddress& ipAdress)
	: mIPAdress(ipAdress),
	mSocket(),
	mPort(0),
	mLatency(0.f),
	mJitter(0.f)
{
}

bool Connection::CreateUDPSocket(uint16_t lPort)
{
	// Create sockets and bind port numbers not used by any major application.
	if (mPort > 0)
	{
		return true;
	}

	mPort = lPort;

	if (mSocket.bind(mPort, mIPAdress) != sf::Socket::Done)
	{
		//Can't bind port number, so we can request sfml to grab any port
		//number that is not bound to a socket.
		APP_ERROR("Could not bind socket to port {0}", mPort);
		APP_TRACE("Fetching new port number...");
		if (mSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		{
			//In case something really wrong happens we kill any attempt
			//to connect to a peer and return to the main menu.
			APP_ERROR("Something went wrong, not ports are available...");
		}
	}

	mSocket.setBlocking(false);
	select.add(mSocket);
}

void Connection::CloseSocket()
{
	mSocket.unbind();
}

void Connection::SendPacket(const float positionX, const float positionY, const float timeStamp, const int32_t id, const uint16_t port)
{
	Packet packet;

	packet.id = id;
	packet.time = timeStamp;
	packet.x = positionX;
	packet.y = positionY;

 	SendPacket(packet, port);
}

void Connection::SendPacket(const Packet& packet, uint16_t port)
{
	if (mSocket.send((const void*)&packet, size_t(sizeof(Packet)), mIPAdress, port) != sf::UdpSocket::Done)
	{
		APP_ERROR("Failed to send data to port {0}...", port);
		APP_TRACE("Attempting a re-send....");
	}
}

void Connection::ListenForPacket(uint16_t port)
{
	Packet recvPacket;
	size_t recvSize;

	if (select.isReady(mSocket))
	{
		if (mSocket.receive((void*)&recvPacket, size_t(sizeof(Packet)), recvSize, mIPAdress, mPort) != sf::UdpSocket::Done)
		{
			APP_ERROR("Failed to recvieve data from port {0}...", 4444);
		}

		mMessages.push_back(recvPacket);
	}
}

bool Connection::EvaluateSockets()
{

}
