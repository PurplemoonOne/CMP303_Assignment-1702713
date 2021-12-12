#include "Server.h"
#include "../Log/ServerLog.h"


const sf::IpAddress SERVER_PUBLIC_IP = sf::IpAddress::getPublicAddress();
const sf::IpAddress MACHINE_LOCAL_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;

Server::Server()
	:
	mHasHostAssets(false),
	mHasClientAssets(false)
{
	//Initialise log system.
	ServerLog::Init();
	mConnections.fill(nullptr);
}

Server::~Server()
{
	mListener.close();
}

void Server::ProcessEvents(sf::RenderWindow& window)
{
	// check all the window's events that were triggered since the last iteration of the loop
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			window.setSize(sf::Vector2u(event.size.width, event.size.height));
			break;
		default:
			break;
		}
	}
}

void Server::ListenForConections()
{
	if (mListener.listen(SERVER_PORT, sf::IpAddress::getLocalAddress()))
	{
		APP_ERROR("Listen failed! Aborting...");
		exit(1);
	}

	APP_TRACE("Listening....");
}

void Server::QueryConnections()
{
	//Wait for any sockets to be ready.
	if (mSelect.wait(sf::milliseconds(16.0f)))
	{
		//Accept new connections.
		if (mSelect.isReady(mListener))
		{
			sf::TcpSocket* clientSocket = new sf::TcpSocket();

			if (mListener.accept(*clientSocket) != sf::TcpListener::Done)
			{
				APP_ERROR("Accept failed!");
			}

			APP_TRACE("New connection established!");
	
			//Initialise the connection.
			InitConnection(clientSocket);
		}
	}	

	if (mSelect.wait(sf::milliseconds(16.0f)))
	{
		//Read newly messages and or requests to leave the stream.
		for (sf::Uint32 i = 0; i < mConnections.size(); ++i)
		{
			//Element 0 is reserved for host. Hence element 0 is NULL if no host.
			if (mConnections.at(i) != nullptr)
			{
				if (mSelect.isReady(*mConnections.at(i)->GetTCPSocket()))
				{
					DisconnectPCKT data;
					mConnections.at(i)->RecieveTCP(data);

					if (data.quit == 1)
					{
						RemoveConnection(i);
						break;
					}
				}
			}
		}
	}

	if (mConnections.at(1) != nullptr)
	{
		if (!mConnections.at(1)->HasAssets())
		{
			if (mHasHostAssets && !(mConnections.at(1)->HasAssets()))
			{
				if (mConnections.at(1)->SendAssets(mHostAssets))
				{
					mConnections.at(1)->SetHasAssets(true);
				}
			}
		}
	}
	if (mConnections.at(0) != nullptr)
	{
		if (!mConnections.at(0)->HasAssets())
		{
			if (mHasClientAssets && !(mConnections.at(0)->HasAssets()))
			{
				if (mConnections.at(0)->SendAssets(mClientAssets))
				{
					mConnections.at(0)->SetHasAssets(true);
				}
			}
		}
	}
}


void Server::RemoveConnection(sf::Uint32 element)
{
	sf::Uint16 port = mConnections.at(element)->GetTCPPort();
	sf::IpAddress ipAddress = mConnections.at(element)->GetIPAddress();

	//Host has left...
	if (element == 0)
	{
		mHasHostAssets = false;
		mHostCount--;
	}
	//Client has left.
	else
	{
		mHasClientAssets = false;
		mClientCount--;
	}

	mTotalConnections--;

	//Remove the reference to the socket from the select object.
	mSelect.remove(*mConnections.at(element)->GetTCPSocket());

	//Close the TCP socket.
	mConnections.at(element)->GetTCPSocket()->disconnect();

	//Delete the socket object from the heap.
	delete mConnections.at(element)->GetTCPSocket();
	delete mConnections.at(element);
	mConnections.at(element) = nullptr;

	if (element == 0)
	{
		APP_TRACE("Host on port {0} : IP Adreess {1} has left.", port, ipAddress.toString());
	}
	else
	{
		APP_TRACE("Client on port {0} : IP Adreess {1} has left.", port, ipAddress.toString());
	}
}

