#pragma once
#include "../CMP303_Assignment/Application/Network/NetworkUtilities.h"

class Connection
{
public:
	Connection(sf::TcpSocket* socket);
	~Connection() = default;

	sf::TcpSocket* GetTCPSocket() { return mTCPSocket; }
	sf::UdpSocket* GetUDPSocket() { return &mUDPSocket; }

	const sf::Uint16 GetTCPPort() const { return mTCPPort; }
	const sf::Uint16 GetUDPPort() const { return mUDPPort; }

	const sf::IpAddress& GetIPAddress() const { return mIPAdress; }

	const ClientPrivelage GetConnectionPrivelage() const { return mConnPrivelage; }
	void SetPrivelage(ClientPrivelage privelage) { mConnPrivelage = privelage; }

	// @brief Send important server announcements via TCP.
	void SendTCP(ChatMSG& message);

	// @brief Send game updates to all spectaters.
	void SendUDP(const GameData& gameData, const sf::Uint16 toPort, const sf::IpAddress& toIpAddress);

	// @brief Recieve all kinds of data from connections.
	void RecieveTCP(AssetData& assetData, ChatMSG& message);

	// @brief Recieve all kinds of data from connections.
	void RecieveUDP(GameData& gameData);

private:

	// @brief A pointer to the client's socket. (sf::Socket non-copyable)
	sf::TcpSocket* mTCPSocket;

	// @brief Connection UDP socket used to recieve updates about the game state.
	sf::UdpSocket mUDPSocket;

	// @brief The client's machine IP
	sf::IpAddress mIPAdress;

	// @brief Clients privelage i.e (The sorts of data they're able to send is tied to their privelage)
	ClientPrivelage mConnPrivelage;

	// @brief This connection's port number.
	sf::Uint16 mTCPPort = 0000, mUDPPort = 0000;
};