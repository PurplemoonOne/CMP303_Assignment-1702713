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


Client::Client(sf::Vector2f windowMaxBoundaries)
	:
	mWindowMaxBoundary(windowMaxBoundaries)
{
	// Fetch the machines IP Address. 
	mIPAdress = sf::IpAddress::getLocalAddress();
	BindUDPSockets();
}

Client::~Client()
{

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

void Client::SendGamePacket(std::vector<sf::Vector2f>& positions, const float appElapsedTime)
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
		data.count = 1;
		data.type = 0;
		data.sizeX = assetSize.x;
		data.sizeY = assetSize.x;
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

ConnectionData& Client::RecieveAssetsDescFromServer()
{
	sf::Packet packet;
	ConnectionData connData;
	connData.count = 0;

	APP_TRACE("Gathering asset data...");

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
			mAssetCount = connData.count;

			if (std::find(mPeers.begin(), mPeers.end(), connData.peerUdpRecvPort) == mPeers.end())
			{
				mPeers.push_back(connData.peerUdpRecvPort);
			}
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
				GameData recvData;
				if (!(packet >> recvData))
				{
					APP_ERROR("Could not unpack game data!");
				}
				else
				{
	
					bool invalid = false;
					//Check for tempering....

					if (recvData.arraySize != mAssetCount)
					{
						APP_WARNING("Packet tampered! Invalid array size! Discarding data...");
						invalid = true;
					}

					for (auto& peerPort : mPeers)
					{
						if (recvData.peerUdpRecvPort != peerPort)
						{
							APP_WARNING("Packet tampered! Invalid port number! Discarding data...");
							invalid = true;
						}
					}

					for (int i = 0; i < mAssetCount; ++i)
					{
						bool validateX = (recvData.x[i] < 0.f || recvData.x[i] > mWindowMaxBoundary.x);
						bool validateY = (recvData.y[i] < 0.f || recvData.y[i] > mWindowMaxBoundary.y);
						bool validID = (recvData.objectIDs[i] > i || recvData.objectIDs[i] < i);
						if (validateX || validateY || validID)
						{
							APP_WARNING("Packet tampered! Invalid position and/or ID! Discarding data...");
							invalid = true;
						}
					}
					
					//Check for packet duplication...
					for (auto& storeData : mGameData)
					{
						if (storeData.time == recvData.time)
						{
							APP_WARNING("Duplicate packet recieved!");
							invalid = true;
						}
					}

					//Finally, if passed all conditions push data back onto the 
					if(!invalid)
					{
						//Otherwise we don't have this packet so store it.
						APP_TRACE("Received a valid packet!");
						mGameData.push_back(recvData);
					}
				}
			}
		}
	}
	else
	{
		APP_TRACE("Wait time out...");
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
			exit(-1);
		}
		else
		{
			mTCPSocket.disconnect();
			mUDPSendSocket.unbind();
			mUDPRecvSocket.unbind();
			returnStatus = true;
		}
	}
	return returnStatus;
}

