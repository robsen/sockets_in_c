// run:	gcc main.c ../shared/network.c ../shared/network_error.c -o server.exe -lwsock32
// DLL:	Wsock32.dll (Microsoft Windows 32-bit Winsock 1.1)
#include <stdio.h>
#include <winsock.h>
#include "../shared/network.h"


// some gcc versions have SD_SEND not defined
#ifndef SD_SEND
#define SD_SEND 1
#endif


void PrintUsage_andDie();

int IsNumber(
	char* string);

unsigned short ExtractPort_orDie(
	char* argument);


int main(int argc, char** argv)
{
	// check correct program usage
	if (argc != 2)
		PrintUsage_andDie();

	unsigned short port =
		ExtractPort_orDie(
			argv[1]);

	printf("Server\n");
	printf("======\n\n");

	WSADATA wsaData;
	InitializeWinSocketDLL_orDie(
		&wsaData);

	SOCKET network =
		CreateSocket_orDie();

	BindSocket_orDie(
		network,
		port);

	ListenForRequests_orDie(
		network);

	SOCKET client;
	ConnectToClient_orDie(
		network,
		&client);
	closesocket(
		network);

	// send data
	/*
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
	*/

	closesocket(network);
	WSACleanup();

	return 0;
}

void PrintUsage_andDie()
{
	PrintErrorMessage(
		"Invalid Argument",
		"Usage of this program:\n"
		 "server.exe port\n"
		 "Example:\tserver.exe 60000");
	exit(1);
}

int IsNumber(
	char* string)
{
	for(
		int i = 0;
		i < 6 && // length of digits in short.max + \0
		string[i] != '\0';
		i++
	)
		if (string[i] < '0' ||
			string[i] > '9')
			return 0; // 0 inidcates false

	return 1; // 1 indicates true
}

unsigned short ExtractPort_orDie(
	char* argument)
{
	unsigned int port;
	char* ptr;

	if (IsNumber(argument))
	{
		port = strtol(
			argument,
			&ptr,
			10);
		return port;
	}

	PrintUsage_andDie();
}
