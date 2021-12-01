#include "Client.h"
#include <chrono>
//Const data
const sf::IpAddress SERVER_PUBLIC_IP = sf::IpAddress::getPublicAddress();
const sf::IpAddress MACHINE_LOCAL_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;

using namespace std::chrono;

sf::Packet& operator <<(sf::Packet& packet, const DisconnectPCKT& data)
{
	return packet << data.systemTime << data.message << data.id << data.quit;
}

sf::Packet& operator >>(sf::Packet& packet, DisconnectPCKT& data)
{
	return packet >> data.systemTime >> data.message >> data.id >> data.quit;
}

sf::Packet& operator >>(sf::Packet& packet, GameData& data)
{
	packet >> data.systemTime >> data.peerUdpRecvPort >> data.arraySize;

	sf::Uint32 count = data.arraySize;
	if (count < 256)
	{
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
		for (int i = 0; i < count; ++i)
		{
			packet >> data.rotations[i];
		}
		for (int i = 0; i < count; ++i)
		{
			packet >> data.scaleX[i];
		}
		for (int i = 0; i < count; ++i)
		{
			packet >> data.scaleY[i];
		}
	}
	return packet;
}

sf::Packet& operator <<(sf::Packet& packet, const GameData& data)
{
	packet  << data.systemTime << data.peerUdpRecvPort << data.arraySize;

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
	for (int i = 0; i < count; ++i)
	{
		packet << data.rotations[i];
	}
	for (int i = 0; i < count; ++i)
	{
		packet << data.scaleX[i];
	}
	for (int i = 0; i < count; ++i)
	{
		packet << data.scaleY[i];
	}

	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, ConnectionData& data)
{
	return packet >> data.systemTime >> data.privelage >> data.peerUdpRecvPort >> data.ipAddress >> data.type >> data.count >> data.sizeX >> data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ConnectionData& data)
{
	return packet << data.systemTime << data.privelage << data.peerUdpRecvPort << data.ipAddress << data.type << data.count << data.sizeX << data.sizeY;
}

sf::Packet& operator <<(sf::Packet& packet, const ClientPortAndIP& data)
{
	return packet << data.udpPort;
}

sf::Packet& operator >>(sf::Packet& packet, ClientPortAndIP& data)
{
	return packet >> data.udpPort;
}

sf::Packet& operator <<(sf::Packet& packet, const LatencyPacket& data)
{
	return packet << data.timeA << data.timeB;
}

sf::Packet& operator >>(sf::Packet& packet, LatencyPacket& data)
{
	return packet >> data.timeA >> data.timeB;
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

void Client::SendGamePacket(std::vector<sf::Vector2f>& positions, std::vector<float>& rotations, std::vector<std::pair<float, float>>& scales)
{
	GameData data;
	bool heapAlloc = false;
	//Initialise the buffer.
	if (mPrivelage == ClientPrivelage::Host)
	{
		//Host boid positions.
		data.InitArray(positions.size());
		heapAlloc = true;
	}
	else
	{
		//Client position.
		data.InitArray(1);
		heapAlloc = true;
	}

	//Fill the buffer.
	for (int i = 0; i < positions.size(); ++i)
	{
		data.x[i] = positions[i].x;
		data.y[i] = positions[i].y;
		data.rotations[i] = rotations[i];
		data.scaleX[i] = scales[i].first;
		data.scaleY[i] = scales[i].second;
		data.objectIDs[i] = i;
	}

	data.peerUdpRecvPort = mUDPRecvPort;


	sf::Packet packet;

	//Track this for latency.
	data.systemTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

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
					if (heapAlloc)
					{
						//Delete local data from heap.
						data.DeleteData();
					}
				}
			}
		}
	}
}

void Client::SendConnectionInformation(AssetType assetType, AssetCount assetCount, sf::Vector2f assetSize)
{
	sf::Packet packet;
	ConnectionData data;
	data.systemTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

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
			mAssetScale = connData.sizeX;

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

			if (mUDPRecvSocket.receive(packet, ipAddress, port) != sf::UdpSocket::Done)
			{
				APP_ERROR("Client : Recieve failed ~RecievePacket() ");
			}
			else 
			{
				GameData recvData;
				LatencyPacket recvLatencyData;
				if ((packet >> recvData))
				{
					APP_INFO("Recieved game update.");
					StoreGameUpdate(recvData);
				}
				else if((packet >> recvLatencyData))
				{
					APP_INFO("Recieved our latest latency data.");
				
					mLatency = recvLatencyData.timeB - recvLatencyData.timeA;
					mLatency *= 0.001; //Convert into seconds.
				}
				else
				{
					APP_ERROR("Unpacking data failed, unknown data recieved.");
				}
			}
		}//Not ready.
	}//Time out.
	else
	{
		APP_TRACE("Wait time out...");

		if (mPeers.size() != 0)
		{

		}

	}
}

