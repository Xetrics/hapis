#pragma once
#include <string>

#include "rrapi.h"
#include "server.h"

#define CLIENT_MAX_RETRIES 32
#define CLIENT_RETRY_DELAY 100
#define CLIENT_TIMEOUT 100

namespace Proxy {
	class Client {
	public:
		RustNetAPI::RakPeer RakNetClient;
		Proxy::Server* ProxyServer;
		void listen();
		Client(std::string target_ip, int target_port, uint64_t guid, Proxy::Server* server);
		
		std::string target_ip;
		int target_port;
		bool connected;
		unsigned char* rcvBuf;
		uint64_t guid;
	};
}