#include "network.h"


void InitializeWinSocketDLL_orDie(
	WSADATA* socketInformation)
{
	const WORD
	 socketVersion =
		MAKEWORD(1, 0);
	int hasFailed =
		WSAStartup(
			socketVersion,
			socketInformation
	);
	if (hasFailed)
		PrintErrorAndExit(
			"Failed to Initialize Winsocket DLL");
}

SOCKET CreateSocket_orDie()
{
	SOCKET newSocket =
		socket(
			AF_INET,
			SOCK_STREAM,
			IPPROTO_TCP
		);
	if (newSocket == INVALID_SOCKET)
	{
		PrintErrorMessage(
			"Invalid Socket Descriptor",
			"It was not possible to establish "
			 "a Windows-Socket communication.");
		WSACleanup();
		exit(1);
	}
	return newSocket;
}

void CreateRemoteAddress(
	struct sockaddr*
	 remoteAddress,
	char* ipV4,
	unsigned short port)
{
	((struct sockaddr_in*)
	 remoteAddress)->sin_family =
		AF_INET;
	// port & IP needs to be in network byte order (big-endian)
	((struct sockaddr_in*)
	 remoteAddress)->sin_port =
		htons(port);
	((struct sockaddr_in*)
	 remoteAddress)->sin_addr.S_un.S_addr =
		inet_addr(ipV4);
}

void EstablishConnection_orDie(
	SOCKET socket,
	char* ipV4,
	unsigned short port)
{
	struct sockaddr
	 remoteAddress;
	CreateRemoteAddress(
		&remoteAddress,
		ipV4,
		port);

	int hasFailed =
		connect(
			socket,
			&remoteAddress,
			sizeof(remoteAddress)
		);
	if (hasFailed)
	{
		PrintErrorMessage(
			"Remote Socket Connection Failed!",
			"Could not connect to server.");
		closesocket(socket);
		WSACleanup();
		exit(1);
	}
}
