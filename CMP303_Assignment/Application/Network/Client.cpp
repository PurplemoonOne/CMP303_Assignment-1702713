#include "Client.h"

const sf::IpAddress SERVER_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;

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

sf::Packet& operator <<(sf::Packet& packet, const ChatMSG& data)
{
	return packet << data.time << data.message << data.id << data.quit;
}

sf::Packet& operator >>(sf::Packet& packet, ChatMSG& data)
{
	return packet >> data.time >> data.message >> data.id >> data.quit;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectionData& data)
{
	return packet << data.time << data.privelage;
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

	sf::Packet packet;
	ConnectionData data;
	data.time = time(0);

	if (mPrivelage == ClientPrivelage::Stream)
	{
		data.privelage = 0;
		if (packet << data)
		{
			APP_ERROR("Streamer : Failed to pack connection data.");
		}

		if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("Unable to send TCP connection data.");
		}
	}
	else 
	{
		data.privelage = 1;
		if (packet << data)
		{
			APP_ERROR("Spectator : Failed to pack connection data.");
		}

		if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("Unable to send TCP connection data.");
		}
	}

	mTCPSocket.setBlocking(false);
	mUDPSocket.setBlocking(false);
}

void Client::SendGamePacket(std::pair<float, float> position)
{
	GameData data;
	data.x = position.first;
	data.y = position.second;

	time_t t = time(0);
	data.time = (double)t;

	sf::Packet packet;
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

bool Client::SendChatMessage(const sf::Text& message)
{
	ChatMSG buffer; 
	sf::Packet packet;

	buffer.message = message.getString();
	buffer.id = 0; 
	time_t t = time(0);
	buffer.time = (double)t;
	bool status = true;

	if (!(packet << buffer))
	{
		APP_ERROR("Could not pack chat message!");
		status = false;
	}

	if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not send asset data to the server!");
		status = false;
	}

	return status;
}

void Client::RecievePacket()
{
	sf::Packet packet;

	//Variables to write the port and ip address of the sender.
	sf::IpAddress serverIPAddress;
	uint16_t serverPort;

	mUDPSocket.setBlocking(false);
	if (mUDPSocket.receive(packet, serverIPAddress, serverPort) != sf::UdpSocket::Done)
	{
		APP_ERROR("Could not send packet to server!");
	}	

	GameData data;

	if (!(packet >> data))
	{
		APP_ERROR("Could not unpack game update from server!");
	}

}

void Client::SendAssetsToServer(AssetList assetList)
{
	sf::Packet packet;

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

void Client::Disconnect()
{
	ChatMSG data;
	sf::Packet packet;

	data.id = 0;
	data.quit = 1;
	data.message = " ";

	packet << data;

	if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Could not disconnect from server... Try again.");
	}
}


