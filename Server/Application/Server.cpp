#include "Server.h"
#include "../Log/ServerLog.h"


const sf::IpAddress SERVER_PUBLIC_IP = sf::IpAddress::getPublicAddress();
const sf::IpAddress MACHINE_LOCAL_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;

Server::Server()
	:
	mLatency(0.f),
	mJitter(0.f),
	mHasAssets(false)
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
		//Read newly messages and or requests to leave the stream.
		for (sf::Uint32 i = 0; i < mConnections.size(); ++i)
		{
			//Element 0 is reserved for host. Hence element 0 is NULL if no host.
			if (mConnections.at(i) != nullptr)
			{
				if (mSelect.isReady(*mConnections.at(i)->GetTCPSocket()))
				{
					DisconnectPCKT chatData;
					mConnections.at(i)->RecieveTCP(chatData);

					if (chatData.quit == 1)
					{
						RemoveConnection(i);
						continue;//Move onto the next connection.
					}
				}
			}
		}
	}

	if (mHasAssets)
	{
		//Check if the client needs assets.
		for (sf::Uint32 i = 0; i < mConnections.size(); ++i)
		{
			if (mConnections.at(i) != nullptr)
			{
				//If the client is still waiting for assets send them now.
				if (!mConnections.at(i)->HasAssets())
				{
					if (mConnections.at(i)->SendTCP(mAssets))
					{
						mConnections.at(i)->SetHasAssets(true);
					}
				}
			}
		}
	}
}


void Server::RemoveConnection(sf::Uint32 element)
{
	sf::Uint16 port = mConnections.at(element)->GetTCPPort();

	if (mConnections.at(element)->GetConnectionPrivelage() == ClientPrivelage::Host)
	{
		mHasAssets = false;
		mHostCount--;
	}
	else
	{
		mClientCount--;
	}
	mTotalConnections--;

	//Remove the reference to the socket from the select object.
	mSelect.remove(*mConnections.at(element)->GetTCPSocket());

	//Close the TCP socket.
	mConnections.at(element)->GetTCPSocket()->disconnect();

	//Delete the socket object from the heap.
	delete mConnections.at(element)->GetTCPSocket();

	//Delete the connection object from the array.
	if (element == 0)
	{
		delete mConnections.at(element);
		mConnections.at(element) = nullptr;
	}
	else
	{
		//Swap the dead connection to the end. (O(N))
		for (sf::Uint32 i = element; i < (mConnections.size() - 1); ++i)
		{
			std::swap(mConnections.at(i), mConnections.at((i + 1)));
		}

		delete mConnections.back();
		mConnections.back() = nullptr;
	}

	//Resize the array to the new number of connections.
	mConnections.resize(mTotalConnections);

	APP_TRACE("Connection on port {0} has been closed.", port);
}

