// run:	gcc main.c ../shared/network.c ../shared/network_error.c -o client.exe -lwsock32
// DLL:	Wsock32.dll (Microsoft Windows 32-bit Winsock 1.1)
#include <stdio.h>
#include <winsock.h>
#include "../shared/network.h"


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

	printf("Client\n");
	printf("======\n\n");

	WSADATA wsaData;
	InitializeWinSocketDLL_orDie(
		&wsaData);

	SOCKET network =
		CreateSocket_orDie();

	EstablishConnection_orDie(
		network,
		serverIP,
		serverPort);

	// send data
	char* data = "Hello, World!";
	int dataToSend = sizeof(data);
	int dataSent = 0;
	int dataRemaining = dataToSend;
	while (dataRemaining)
	{
		dataSent = send(
			network,
			data,
			dataToSend,
			MSG_OOB);
		if (dataSent == SOCKET_ERROR)
		{
			PrintErrorMessage(
				"Sending Data Failed",
				"Data could not be sent!");
			closesocket(network);
			WSACleanup();
			exit(1);
		}

		dataRemaining = dataToSend - dataSent;
		// next data chunk
		data += sizeof(char) * dataSent;
	}
	printf("Data has been sent.\n");

	// stop socket from sending
	int hasFailed =
		shutdown(
			network,
			SD_SEND
		);
	if (hasFailed)
	{
		PrintErrorMessage(
			"Connection Shutdown Failed",
			"Could not shutdown the "
			 "sending connection.");
		closesocket(network);
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
				network,
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
			closesocket(network);
			WSACleanup();
			exit(1);
		}
	} while (numberOfRecivedBytes > 0);

	closesocket(network);
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
