// run:	gcc main.c ../shared/network.c ../shared/network_error.c -o client.exe -lwsock32
// DLL:	Wsock32.dll (Microsoft Windows 32-bit Winsock 1.1)
#include <stdio.h>
#include <winsock.h>
#include "../shared/network.h"


// temporary configuration data for remote socket
#define PORT 80
#define IP_ADDRESS "127.0.0.1"

int main(int argc, char** argv)
{
	printf("Client\n");
	printf("======\n\n");

	WSADATA wsaData;
	InitializeWinSocketDLL_orDie(
		&wsaData);

	SOCKET network =
		CreateSocket_orDie();

	EstablishConnection_orDie(
		network,
		IP_ADDRESS,
		PORT);

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