void Client::StoreGameUpdate(const GameData& data)
{
	bool invalid = false;
	//Check for tempering....
	//Is there a strange asset count...?
	if (data.arraySize != mAssetCount)
	{
		APP_WARNING("Packet tampered! Invalid array size! Discarding data...");
		invalid = true;
	}

	if (!invalid)
	{
		//Does the port match current records of port numbers...?
		if(std::find(mPeers.begin(), mPeers.end(), data.peerUdpRecvPort) == mPeers.end())
		{
			APP_WARNING("Packet tampered! Invalid port number! Discarding data...");
			invalid = true;
		}
	}

	if (!invalid)
	{
		//Is there an unexpected value?
		for (int i = 0; i < mAssetCount; ++i)
		{
			//If we're in the boundaries of the level we have partly a valid packet.
			bool validateX = (data.x[i] > 0.f && data.x[i] < mWindowMaxBoundary.x);
			bool validateY = (data.y[i] > 0.f && data.y[i] < mWindowMaxBoundary.y);
			//Check rotations are valid.
			bool validateRotation = (data.rotations[i] >= 0.0f && data.rotations[i] <= 360.0f) ? true : false;
			//If the ID and index match we have a valid ID for this asset.
			bool validID = (data.objectIDs[i] == i);
			//All must pass the previous tests to be considered.
			if (!(validateX && validateY && validID && validateRotation))
			{
				APP_WARNING("Packet tampered! Invalid position and/or ID! Discarding data...");
				invalid = true;
			}
		}
	}

	if (!invalid)
	{

		//Is there an unexpected value?
		for (int i = 0; i < mAssetCount; ++i)
		{
			//If the packet reads we're larger or smaller than the expected size... invalid.
			bool validateScaleX = (int(data.scaleX[i]) == mAssetScale || int(data.scaleX[i]) == -mAssetScale) ? true : false;
			bool validateScaleY = (int(data.scaleY[i]) == mAssetScale || int(data.scaleY[i]) == -mAssetScale) ? true : false;
			if (!(validateScaleX && validateScaleY))
			{
				APP_WARNING("Packet tampered! Invalid scale! Discarding data...");
				invalid = true;
			}
		}
	}

	if (!invalid)
	{
		//Check for packet duplication...
		for (auto& storeData : mGameData)
		{
			if (int(storeData.systemTime == data.systemTime))
			{
				APP_WARNING("Duplicate packet recieved!");
				invalid = true;
			}
		}
	}


	//Finally, if passed all conditions push data back onto the array.
	//Also send back latency data to connection.
	if (!invalid)
	{
		//Otherwise we don't have this packet so store it.
		APP_INFO("Received a valid packet!");
		mGameData.push_back(data);

		//Send back a latency packet to inform the other connection their current latency.
		LatencyPacket latencyData;
		sf::Packet latencyPacket;
		latencyData.timeA = data.systemTime;//Time stored when sent from other connection.
		latencyData.timeB = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();//Time now when recieved.

		if (!(latencyPacket << latencyData))
		{
			APP_ERROR("Failed to pack latency data...");
		}
		else
		{
			sf::Uint16 returnToSenderPort = data.peerUdpRecvPort;
			sf::IpAddress ipAddress = MACHINE_LOCAL_IP;
			//Already have valid port number and ip address from the connection.
			if (mUDPSendSocket.send(latencyPacket, ipAddress, returnToSenderPort) != sf::UdpSocket::Done)
			{
				APP_ERROR("Failed to send latency packet!");
			}
			else
			{
				APP_INFO("Sending latency data back to peer...");
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
	data.systemTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	if (!(packet << data))
	{
		APP_ERROR("Failed to pack data...");
	}
	else
	{
		if (mTCPSocket.send(packet) != sf::TcpSocket::Done)
		{
			APP_ERROR("Could not disconnect from server gracefully......");
			mTCPSocket.disconnect();
			mUDPSendSocket.unbind();
			mUDPRecvSocket.unbind();
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

