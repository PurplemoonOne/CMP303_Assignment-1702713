#include "Client.h"

const sf::IpAddress SERVER_IP = sf::IpAddress::getLocalAddress();
const uint16_t SERVER_PORT = 5555;

Client::Client()
{
	// Fetch the machines IP Address. 
	mIPAdress = sf::IpAddress::getLocalAddress();
	mPort = 4444;

	//Bind the UDP socket to an available port.
	if (mUDPSocket.bind(mPort, mIPAdress) != sf::UdpSocket::Done)
	{
		APP_TRACE("Could not bind to port {0}, fetching an unused port.", mPort);
		//Try again using any port which is available, SFML will find an unused port.	
		if (mUDPSocket.bind((mPort = sf::UdpSocket::AnyPort), mIPAdress) != sf::UdpSocket::Done)
		{
			APP_ERROR("We're sorry, unfortunately there are no available ports on this machine.");
		}

		APP_TRACE("Bound UDP socket to port {0}", mPort);
	}

	mSelect.add(mUDPSocket);
}

Client::~Client()
{

}

void Client::ConnectToServer()
{
	//Establish connection with the server.
	if (mTCPSocket.connect(sf::IpAddress::getLocalAddress(), SERVER_PORT) != sf::TcpSocket::Done)
	{
		APP_ERROR("Connection failed!");
	}
}

void Client::SendPacket(std::pair<float, float> position, const float timeStamp)
{
	GamePacket packet;

	//Tell the server whether the client is spectating or streaming.
	packet.append((const void*)mPrivelage, sizeof(mPrivelage));

	if (mSelect.wait(sf::microseconds(16.0f)))
	{
		if (mSelect.isReady(mUDPSocket))
		{
			if (mUDPSocket.send(packet, SERVER_IP, SERVER_PORT) != sf::UdpSocket::Done)
			{
				APP_ERROR("Could not send packet to server!");
			}
		}
	}
	else
	{
		APP_TRACE("Client UDP socket not ready to send data.");
	}
}

void Client::RecievePacket()
{
	GamePacket packet;

	if (mSelect.wait(sf::microseconds(16.0f)))
	{
		if (mSelect.isReady(mUDPSocket))
		{
			//Variables to write the port and ip address of the sender.
			sf::IpAddress serverIPAddress;
			uint16_t serverPort;

			if (mUDPSocket.receive(packet, serverIPAddress, serverPort) != sf::UdpSocket::Done)
			{
				APP_ERROR("Could not send packet to server!");
			}
		}
	}
	else
	{
		APP_TRACE("Client UDP socket not ready to recieve data.");
	}
}

void Client::SendAssetsToServer(AssetList assetList)
{
	AssetPacket packet;

	//Tell the server whether the client is spectating or streaming.
	packet.append((const void*)mPrivelage, sizeof(mPrivelage));

	//Upload the asset list so the server can replicate the game.
	//In addition to inform other clients on simulating the game.
	for (auto& item : assetList)
	{
		packet.append((const void*)&item, sizeof(item));
	}

	if (mSelect.wait(sf::microseconds(16.0f)))
	{
		if (mSelect.isReady(mTCPSocket))
		{
			if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
			{
				APP_ERROR("Could not send asset data to the server!");
			}
		}
	}
}

void Client::SetClientPrivelage(ClientPrivelage privelage)
{
	mPrivelage = privelage;
}

void Client::Disconnect()
{

}


