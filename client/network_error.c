#include "network_error.h"
#include <stdio.h>
#include <winsock.h>


void PrintErrorMessage(
	char* title,
	char* message)
{
	const int errorCode =
		WSAGetLastError();
	printf("[Error] %s:\n", title);
	printf(message);
	printf("\n");
	printf(
		"Error-Code: %d (0x%X)\n",
		errorCode,
		errorCode);
}

void PrintErrorAndExit(
	char* title)
{
	char* message;
	const int errorCode =
		WSAGetLastError();
	switch (errorCode)
	{
		case WSASYSNOTREADY:
			message =
				"The underlying network subsystem "
				 "is not ready for network communication.";
			break;

		case WSAVERNOTSUPPORTED:
			message =
				"The version of Windows Sockets support "
				 "requested is not provided by this "
				 "particular Windows Sockets implementation.";
			break;

		case WSAEINPROGRESS:
			message =
				"A blocking Windows Sockets 1.1 "
				 "operation is in progress.";
			break;

		case WSAEPROCLIM:
			message =
				"A limit on the number of tasks supported "
				 "by the Windows Sockets implementation "
				 "has been reached.";
			break;

		case WSAEFAULT:
			message =
				"The lpWSAData parameter is not "
				 "a valid pointer.";
			break;

		default:
			message =
				"Unexpected error.";
	}

	PrintErrorMessage(
		title,
		message);
	exit(1);
}