void Server::InitConnection(sf::TcpSocket* socket)
{
	//Create a new connection object.
	Connection* connection = new Connection(socket);

	//Gather information describing this connection.
	ConnectionData connectionData;
	connection->RecieveAssets(connectionData);

	if (connectionData.privelage == 0)
	{
		//If the number of host's is not greater than 1.
		if (mHostCount < 1)
		{
			APP_TRACE("A new host has joined!");

			connection->SetPrivelage(ClientPrivelage::Host);
			connection->SetUDPPort(connectionData.peerUdpRecvPort);
			connection->SetNetworkID(mTotalConnections);
			connection->SetInit(true);
			connection->SetConnectionIP(connectionData.ipAddress);
			connection->SetHasAssets(false);
			mSelect.add(*connection->GetTCPSocket());
			mConnections.at(0) = connection;

			StoreAssetData(connectionData);
			
			APP_TRACE("Storing host asset description.");

			mHostCount++;
			mTotalConnections++;
		}

		//Another host attempted to join the server.
		else
		{
			//Send a message to the client attepted to host and close connection.
			APP_WARNING("Another connection attempted to host!");
			DisconnectPCKT connectionData;
			connectionData.message = "The server has too many hosts, try again later.";
			connectionData.id = 0;
			connection->SendTCP(connectionData);
			connection->GetTCPSocket()->disconnect();
			delete connection;
			connection = nullptr;
			return;
		}
	}


	//A client joined.
	else if(connectionData.privelage == 1)
	{
		if (mClientCount < 1)
		{
			APP_TRACE("A new client has joined!");

			connection->SetPrivelage(ClientPrivelage::Client);
			connection->SetUDPPort(connectionData.peerUdpRecvPort);
			connection->SetNetworkID(mTotalConnections);
			connection->SetInit(true);
			connection->SetHasAssets(false);
			connection->SetConnectionIP(connectionData.ipAddress);
			mConnections.at(1) = connection;
			mSelect.add(*connection->GetTCPSocket());
			StoreAssetData(connectionData);
			APP_TRACE("Storing client asset description.");

			mClientCount++;
			mTotalConnections++;
		}

		else
		{
			//Send a message to the client attepted to host and close connection.
			APP_WARNING("Another connection attempted to join!");
			DisconnectPCKT connectionData;
			connectionData.message = "The server has too many clients, try again later.";
			connectionData.id = 0;
			connection->SendTCP(connectionData);
			connection->GetTCPSocket()->disconnect();
			delete connection;
			connection = nullptr;
			return;
		}
	}

	//Something went wrong.
	else
	{
		delete connection;
		connection = nullptr;
		APP_WARNING("Unknown entity has joined... Resolving.");
	}
}

void Server::StoreAssetData(ConnectionData& data)
{
	if (data.privelage == 0)
	{
		mHostAssets.count = data.count;
		mHostAssets.type = data.type;
		mHostAssets.sizeX = data.sizeX;
		mHostAssets.sizeY = data.sizeY;
		mHostAssets.ipAddress = data.ipAddress;
		mHostAssets.peerUdpRecvPort = data.peerUdpRecvPort;
		mHasHostAssets = true;
	}
	else
	{
		mClientAssets.count = data.count;
		mClientAssets.type = data.type;
		mClientAssets.sizeX = data.sizeX;
		mClientAssets.sizeY = data.sizeY;
		mClientAssets.ipAddress = data.ipAddress;
		mClientAssets.peerUdpRecvPort = data.peerUdpRecvPort;
		mHasClientAssets = true;
	}
}

void Server::Run()
{

	sf::Time elapsed;
	sf::Clock clock;

	//Windows window used for rendering mGraphics to the screen.
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Server");

	float appElapsedTime = 0.f;

	// Listen for new connections.
	ListenForConections();

	mSelect.add(mListener);

	// run the program as long as the window is open
	while (window.isOpen())
	{
		//Calculate delta time.
		elapsed = clock.restart();

		QueryConnections();

		//Process the event queue.
		ProcessEvents(window);

		window.clear(sf::Color(125, 125, 125, 255));

		window.display();

		appElapsedTime += 1.f * elapsed.asSeconds();
	}

}
