// run:	gcc main.c network.c -o client.exe -lwsock32
// DLL:	Wsock32.dll (Microsoft Windows 32-bit Winsock 1.1)
#include <stdio.h>
#include <winsock.h>
#include "network.h"


// temporary configuration data for remote socket
#define PORT 80
#define IP_ADDRESS "192.168.1.1"

int main(int argc, char** argv)
{
	WSADATA wsaData;
	WORD requestedVersion =
		MAKEWORD(1, 0);
	int hasFailed;

	printf("Client\n");
	printf("======\n\n");

	hasFailed =
		WSAStartup(
			requestedVersion,
			&wsaData
	);
	if (hasFailed)
		PrintErrorAndExit(
			"Failed to Initialize Winsocket DLL");

	// get Windows Socket
	SOCKET socketDescriptor = INVALID_SOCKET;
	socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketDescriptor == INVALID_SOCKET)
	{
		PrintErrorMessage(
			"Invalid Socket Descriptor",
			"It was not possible to establish "
			 "a Windows-Socket communication.");
		WSACleanup();
		exit(1);
	}

	// specify remote socket address
	struct sockaddr_in remoteSocketAddress;
	remoteSocketAddress.sin_family = AF_INET;
	// port & IP needs to be in network byte order (big-endian)
	remoteSocketAddress.sin_port = htons(PORT);
	remoteSocketAddress.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);

	// establish connection to remote socket
	hasFailed =
		connect(
			socketDescriptor,
			(struct sockaddr*)(&remoteSocketAddress),
			sizeof(remoteSocketAddress)
		);
	if (hasFailed)
	{
		PrintErrorMessage(
			"Remote Socket Connection Failed!",
			"Could not connect to server.");
		closesocket(socketDescriptor);
		WSACleanup();
		exit(1);
	}

	// send data
	char* data = "Hello, World!";
	int dataToSend = sizeof(data);
	int dataSent = 0;
	int dataRemaining = dataToSend;
	while (dataRemaining)
	{
		dataSent = send(
			socketDescriptor,
			data,
			dataToSend,
			MSG_OOB);
		if (dataSent == SOCKET_ERROR)
		{
			PrintErrorMessage(
				"Sending Data Failed",
				"Data could not be sent!");
			closesocket(socketDescriptor);
			WSACleanup();
			exit(1);
		}

		dataRemaining = dataToSend - dataSent;
		// next data chunk
		data += sizeof(char) * dataSent;
	}
	printf("Data has been sent.\n");

	// stop socket from sending
	hasFailed =
		shutdown(
			socketDescriptor,
			SD_SEND
		);
	if (hasFailed)
	{
		PrintErrorMessage(
			"Connection Shutdown Failed",
			"Could not shutdown the "
			 "sending connection.");
		closesocket(socketDescriptor);
		WSACleanup();
		exit(1);
	}

	closesocket(socketDescriptor);
	WSACleanup();

	return 0;
}
