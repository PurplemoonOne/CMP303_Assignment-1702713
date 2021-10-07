#pragma once

#ifdef WIN32
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <winsock2.h>

	#pragma comment(lib, "ws2_32.lib")
#else
	error platform not supported!
#endif


#define BUFFER_SIZE 40

#ifdef _DEBUG
	#define ASSERT(expr, ...) if(!expr){printf(...); __debugbreak();}
#endif

class UDPSocket
{
public:
	UDPSocket() = default;
	UDPSocket(const UDPSocket&) = default;
	~UDPSocket() = default;

	bool CreateUDPSocket();
	inline void EstablishConnection(int port, char* ipAdress);
	void SendData(const char* bytes, int port, char* ip);
	const SOCKET GetSocket() const { return sock; }
	inline void CloseSocket() const { closesocket(sock); }
private:
	SOCKET sock;
	mutable sockaddr_in toAddr, fromAddr;
};

