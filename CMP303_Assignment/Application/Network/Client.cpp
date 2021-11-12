#include "Client.h"


Client::Client()
{
	// Fetch the machines IP Address.
	mIPAdress ="127.0.0.1";
	mPrivelage = Privelage::Client;
	mPort = 4444;
	mSocket.bind(mPort, mIPAdress);
}

Client::~Client()
{
	
}

void Client::SendPacket(std::pair<float, float> position, const float timeStamp, const int32_t id, const uint16_t port)
{
	Packet packet;
	packet.id = id;
	packet.time = timeStamp;
	packet.x = position.first;
	packet.y = position.second;

	mHostIpAddress = "127.0.0.1";
	mHostPort = 5555;

	if (mSocket.send((const void*)&packet, sizeof(Packet), mHostIpAddress, mHostPort) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not send packet to server w/ port {0} : IP Address {1}", mHostPort, mHostIpAddress);
	}

}

void Client::RecievePacket()
{
	Packet packet;
	size_t sizeOfPacket = sizeof(Packet);
	size_t recievedSize;

	//Do not block app, return right away.
	mSocket.setBlocking(false);

	if (mSocket.receive((void*)&packet, sizeOfPacket, recievedSize, mHostIpAddress, mHostPort) != sf::UdpSocket::Done)
	{
		APP_ERROR("Did not recieve a packet from the server...")
	}

	mMessages.push_back(packet);
}


