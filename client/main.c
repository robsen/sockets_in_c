// run:	gcc main.c -o client.exe -lwsock32
#include <stdio.h>
#include <winsock.h>

#define WSA_SUCCESS 0

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
	if (errorCode != 0)
		return 1;

	WSACleanup();

	return 0;
}
