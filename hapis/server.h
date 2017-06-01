#pragma once
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <string>

#include "rrapi.h"
#include "client.h"
#include "util.h"

#define SERVER_PORT 5678
#define SERVER_MAX_CONNECTIONS 10
#define NEW_INCOMING_CONNECTION 19
#define SERVER_PACKET_PRIORITY 2
#define SERVER_PACKET_RELIABILITY 0
#define SERVER_PACKET_CHANNEL 0

namespace Proxy {
	class Client; // forward declaration

	class Server
	{
	private:
		util::athread thread;

	public:
		Server(std::string target_ip, int target_port);
		void Start();
		void SetGUID(uint64_t);

		RustNetAPI::RakPeer RakNetServer;
		Proxy::Client* GameServerClient;

		bool alive;
		std::string target_ip;
		int target_port;
		uint64_t incoming_guid; /* the identifier for the client we are connected to (our pc) */
	};

}