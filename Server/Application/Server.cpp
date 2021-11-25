#include "Server.h"
#include "../Log/ServerLog.h"

const sf::IpAddress SERVER_IP = sf::IpAddress::getLocalAddress();
const sf::Uint16 SERVER_PORT = 5555;


Server::Server()
	:
	latency(0.f),
	jitter(0.f)
{
	//Initialise log system.
	ServerLog::Init();
}

Server::~Server()
{
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

void Server::RemoveConnection(sf::Uint32 element)
{
	if (mConnections.size() == 1)
	{
		delete mConnections.at(element);
		mConnections.at(element) = nullptr;
		mConnections.resize(0);
	}
	else
	{
		//Swap the dead connection to the end and resize the array to remove it.
		for (sf::Uint32 i = element; i < (mConnections.size() - 1); ++i)
		{
			std::swap(mConnections.at(i), mConnections.at((i + 1)));
		}

		//Resize the array to the new number of connections.
		mConnections.resize(mConnections.size() - 1);
	}

	mSelect.clear();

	//Re-add the sockets that are still connected.
	for (sf::Uint32 i = 0; i < mConnections.size(); ++i)
	{
		mSelect.add(*mConnections.at(i)->GetTCPSocket());
	}

}

void Server::QueryConnections()
{
	if (mSelect.wait(sf::milliseconds(16.0f)))
	{
		if (mSelect.isReady(mListener))
		{
			sf::TcpSocket* clientSocket = new sf::TcpSocket();

			if (mListener.accept(*clientSocket) != sf::TcpListener::Done)
			{
				APP_ERROR("Accept failed!");
			}

			APP_TRACE("New connection established!");
			mConnections.push_back(new Connection(clientSocket));
			mConnections.back()->SetPrivelage(ClientPrivelage::Stream);

			//Add the connections TCP socket.
			mSelect.add(*clientSocket);

			//Add the connections UDP socket.
			mSelect.add(*mConnections.back()->GetUDPSocket());
		}

		std::vector<ChatMSG> incomingChat;

		//Read newly messages and or requests to leave the stream.
		for (sf::Uint32 i = 0; i < mConnections.size(); ++i)
		{
			if (mSelect.isReady(*mConnections.at(i)->GetTCPSocket()))
			{
				AssetData assetData;
				ChatMSG chatData;

				mConnections.at(i)->RecieveTCP(assetData, chatData);

				if (chatData.quit == 1)
				{
					RemoveConnection(i);
				}
				else
				{
					APP_TRACE("Chat message recieved : {0}", chatData.message.toAnsiString());
					mChatLog.push_back(chatData);
					incomingChat.push_back(chatData);
				}
			}
		}

		//Check if the TCP sockets are ready to send messages.
		if (incomingChat.size() > 0)
		{
			for (auto& conn : mConnections)
			{
				for (sf::Uint32 j = 0; j < incomingChat.size(); ++j)
				{
					conn->SendTCP(incomingChat.at(j));
				}
			}
		}
		
		incomingChat.clear();
	}	
}

void Server::RecievePacketsFromStreamer()
{	
	for (auto& conn : mConnections)
	{
		if (conn->GetConnectionPrivelage() == ClientPrivelage::Stream)
		{
			GameData data;
			conn->RecieveUDP(data);

			//Check the packet recieved is valid. Should never recieve a packet on 0th second.
			if (!(sizeof(data) == sizeof(GameData)))
			{
				APP_WARNING("Did not recieve a valid packet from the streamer...");

			}
			else
			{
				mMessages.push_back(data);
			}
		}
	}
}

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



void Server::Prediction(const float currentTime, sf::RectangleShape* graphics, std::vector<GameData>& messages)
{
	if (mMessages.size() >= 3)
	{
		int offset = 0;
		for (int i = 0; i < 24; ++i)
		{
			//We send 6 messages, 1 for each entity. We need atleast 2 messages for each entity to run a prediction.
			//Hence offset the messages by 6 to obtain the second message for the 'i'th entity.
			sf::Vector2f predictedPosition = LinearPrediction(mMessages.at(i), mMessages.at(i + offset));

			sf::Vector2f newPosition = sf::Vector2f
			(
				lerp(graphics[i].getPosition().x, predictedPosition.x, 0.9f),
				lerp(graphics[i].getPosition().y, predictedPosition.y, 0.9f)
			);

			graphics[i].setPosition(newPosition);
		}
		mMessages.clear();
		mMessages.resize(0);
	}
	else
	{
		APP_TRACE("Not enough messages to run predictions with.");
	}
}

void Server::GenerateAssets()
{
}

void Server::Run()
{

	sf::Time elapsed;
	sf::Clock clock;

	//Windows window used for rendering graphics to the screen.
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Server");

	float appElapsedTime = 0.f;

	// Listen for new connections.
	ListenForConections();

	mSelect.add(mListener);

	sf::RectangleShape graphics[24];
	sf::Color colours[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::White, sf::Color::Yellow, sf::Color::Magenta };

	for (int i = 0; i < 12; ++i)
	{
		graphics[i].setPosition(rand() % 1000 + 100, rand() % 1000 + 1000);
		graphics[i].setFillColor(sf::Color::Cyan);
		graphics[i].setSize(sf::Vector2f(64.0f, 64.0f));
	}
	// run the program as long as the window is open
	while (window.isOpen())
	{
		QueryConnections();

		//Process the event queue.
		ProcessEvents(window);

		//Calculate delta time.
		elapsed = clock.restart();

		if (mConnections.size() > 0)
		{
			RecievePacketsFromStreamer();

			Prediction(elapsed.asSeconds(), graphics, mMessages);
		}

		window.clear(sf::Color(125, 125, 125, 255));

		for (sf::Uint32 i = 5; i > 0; --i)
		{
			window.draw(graphics[i]);
		}

		window.display();

		appElapsedTime += 1.f * elapsed.asSeconds();
	}

}
