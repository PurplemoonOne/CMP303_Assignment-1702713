#include "Server.h"
#include "../Log/ServerLog.h"


const sf::IpAddress SERVER_PUBLIC_IP = sf::IpAddress::getPublicAddress();
const sf::IpAddress MACHINE_LOCAL_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;

Server::Server()
{
	//Initialise log system.
	ServerLog::Init();
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
		//Read requests to leave the stream.
		for (sf::Uint32 i = 0; i < mConnections.size(); ++i)
		{
			Game& gameRef = mConnections.at(i);
			Connection* connection = nullptr;
			sf::TcpSocket* socket = nullptr;
			DisconnectPCKT data;
			if (gameRef.mHost != nullptr)
			{
				connection = gameRef.mHost;
				socket = connection->GetTCPSocket();
				if (mSelect.isReady(*socket))
				{
					connection->UnpackData(data);
					if (data.quit == 1)
					{
						mSelect.remove(*connection->GetTCPSocket());
						connection->GetTCPSocket()->disconnect();
						delete gameRef.mHost;
						gameRef.mHost = nullptr;
						gameRef.mHasHost = false;
						gameRef.mHasHostAssets = false;
					}
				}
			}
			if(gameRef.mClient != nullptr)
			{
				connection = gameRef.mClient;
				socket = connection->GetTCPSocket();

				if (mSelect.isReady(*connection->GetTCPSocket()))
				{
					connection->UnpackData(data);
					if (data.quit == 1)
					{
						mSelect.remove(*connection->GetTCPSocket());
						connection->GetTCPSocket()->disconnect();
						delete gameRef.mClient;
						gameRef.mClient = nullptr;
						gameRef.mHasClient = false;
						gameRef.mHasClientAssets = false;
					}
					
				}
			}
		}
	}

	for (auto& game : mConnections)
	{
		if (game.mHasHostAssets)
		{
			if (game.mClient != nullptr)
			{
				if (!game.mClient->HasAssets())
				{
					if (game.mClient->SendAssets(game.mHostAssets))
					{
						game.mClient->SetHasAssets(true);
					}
				}
			}
		}

		if (game.mHasClientAssets)
		{
			if (game.mHost != nullptr)
			{
				if (!game.mHost->HasAssets())
				{
					if (game.mHost->SendAssets(game.mClientAssets))
					{
						game.mHost->SetHasAssets(true);
					}
				}
			}
		}
	}

//	if (mConnections.at(1) != nullptr)
//	{
//		if (!mConnections.at(1)->HasAssets())
//		{
//			if (mHasHostAssets && !(mConnections.at(1)->HasAssets()))
//			{
//				if (mConnections.at(1)->SendAssets(mHostAssets))
//				{
//					mConnections.at(1)->SetHasAssets(true);
//				}
//			}
//		}
//	}
//	if (mConnections.at(0) != nullptr)
//	{
//		if (!mConnections.at(0)->HasAssets())
//		{
//			if (mHasClientAssets && !(mConnections.at(0)->HasAssets()))
//			{
//				if (mConnections.at(0)->SendAssets(mClientAssets))
//				{
//					mConnections.at(0)->SetHasAssets(true);
//				}
//			}
//		}
//	}
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
		APP_TRACE("A new host has joined!");

		connection->SetPrivelage(ClientPrivelage::Host);
		connection->SetUDPPort(connectionData.peerUdpRecvPort);
		connection->SetNetworkID(mTotalConnections);
		connection->SetInit(true);
		connection->SetConnectionIP(connectionData.ipAddress);
		connection->SetHasAssets(false);
		mSelect.add(*connection->GetTCPSocket());

		for (int i = 0; i < mConnections.size(); ++i)
		{
			Game& gameRef = mConnections.at(i);
			if (!gameRef.mHasHost)
			{
				gameRef.mHost = connection;	
				gameRef.StoreAssetData(connectionData);
				gameRef.mHasHost = true;
				break;
			}
		}
		
		APP_TRACE("Storing host asset description.");

		mHostCount++;
		mTotalConnections++;
	}


	//A client joined.
	else if(connectionData.privelage == 1)
	{
		APP_TRACE("A new client has joined!");

		connection->SetPrivelage(ClientPrivelage::Client);
		connection->SetUDPPort(connectionData.peerUdpRecvPort);
		connection->SetNetworkID(mTotalConnections);
		connection->SetInit(true);
		connection->SetHasAssets(false);
		connection->SetConnectionIP(connectionData.ipAddress);
		mSelect.add(*connection->GetTCPSocket());
		
		APP_TRACE("Storing client asset description.");

		for (int i = 0; i < mConnections.size(); ++i)
		{
			Game& gameRef = mConnections.at(i);
			if (!gameRef.mHasClient)
			{
				gameRef.mClient = connection;
				gameRef.StoreAssetData(connectionData);
				gameRef.mHasClient = true;
				break;
			}
		}
		mClientCount++;
		mTotalConnections++;
	}


}


void Server::Run()
{

	sf::Time elapsed;
	sf::Clock clock;

	//Windows window used for rendering mGraphics to the screen.
	sf::RenderWindow window(sf::VideoMode(640, 480), "Server");

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

		//window.clear(sf::Color(125, 125, 125, 255));

		//window.display();

		appElapsedTime += 1.f * elapsed.asSeconds();
	}

}
