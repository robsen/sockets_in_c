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

	printf("Echo-Server\n");
	printf("===========\n\n");

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

	// receive data from client
	printf("Receiving Data from Client:\n");
	int numberOfRecivedBytes;
	const int recivedDataLength = 256;
	char recivedData[recivedDataLength];
	int dataSent;
	do
	{
		numberOfRecivedBytes =
			recv(
				client,
				recivedData,
				recivedDataLength,
				0
			);
		if (numberOfRecivedBytes > 0)
		{
			printf("%s\n", recivedData);

			// echo data back to client
			dataSent = send(
				client,
				recivedData,
				numberOfRecivedBytes,
				0);
			if (dataSent == SOCKET_ERROR)
			{
				PrintErrorMessage(
					"Sending Data Failed",
					"Data could not be sent to client!");
				closesocket(client);
				WSACleanup();
				exit(1);
			}
		}
		else if (numberOfRecivedBytes == CONNECTION_CLOSED)
		{
			printf("Connection closed by client.\n");
			closesocket(client);
		}
		else
		{
			PrintErrorMessage(
				"Receiving Data Failed",
				"Failed to recive data from client.");
			closesocket(client);
			WSACleanup();
			exit(1);
		}
	} while (numberOfRecivedBytes > 0);

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
