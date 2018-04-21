#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <winsock.h>
#include "network_error.h"

#define CONNECTION_CLOSED 0


void InitializeWinSocketDLL_orDie(
	WSADATA* socketInformation);

SOCKET CreateSocket_orDie();

void CreateRemoteAddress(
	struct sockaddr*
	 remoteAddress,
	char* ipV4,
	unsigned short port);

void EstablishConnection_orDie(
	SOCKET socket,
	char* ipV4,
	unsigned short port);

void BindSocket_orDie(
	SOCKET socketToBind,
	unsigned short port);

void ListenForRequests_orDie(
	SOCKET listening);
#endif
