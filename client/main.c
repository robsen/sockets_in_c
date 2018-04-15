// run:	gcc main.c network.c -o client.exe -lwsock32
// DLL:	Wsock32.dll (Microsoft Windows 32-bit Winsock 1.1)
#include <stdio.h>
#include <winsock.h>
#include "network.h"


// temporary configuration data for remote socket
#define PORT 80
#define IP_ADDRESS "192.168.1.1"


void ExtractIPAndPort(
	char* argument,
	char* ip,
	int* port);


int main(int argc, char** argv)
{
	// check correct program usage
	if (argc != 2)
	{
		PrintErrorMessage(
			"Invalid Argument",
			"Usage of this program:\n"
			 "client.exe ip:port\n"
			 "Example:\tclient.exe 127.0.0.1:60000");
		exit(1);
	}

	// get IP and port
	char serverIP[16] = {0};
	int serverPort;
	ExtractIPAndPort(
		argv[1],
		serverIP,
		&serverPort);

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
	remoteSocketAddress.sin_port = htons(serverPort);
	remoteSocketAddress.sin_addr.S_un.S_addr = inet_addr(serverIP);

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

	// recive data from server
	int numberOfRecivedBytes;
	const int recivedDataLength = 256;
	char recivedData[recivedDataLength];
	do
	{
		numberOfRecivedBytes =
			recv(
				socketDescriptor,
				recivedData,
				recivedDataLength,
				0
			);
		if (numberOfRecivedBytes > 0)
			printf(recivedData);
		else if (numberOfRecivedBytes == CONNECTION_CLOSED)
			printf("Connection closed by server.\n");
		else
		{
			PrintErrorMessage(
				"Receiving Data Failed",
				"Failed to recive data.");
			closesocket(socketDescriptor);
			WSACleanup();
			exit(1);
		}
	} while (numberOfRecivedBytes > 0);

	closesocket(socketDescriptor);
	WSACleanup();

	return 0;
}


void ExtractIPAndPort(
	char* argument,
	char* ip,
	int* port)
{
	int i = 0;
	while(
		argument[i] != ':' &&
		argument[i] != '\0')
	{
		if (i == 15)
			break;
		ip[i] = argument[i];
		i++;
	}

	char* ptr;
	*port =
		(int)strtol(
			&argument[i+1], // start behind 'ip:'
			&ptr,
			10
		);
}
