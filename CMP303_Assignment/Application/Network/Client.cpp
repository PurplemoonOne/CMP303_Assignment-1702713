#include "Client.h"

//Const data
const sf::IpAddress SERVER_PUBLIC_IP = sf::IpAddress::getPublicAddress();
const sf::IpAddress MACHINE_LOCAL_IP = sf::IpAddress::getLocalAddress();
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
	packet >> data.time >> data.peerUdpRecvPort >> data.arraySize;

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
	packet << data.time << data.peerUdpRecvPort << data.arraySize;

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
	return packet >> data.time >> data.privelage >> data.peerUdpRecvPort >> data.ipAddress >> data.type >> data.count >> data.sizeX >> data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectionData& data)
{
	return packet << data.time << data.privelage << data.peerUdpRecvPort << data.ipAddress << data.type << data.count << data.sizeX << data.sizeY;
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
	BindUDPSockets();
}

Client::~Client()
{
	mUDPSendSocket.unbind();
	mUDPRecvSocket.unbind();
}

void Client::BindUDPSockets()
{
	//Bind the UDP socket to an available port.
	if (mUDPSendSocket.bind(4444) != sf::UdpSocket::Done)
	{
		APP_TRACE("Could not bind to port {0}, fetching an unused port.", mUDPSendSocket.getLocalPort());
		//Try again using any port which is available, SFML will find an unused port.	
		if (mUDPSendSocket.bind(sf::UdpSocket::AnyPort) != sf::UdpSocket::Done)
		{
			APP_ERROR("We're sorry, unfortunately there are no available ports on this machine.");
		}

		APP_TRACE("Bound Send UDP socket to port {0}", mUDPSendSocket.getLocalPort());
	}
	else
	{
		APP_TRACE("Bound Send UDP socket to port {0}", mUDPSendSocket.getLocalPort());
	}

	mUDPSendPort = mUDPSendSocket.getLocalPort();

	//Bind the UDP socket to an available port.
	if (mUDPRecvSocket.bind(4445) != sf::UdpSocket::Done)
	{
		APP_TRACE("Could not bind to port {0}, fetching an unused port.", mUDPRecvSocket.getLocalPort());
		//Try again using any port which is available, SFML will find an unused port.	
		if (mUDPRecvSocket.bind(sf::UdpSocket::AnyPort) != sf::UdpSocket::Done)
		{
			APP_ERROR("We're sorry, unfortunately there are no available ports on this machine.");
		}

		APP_TRACE("Bound Recieve UDP socket to port {0}", mUDPRecvSocket.getLocalPort());
	}
	else
	{
		APP_TRACE("Bound Recieve UDP socket to port {0}", mUDPRecvSocket.getLocalPort());
	}

	mUDPRecvPort = mUDPRecvSocket.getLocalPort();

}

void Client::ConnectToServer()
{
	//Establish connection with the server.
	if (mTCPSocket.connect(sf::IpAddress::getLocalAddress(), SERVER_PORT) != sf::TcpSocket::Done)
	{
		APP_ERROR("Connection failed!");
	}
	else
	{
		APP_TRACE("Connected to server.");


		//Set the sockets to not block the application.
		mTCPSocket.setBlocking(false);

		//Add the reciece UDP socket to the select.
		mSelect.add(mUDPRecvSocket);

		mUDPSendSocket.setBlocking(false);
	}
}

void Client::SendGamePacket(std::vector<sf::Vector2f> positions, const float appElapsedTime)
{
	GameData data;
	//Initialise the buffer.
	if (mPrivelage == ClientPrivelage::Host)
	{
		//Host boid positions.
		data.InitArray(positions.size());
	}
	else
	{
		//Client position.
		data.InitArray(1);
	}

	//Fill the buffer.
	for (int i = 0; i < positions.size(); ++i)
	{
		data.x[i] = positions[i].x;
		data.y[i] = positions[i].y;
		data.objectIDs[i] = i;
	}

	data.time = appElapsedTime;
	data.peerUdpRecvPort = mUDPRecvPort;
	sf::Packet packet;

	if (!(packet << data))
	{
		APP_ERROR("Could not pack game data!");
	}
	else
	{
		//Variables to write the port and ip address of the sender.
		sf::IpAddress ipAddress = sf::IpAddress::getLocalAddress();
		sf::Uint16 port;
		//Send data to peers.
		if (mPeers.size() != 0)
		{
			for (auto& peer : mPeers)
			{
				port = peer;
				if (mUDPSendSocket.send(packet, ipAddress, port) != sf::UdpSocket::Done)
				{
					APP_ERROR("Could not send packet to peer on port {0} !", port);
				}
				else
				{
					APP_TRACE("PACKET SENT TO PORT : {0}", port);
				}
			}
		}
	}
}

void Client::SendConnectionInformation(AssetType assetType, AssetCount assetCount, sf::Vector2f assetSize)
{
	sf::Packet packet;
	ConnectionData data;
	data.time = 0.0f;

	if (mPrivelage == ClientPrivelage::Host)
	{
		data.privelage = 0;
		data.count = assetCount;
		data.type = assetType;
		data.sizeX = assetSize.x;
		data.sizeY = assetSize.y;
		data.peerUdpRecvPort = mUDPRecvPort;

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
		data.peerUdpRecvPort = mUDPRecvPort;

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
	}
	else
	{
		if (!(packet >> connData))
		{
			APP_ERROR("Data unpack failed! : RecieveHostAssets()");
		}
		else
		{
			mPeers.push_back(connData.peerUdpRecvPort);
		}
	}

	return connData;
}

void Client::RecievePacket()
{
	if (mSelect.wait(sf::milliseconds(16.0f)))
	{
		if (mSelect.isReady(mUDPRecvSocket))
		{
			sf::Packet packet;

			//Variables to write the port and ip address of the sender.
			sf::IpAddress ipAddress;
			sf::Uint16 port;
			sf::UdpSocket::Status status;

			if (mUDPRecvSocket.receive(packet, ipAddress, port) != sf::UdpSocket::Done)
			{
				APP_ERROR("Client : Recieve failed ~RecievePacket() ");
			}
			else 
			{
				GameData data;
				if (!(packet >> data))
				{
					APP_ERROR("Could not unpack game data!");
				}
				else
				{
					if (std::find(mPeers.begin(), mPeers.end(), data.peerUdpRecvPort) == mPeers.end())
					{
						mPeers.push_back(data.peerUdpRecvPort);
						APP_TRACE("Added a new valid peer.");
					}
					APP_TRACE("Received packet!");
					mGameData.push_back(data);
				}
			}
		}
	}
	else
	{
		APP_TRACE("Wait time out..");
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

