#include "Server.h"
#include "../Log/Log.h"

#define SERVER_PORT 5555

Server::Server()
	:
	latency(0.f),
	jitter(0.f)
{
	//Initialise log system.
	Logger::Init();

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

void Server::RecieveAssetList()
{
	AssetPacket packet;

	if (mSelect.wait(sf::microseconds(16.0f)))
	{
		for (auto& conn : mConnections)
		{
			if (conn->GetConnectionPrivelage() == ClientPrivelage::Stream)
			{
				if (mSelect.isReady(*conn->GetTCPSocket()))
				{
					conn->
				}
			}
		}
	}

}

void Server::RecieveIncomingPackets()
{
}

void Server::Prediction()
{
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

void Server::CloseConnections()
{

}

void Server::AcceptNewConnections()
{
	if (mSelect.wait())
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
			mSelect.add(*clientSocket);
		}

	}
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

	// run the program as long as the window is open
	while (window.isOpen())
	{
		AcceptNewConnections();

		//Process the event queue.
		ProcessEvents(window);

		//Calculate delta time.
		elapsed = clock.restart();

		window.clear(sf::Color(255, 0, 255, 255));

		window.display();

		appElapsedTime += 1.f * elapsed.asSeconds();
	}


	CloseConnections();

}
