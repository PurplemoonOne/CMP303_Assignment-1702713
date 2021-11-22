#include "Client.h"

const sf::IpAddress SERVER_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;

sf::Packet& operator <<(sf::Packet& packet, const GameData& data)
{
	return packet << data.appElapsedTime << data.id << data.x << data.y;
}

sf::Packet& operator <<(sf::Packet& packet, const AssetData& data)
{
	return packet << data.type << data.count;
}

sf::Packet& operator <<(sf::Packet& packet, const ChatMSG& data)
{
	return packet << data.message << data.timeStamp << data.id << data.quit;
}

Client::Client()
{
	// Fetch the machines IP Address. 
	mIPAdress = sf::IpAddress::getLocalAddress();
	mUDPPort = 4444;

	//Bind the UDP socket to an available port.
	if (mUDPSocket.bind(mUDPPort) != sf::UdpSocket::Done)
	{
		APP_TRACE("Could not bind to port {0}, fetching an unused port.", mUDPPort);
		//Try again using any port which is available, SFML will find an unused port.	
		if (mUDPSocket.bind((mUDPPort = sf::UdpSocket::AnyPort)) != sf::UdpSocket::Done)
		{
			APP_ERROR("We're sorry, unfortunately there are no available ports on this machine.");
		}

		APP_TRACE("Bound UDP socket to port {0}", mUDPPort);
	}
	else
	{
		APP_TRACE("Bound UDP socket to port {0}", mUDPPort);
	}

	
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

	mTCPSocket.setBlocking(false);
	mUDPSocket.setBlocking(false);
}

void Client::SendPacket(std::pair<float, float> position, const float timeStamp, const sf::Uint32 id)
{
	GameData data;
	data.appElapsedTime = timeStamp;
	data.id = id;
	data.x = position.first;
	data.y = position.second;

	GamePacket packet;
	if (!(packet << data))
	{
		APP_ERROR("Error! Could not append data to packet!");
		return;
	}
	else
	{
		APP_TRACE("Successfully packed data.");
	}

	sf::IpAddress ipAddress = sf::IpAddress::getLocalAddress();
	sf::Uint16 port = 3333;

	if (mUDPSocket.send(packet, ipAddress, port) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not send packet to server!");
	}

}

void Client::RecievePacket()
{
	GamePacket packet;

	//Variables to write the port and ip address of the sender.
	sf::IpAddress serverIPAddress;
	uint16_t serverPort;

	mUDPSocket.setBlocking(false);
	if (mUDPSocket.receive(packet, serverIPAddress, serverPort) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not send packet to server!");
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
		packet << item;
	}

	if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not send asset data to the server!");
	}
}

void Client::Disconnect(const float appElapsedTime)
{
	ChatMSG data;
	sf::Packet packet;

	data.id = 0;
	data.quit = 1;
	data.message = " ";
	data.timeStamp = appElapsedTime;

	packet << data;

	if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not disconnect from server... Try again.");
	}
}


