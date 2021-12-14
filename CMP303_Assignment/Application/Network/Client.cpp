#include "Client.h"
#include <chrono>
//Const data
const sf::Uint16 SERVER_PORT = 5555;

using namespace std::chrono;

sf::Packet& operator <<(sf::Packet& packet, const DisconnectPCKT& data)
{
	return packet << data.systemTime << data.latency << data.message << data.id << data.quit;
}

sf::Packet& operator >>(sf::Packet& packet, DisconnectPCKT& data)
{
	return packet >> data.systemTime >> data.latency >> data.message >> data.id >> data.quit;
}

sf::Packet& operator >>(sf::Packet& packet, GameData& data)
{
	packet >> data.systemTime >> data.latency >> data.peerUdpRecvPort >> data.peerIpAddress >> data.arraySize >> data.quit;

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
	packet  << data.systemTime << data.latency << data.peerUdpRecvPort << data.peerIpAddress << data.arraySize << data.quit;

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


Client::Client(sf::Vector2f windowMaxBoundaries)
	:
	mWindowMaxBoundary(windowMaxBoundaries),
	mThisConnQuit(false),
	mPeerQuit(false)
{
	// Fetch the machines IP Address. 
	mLocalIpAddress = sf::IpAddress::getLocalAddress();
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
		APP_TRACE("Could not bind to port {0}, fetching an unused port.", 4444);
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
	if (mTCPSocket.connect(mServerIpAddress, SERVER_PORT) != sf::TcpSocket::Done)
	{
		APP_ERROR("Connection failed... defaulting to '127.0.0.1'.");
		if (mTCPSocket.connect("127.0.0.1", SERVER_PORT) != sf::TcpSocket::Done)
		{
			APP_ERROR("Critical failure... could not establish a connection with the server.");
		}
		else
		{
			APP_INFO("Connected to server on port {0} network {1}.", SERVER_PORT, mLocalIpAddress);
		}
	}
	else
	{
		APP_INFO("Connected to server.");

		//Add the reciece UDP socket to the select.
		mSelect.add(mUDPRecvSocket);
		mSelect.add(mTCPSocket);
		mUDPSendSocket.setBlocking(false);

		//Get this machines address.
		mLocalIpAddress = sf::IpAddress::getLocalAddress();
	}
}

void Client::SendGamePacket(std::vector<sf::Vector2f>& positions, std::vector<float>& rotations, std::vector<std::pair<float, float>>& scales)
{
	GameData data;
	bool heapAlloc = false;

	if (mPeerLatency > 0.f)
	{
		data.latency = mPeerLatency;
	}
	if (mThisConnQuit)
	{
		data.quit = 1;
	}

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
		sf::IpAddress ipAddress;
		sf::Uint16 port;
		//Send data to peer.
		
		ipAddress = sf::IpAddress(mPeer.ipAddress);
		port = mPeer.port;
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
		data.ipAddress = mLocalIpAddress.toInteger();

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
		data.ipAddress = mLocalIpAddress.toInteger();

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

ConnectionData& Client::RecieveAssetsDescFromClient()
{
	sf::Packet packet;
	ConnectionData connData;
	connData.count = 0;
	Peer peer;
	APP_TRACE("Gathering asset data...");

	if (mSelect.wait(sf::milliseconds(16)))
	{
		if (mSelect.isReady(mTCPSocket))
		{
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

					if (!(mPeer.port == connData.peerUdpRecvPort))
					{
						mPeer.port = connData.peerUdpRecvPort;
						mPeer.ipAddress = connData.ipAddress;

					}
					else
					{
						APP_WARNING("Already have peer data.")
					}
				}
			}
		}
	}


	return connData;
}

void Client::RecieveGameUpdate()
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
				if ((packet >> recvData))
				{
					//Store the game update and runa security check on the data.
					APP_INFO("Recieved game update.");
					StoreGameUpdate(recvData);

					mRecvTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

					mPeerLatency = mRecvTime - recvData.systemTime;
					mPeerLatency *= 0.001; //Convert into seconds.

					if (recvData.latency > 0.f)
					{
						mLatency = recvData.latency;
					}

					if (recvData.quit == 1)
					{
						mPeerQuit = true;
					}
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
		if (mPeer.port != data.peerUdpRecvPort)
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

			//If the ID and index match we have a valid ID for this asset.
			bool validID = (data.objectIDs[i] == i);
			//All must pass the previous tests to be considered.
			if (!(validateX && validateY && validID))
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
	}
}

bool Client::Disconnect()
{
	DisconnectPCKT data;
	GameData sendData;
	sf::Packet packet;

	data.id = 0;
	data.quit = 1;
	std::string privelage = std::to_string((int)mPrivelage);
	data.message = privelage;
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
		}
		else
		{
			APP_INFO("Disconnection packet successfully sent to the server");
			mThisConnQuit = true;
		}
	}
	return mThisConnQuit;
}

