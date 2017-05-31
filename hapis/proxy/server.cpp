#include <stdio.h>

#include "server.h"
#include "../rrapi.h"

Proxy::Server::Server() {
	this->RakNetServer = RustNetAPI::NET_Create();
	RustNetAPI::NET_StartServer(this->RakNetServer, "0.0.0.0", SERVER_PORT, SERVER_MAX_CONNECTIONS);
	printf("Server listening on :%n", SERVER_PORT);
}	