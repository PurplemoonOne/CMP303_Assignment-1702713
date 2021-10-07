#include "UDPSocket.h"

bool UDPSocket::CreateUDPSocket()
{
	if (!(sock == INVALID_SOCKET)){
		//Error msg.
	}
	else{
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET)
			return -1;
	}
	return 0;
}

void UDPSocket::EstablishConnection(int port, char* ipAdress)
{
	// Fill out a sockaddr_in structure with the address that
	// we want to send to.
	toAddr.sin_family = AF_INET;
	// htons converts the port number to network byte order (big-endian).
	toAddr.sin_port = htons(port);
	toAddr.sin_addr.s_addr = inet_addr(ipAdress);
}

void UDPSocket::SendData(const char* bytes, int port, char* ip)
{
	if (!(sock == INVALID_SOCKET))
	{
		int count = sizeof(bytes);
		toAddr.sin_family = AF_INET;
		// htons converts the port number to network byte order (big-endian).
		toAddr.sin_port = htons(port);
		toAddr.sin_addr.s_addr = inet_addr(ip);

		char buffer[BUFFER_SIZE];
		memset(buffer, '-', BUFFER_SIZE);
		memcpy(buffer, bytes, min(count, BUFFER_SIZE));

		if (sendto(sock, buffer, BUFFER_SIZE, 0,
			(const sockaddr*)&toAddr, sizeof(toAddr)) != BUFFER_SIZE)
		{ return; }
	}
}
