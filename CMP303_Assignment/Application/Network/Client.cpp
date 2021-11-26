#include "Client.h"

//Const data
const sf::IpAddress SERVER_PUBLIC_IP = sf::IpAddress::getPublicAddress();
const sf::IpAddress SERVER_LOCAL_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;


sf::Packet& operator <<(sf::Packet& packet, const DisconnectPCKT& data)
{
	return packet << data.time << data.message << data.id << data.quit;
}
sf::Packet& operator >>(sf::Packet& packet, DisconnectPCKT& data)
{
	return packet >> data.time >> data.message >> data.id >> data.quit;
}

sf::Packet& operator >>(sf::Packet& packet, GameData& data)
{
	packet >> data.time >> data.arraySize;

	sf::Uint32 count = data.arraySize;
	data.InitArray(count);

	for (int i = 0; i < count; ++i)
	{
		packet >> data.objectIDs[i];
	}
	for (int i = 0; i < count; ++i)
	{
		packet >> data.x[i];
	}
	for (int i = 0; i < count; ++i)
	{
		packet >> data.y[i];
	}
	

	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const GameData& data)
{
	packet << data.time << data.arraySize;

	sf::Uint32 count = data.arraySize;

	for (int i = 0; i < count; ++i)
	{
		packet << data.objectIDs[i];
	}
	for (int i = 0; i < count; ++i)
	{
		packet << data.x[i];
	}
	for (int i = 0; i < count; ++i)
	{
		packet << data.y[i];
	}


	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, ConnectionData& data)
{
	return packet >> data.time >> data.privelage >> data.udpPort >> data.ipAddress >> data.type >> data.count >> data.sizeX >> data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectionData& data)
{
	return packet << data.time << data.privelage << data.udpPort << data.ipAddress << data.type << data.count << data.sizeX << data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ClientPortAndIP& data)
{
	return packet << data.udpPort;
}

sf::Packet& operator >>(sf::Packet& packet, ClientPortAndIP& data)
{
	return packet >> data.udpPort;
}


Client::Client()
	:
	mJitter(0.f),
	mLatency(0.f),
	mPrivelage(ClientPrivelage::None)
{
	// Fetch the machines IP Address. 
	mIPAdress = sf::IpAddress::getLocalAddress();
	mUDPPort = 4444;

	//Bind the UDP socket to an available port.
	if (mUDPSocket.bind(sf::UdpSocket::AnyPort) != sf::UdpSocket::Done)
	{
		APP_TRACE("Could not bind to port {0}, fetching an unused port.", mUDPSocket.getLocalPort());
		//Try again using any port which is available, SFML will find an unused port.	
		if (mUDPSocket.bind(sf::UdpSocket::AnyPort) != sf::UdpSocket::Done)
		{
			APP_ERROR("We're sorry, unfortunately there are no available ports on this machine.");
		}

		APP_TRACE("Bound UDP socket to port {0}", mUDPSocket.getLocalPort());
	}
	else
	{
		APP_TRACE("Bound UDP socket to port {0}", mUDPSocket.getLocalPort());
	}
}

Client::~Client()
{
	mUDPSocket.unbind();
}

void Client::ConnectToServer()
{
	//Establish connection with the server.
	if (mTCPSocket.connect(sf::IpAddress::getLocalAddress(), SERVER_PORT) != sf::TcpSocket::Done)
	{
		APP_ERROR("Connection failed!");
	}

 	APP_TRACE("Connected to server.");


	//Set the sockets to not block the application.
	mTCPSocket.setBlocking(false);
	mUDPSocket.setBlocking(false);
}

void Client::SendGamePacket(std::vector<sf::Vector2f> positions)
{
	GameData data;
	//Initialise the buffer.
	if (mPrivelage == ClientPrivelage::Host)
	{
		data.InitArray(positions.size());
	}
	else
	{
		//Only one position to send.
		data.InitArray(1);
	}

	for (int i = 0; i < positions.size(); ++i)
	{
		data.x[i] = positions[i].x;
		data.y[i] = positions[i].y;
		data.objectIDs[i] = i;
	}

	time_t t = time(0);
	data.time = (double)t;
	sf::Packet packet;

	if (!(packet << data))
	{
		APP_ERROR("Could not pack game data!");
	}
	else
	{
		//Send data to peers.
		if (mPeers.size() != 0)
		{
			for (auto& peer : mPeers)
			{
				if (mUDPSocket.send(packet, sf::IpAddress::getLocalAddress(), peer) != sf::UdpSocket::Done)
				{
					APP_ERROR("Could not send packet to peer on port {0} !", peer);
				}
			}
		}
	}
}

