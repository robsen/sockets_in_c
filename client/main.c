// run:	gcc main.c -o client.exe -lwsock32
// DLL:	Wsock32.dll (Microsoft Windows 32-bit Winsock 1.1)
#include <stdio.h>
#include <winsock.h>

#define WSA_SUCCESS 0

// temporary configuration data for remote socket
#define PORT 80
#define IP_ADDRESS "192.168.1.1"

int main(int argc, char** argv)
{
	WORD requestedVersion = MAKEWORD(1, 0);
	WSADATA wsaData;
	int errorCode;

	printf("Client\n");
	printf("======\n\n");

	errorCode = WSAStartup(requestedVersion, &wsaData);
	switch (errorCode)
	{
		case WSA_SUCCESS:
			break;

		// errors:
		case WSASYSNOTREADY:
			// TODO: Create error-printing function for a consistent error output format
			printf("The underlying network subsystem is not ready for network communication.\n");
			break;

		case WSAVERNOTSUPPORTED:
			printf("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.\n");
			break;

		case WSAEINPROGRESS:
			printf("A blocking Windows Sockets 1.1 operation is in progress.\n");
			break;

		case WSAEPROCLIM:
			printf("A limit on the number of tasks supported by the Windows Sockets implementation has been reached.\n");
			break;

		case WSAEFAULT:
			printf("The lpWSAData parameter is not a valid pointer.\n");
			break;

		default:
			printf("Unexpected error (%d) at \"WSAStartup\"\n", errorCode);
			
			// TODO: Use function "FormatMessage()" for getting error message of all other errors
	}
	if (errorCode != WSA_SUCCESS)
		return 1;

	// get Windows Socket
	SOCKET socketDescriptor = INVALID_SOCKET;
	socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketDescriptor == INVALID_SOCKET)
	{
		// TODO: Create error-printing function for a consistent error output format
		printf("[ERROR] Invalid Socket Descriptor.\n");
		printf("It was not possible to establish a Windows-Socket communication.\n");
		printf("Error-Code: %d\n", WSAGetLastError());
		printf("[END-ERROR]\n");

		WSACleanup();
		exit(-1);

		// TODO: Create a switch-listing with all possible errors for useablity and human error-handling
	}

	// specify remote socket address
	struct sockaddr_in remoteSocketAddress;
	remoteSocketAddress.sin_family = AF_INET;
	// port & IP needs to be in network byte order (big-endian)
	remoteSocketAddress.sin_port = htons(PORT);
	remoteSocketAddress.sin_addr.S_un.S_addr = inet_addr(IP_ADDRESS);

	// establish connection to remote socket
	errorCode = connect(socketDescriptor,
		(struct sockaddr*)(&remoteSocketAddress),
		sizeof(remoteSocketAddress));
	if (errorCode != WSA_SUCCESS)
	{
		// TODO: Create error-printing function for a consistent error output format
		printf("[ERROR] Remote Socket Connection Failed!\n");
		printf("Error-Code: %d\n", WSAGetLastError());

		closesocket(socketDescriptor);
		WSACleanup();
		exit(-1);

		// TODO: Create a switch-listing with all possible errors for useablity and human error-handling
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
		dataRemaining = dataToSend - dataSent;
		if (dataRemaining == SOCKET_ERROR)
		{
			// TODO: Create error-printing function for a consistent error output format
			printf("[ERROR] Data could not be sent!\n");
			printf("Error-Code: %d\n", WSAGetLastError());

			closesocket(socketDescriptor);
			WSACleanup();
			exit(-1);

			// TODO: Create a switch-listing with all possible errors for useablity and human error-handling
		}
		// next data chunk
		data += sizeof(char) * dataSent;
	}

	printf("Data has been sent.\n");
	closesocket(socketDescriptor);
	WSACleanup();

	return 0;
}
