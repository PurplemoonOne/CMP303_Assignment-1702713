#pragma once
#include "../CMP303_Assignment/Application/Network/NetworkUtilities.h"

class Connection
{
public:
	Connection(sf::TcpSocket* socket);
	~Connection() = default;

	sf::TcpSocket* GetTCPSocket() { return mTCPSocket; }

	const sf::Uint16 GetTCPPort() const { return mTCPPort; }

	void SetUDPPort(sf::Uint16 port) { mUDPPort = port; }
	const sf::Uint16 GetUDPPort() const { return mUDPPort; }

	void SetConnectionIP(sf::Uint32 address) { mIPAdress = sf::IpAddress(address); }
	const sf::IpAddress& GetIPAddress() const { return mIPAdress; }
	const sf::Uint32 GetIPAddressU32() const { return mIPAdress.toInteger(); }

	const sf::Uint16 GetNetworkID() const { return mNetworkID; }
	void SetNetworkID(sf::Uint16 id) { mNetworkID = id; }


	void SetInit(bool value) { mHasBeenInitialised = value; }
	const bool HasBeenInit() const { return mHasBeenInitialised; }

	const ClientPrivelage GetConnectionPrivelage() const { return mConnPrivelage; }
	void SetPrivelage(ClientPrivelage privelage) { mConnPrivelage = privelage; }

	// @brief Send important server announcements via TCP.
	void SendTCP(DisconnectPCKT& message);
	bool SendAssets(ConnectionData& message);
	void SendTCP(ClientPortAndIP& data);

	// @brief Recieve TCP messages from connections.
	void UnpackData(DisconnectPCKT& data);
	void RecieveAssets(ConnectionData& data);
	void RecieveTCP(ClientPortAndIP& data);

	void SetHasAssets(bool value) { mHasAssets = value; }
	const bool HasAssets() const { return mHasAssets; }

private:

	// @brief A pointer to the client's socket. (sf::Socket non-copyable)
	sf::TcpSocket* mTCPSocket;

	// @brief The client's machine IP
	sf::IpAddress mIPAdress;

	// @brief Clients privelage i.e (The sorts of data they're able to send is tied to their privelage)
	ClientPrivelage mConnPrivelage;

	// @brief Special ID given by the server.
	sf::Uint16 mNetworkID;

	// @brief This connection's port number.
	sf::Uint16 mTCPPort = 0000;


	sf::Uint16 mUDPPort = 0;

	// @brief A flag to check whether this connection had been initialised.
	bool mHasBeenInitialised = false;

	// @brief Tracks if the client has assets.
	bool mHasAssets = false;
};