void Client::SendConnectionInformation(AssetType assetType, AssetCount assetCount, sf::Vector2f assetSize)
{
	sf::Packet packet;
	ConnectionData data;
	data.time = time(0);

	if (mPrivelage == ClientPrivelage::Host)
	{
		data.privelage = 0;
		data.count = assetCount;
		data.type = assetType;
		data.sizeX = assetSize.x;
		data.sizeY = assetSize.y;
		data.udpPort = mUDPSocket.getLocalPort();
		data.ipAddress = mIPAdress.toInteger();

		if (!(packet << data))
		{
			APP_ERROR("Host : Failed to pack connection data.");
		}
		else
		{
			if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
			{
				APP_ERROR("Unable to send TCP connection data.");
			}
			else
			{
				APP_TRACE("Client : Sending connection data successful");
			}
		}
	}
	else
	{
		data.privelage = 1;
		data.count = 0;
		data.type = 0;
		data.sizeX = 0;
		data.sizeY = 0;
		data.udpPort = mUDPSocket.getLocalPort();
		data.ipAddress = mIPAdress.toInteger();

		if (!(packet << data))
		{
			APP_ERROR("Peer : Failed to pack connection data.");
		}
		else
		{
			if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
			{
				APP_ERROR("Unable to send TCP connection data.");
			}
			else
			{
				APP_TRACE("Client : Sending connection data successful");
			}
		}
	}
}

ConnectionData& Client::RecieveHostAssets()
{
	sf::Packet packet;
	ConnectionData connData;
	connData.count = 0;

	APP_TRACE("Waiting... gathering host asset data...");
	
	if (mTCPSocket.receive(packet) != sf::TcpSocket::Done)
	{
		APP_ERROR("Recieve failed! : RecieveHostAssets()");
		return connData;
	}
	
	if (!(packet >> connData))
	{
		APP_ERROR("Data unpack failed! : RecieveHostAssets()");
	}
	else
	{
		sf::IpAddress ipAddress = sf::IpAddress(connData.ipAddress);
		mPeers.push_back(connData.udpPort);
		return connData;
	}
}

void Client::RecievePacket()
{
	sf::Packet packet;

	//Variables to write the port and ip address of the sender.
	sf::IpAddress serverIPAddress;
	uint16_t serverPort;


	if (mUDPSocket.receive(packet, serverIPAddress, serverPort) == sf::UdpSocket::Done)
	{
		GameData data;

		if (!(packet >> data))
		{
			APP_ERROR("Could not unpack game data!");
		}
		else
		{
			mGameData.push_back(data);
		}
	}	
	else
	{
		APP_ERROR("Client : Recieve failed ~RecievePacket() ");
	}
}

void Client::GatherNewPorts()
{
	sf::Packet packet;
	ClientPortAndIP data;

	if (mTCPSocket.receive(packet) != sf::UdpSocket::Done)
	{
		APP_ERROR("Failed to obtain new ports.");
	}
	else
	{
		if (!(packet >> data))
		{
			APP_ERROR("Failed to pack data : Client GatherNewPorts() ");
		}
		else
		{
			if (mPeers.size() == 0)
			{
				APP_TRACE("New peer joined on port {0} .", data.udpPort);
				mPeers.push_back(data.udpPort);
			}
			else if(mPeers.size() != 0)
			{
				for (auto& peers : mPeers)
				{
					if ((peers != data.udpPort))
					{
						APP_TRACE("New peer joined on port {0} .", data.udpPort);
						mPeers.push_back(data.udpPort);
					}
				}
			}
		}
	}
}

bool Client::Disconnect()
{
	DisconnectPCKT data;
	sf::Packet packet;
	bool returnStatus = false;

	data.id = 0;
	data.quit = 1;
	data.message = " ";

	if (!(packet << data))
	{
		APP_ERROR("Failed to pack data...");
	}
	else
	{
		if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("Could not disconnect from server gracefully......");
		}
		else
		{
			mTCPSocket.disconnect();
			returnStatus = true;
		}
	}
	return returnStatus;
}


