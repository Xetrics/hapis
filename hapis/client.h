#pragma once
#include <string>

#include "rrapi.h"
#include "server.h"
#include "util.h"

#define CLIENT_MAX_RETRIES 32
#define CLIENT_RETRY_DELAY 100
#define CLIENT_TIMEOUT 2500

namespace Proxy {
	class Server; // forward declaration

	class Client
	{
	public:
		Client(std::string target_ip, int target_port, Proxy::Server* server);
		~Client();
		Client(const Client& other) = delete;

		void Send(unsigned char* data, uint32_t size);
		uint32_t Tick();
		void Close();
		
		Rust::API::RakPeer pointer;
		Proxy::Server* server;

		std::string target_ip;
		int target_port;
		bool is_connected;
		uint64_t incoming_guid; /* the identifier for the server we are connected to (rust server) */
	};
}