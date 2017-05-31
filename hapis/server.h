#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <string>

#include "rrapi.h"
#include "client.h"

#define SERVER_PORT 5678
#define SERVER_MAX_CONNECTIONS 10
#define NEW_INCOMING_CONNECTION 19
#define SERVER_PACKET_PRIORITY 2
#define SERVER_PACKET_RELIABILITY 0
#define SERVER_PACKET_CHANNEL 0

namespace Proxy {

	class Server {
	public:
		RustNetAPI::RakPeer RakNetServer;
		Proxy::Client* GameServerClient;
		void listen();
		void SetGUID(uint64_t);
		Server(std::string target_ip, int target_port);

		bool alive;
		std::string target_ip;
		int target_port;
		unsigned char* rcvBuf;
		uint64_t guid;
	};

}