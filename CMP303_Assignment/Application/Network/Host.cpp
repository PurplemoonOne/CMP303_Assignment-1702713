#include "Host.h"
#include <iostream>

Host::Host()
	:
	mLatency(0.f),
	mJitter(0.f)
{
	// Fetch the machines IP Address.
	mIPAdress = sf::IpAddress::getLocalAddress();
	mPrivelage = Privelage::Host;
	mPort = 5555;

	if (mSocket.bind(mPort, mIPAdress) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not bind socket to port {0} : IP Address {1}", mPort, mIPAdress.toString());
	}
	else
	{
		mSelect.add(mSocket);
	}
}

void Host::SearchForConnections()
{
	while (mConnections.size() < 1)
	{
		sf::IpAddress ipAddress;
		uint16_t port;
		Packet packet;
		size_t sizeOfPacket = sizeof(Packet);
		size_t recievedSize;
		if (mSocket.receive((void*)&packet, sizeOfPacket, recievedSize, ipAddress, port) != sf::UdpSocket::Done)
		{
			APP_ERROR("Socket failed to recieve anything");
		}

		Client* client = new Client();
		client->SetPort(port);
		client->SetIPAddress(ipAddress);
		client->setHostPort(mPort);
		client->SetHostIPAddress(mIPAdress);

		for (auto& connection : mConnections)
		{
			if (connection->GetLocalPort() == client->GetLocalPort())
			{
				APP_TRACE("Client already connected to game.");
				break;
			}
			mSelect.add(client->GetSocket());
			mConnections.push_back(client);
		}
	}
}