void Server::InitConnection(sf::TcpSocket* socket)
{
	//Create a new connection object.
	Connection* connection = new Connection(socket);

	//Gather information describing this connection.
	ConnectionData connectionData;
	connection->RecieveTCP(connectionData);

	// If '1' then connection is a client.
	if (connectionData.privelage == 0)
	{
		//If the number of host's is not greater than 1.
		if (mHostCount < 1)
		{
			APP_TRACE("A new host has joined!");

			connection->SetPrivelage(ClientPrivelage::Host);

			mHostCount++;

			connection->SetUDPPort(connectionData.peerUdpRecvPort);

			//Streamer gets special ID.
			connection->SetNetworkID(0);

			//Add the streamers sockets to the select.
			mSelect.add(*connection->GetTCPSocket());

			//insert the streamer at the front of the array.
			mConnections.push_back(connection);

			//Finally set the streamer to initialised.
			connection->SetInit(true);

			//incriment total connections.
			mTotalConnections++;

			connection->SetHasAssets(true);

			APP_TRACE("Generating assets...");

			StoreClientAssetData(connectionData);
			
			APP_TRACE("Generating assets completed!");
		}

		//Another host attempted to join the server.
		else
		{
			//Send a message to the client attepted to host and close connection.
			APP_WARNING("Another connection attempted to host!");
			DisconnectPCKT connectionData;
			connectionData.message = "The server has too many hosts, try again later.";
			connectionData.id = 0;
			connectionData.time = time(0);
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
		APP_TRACE("A new client has joined!");

		connection->SetPrivelage(ClientPrivelage::Client);

		connection->SetUDPPort(connectionData.peerUdpRecvPort);

		connection->SetNetworkID(mTotalConnections);

		mConnections.push_back(connection);
	
		//Add the streamers sockets to the select.
		mSelect.add(*connection->GetTCPSocket());

		//Finally set the streamer to initialised.
		connection->SetInit(true);

		connection->SetHasAssets(false);


		mClientCount++;
		mTotalConnections++;
	}

	//Something went wrong.
	else
	{
		delete connection;
		connection = nullptr;
		APP_WARNING("Unknown entity has joined... Resolving.");
	}

}

/*
sf::Vector2f Server::LinearPrediction(const GameData& messageA, const GameData& messageB)
{
	float dt = messageA.time - messageB.time;

	float vX = (messageA.x - messageB.x) /dt;
	float vY = (messageA.y - messageB.y) /dt;
	
	return { messageA.x + vX * dt, messageA.y + vY * dt };
}

sf::Vector2f Server::QuadraticPrediction(const GameData& messageA, const GameData& messageB, const GameData& messageC)
{

	float dtAB = messageA.time - messageB.time;
	float dtBC = messageB.time - messageC.time;

	float vX = (messageA.x - messageB.x) / dtAB;
	float vY = (messageA.y - messageB.y) / dtAB;
	float aX = (messageA.x - messageB.x) / dtAB - (messageB.x - messageC.x) / dtBC;
	float aY = (messageA.y - messageB.y) / dtAB - (messageB.y - messageC.y) / dtBC;
	float sX = (messageA.x - messageB.x) / dtAB;
	float sY = (messageA.y - messageB.y) / dtAB;

	return { messageA.x + vX * dtAB + 0.5f * aX * dtBC, messageA.y + vY * dtAB + 0.5f * aY * dtBC };
}

void Server::Prediction(std::vector<sf::RectangleShape>& mGraphics, std::vector<GameData>& messages)
{
	if (mGameData.size() >= (mGraphics.size() * 2))
	{
		//Sort packets into order.
		std::sort(mGameData.begin(), mGameData.end(), GamePacketIDSort());

		//For linear we need at least two packets of data to determine position.
		//After (recieving 2 * numberOfEntities) we can begin to predict new positions.
		int offset = mGraphics.size();
		for (int i = 0; i < mGraphics.size(); ++i)
		{
			if (!((i + offset) < mGameData.size()))
			{
				continue;
			}
			else
			{
				//Obtain the predicted posistion.
				sf::Vector2f predictedPosition = LinearPrediction(mGameData.at(i), mGameData.at(i + offset));

				sf::Vector2f newPosition = sf::Vector2f
				(
					//Linear interpolate between the last position and new position by 60%
					lerp(mGraphics[i].getPosition().x, predictedPosition.x, 0.6f),
					lerp(mGraphics[i].getPosition().y, predictedPosition.y, 0.6f)
				);

				mGraphics[i].setPosition(newPosition);
			}
		}
		mGameData.clear();
		mGameData.resize(0);
	}
}
*/

void Server::StoreClientAssetData(ConnectionData& data)
{
	mAssets.count = data.count;
	mAssets.type = data.type;
	mAssets.sizeX = data.sizeX;
	mAssets.sizeY = data.sizeY;
	mAssets.ipAddress = data.ipAddress;
	mAssets.peerUdpRecvPort = data.peerUdpRecvPort;
	mHasAssets = true;
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
