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
#define PROXY_TICK_MS 20

#define CONNECTION_REQUEST_ACCEPTED 16
#define NEW_INCOMING_CONNECTION 19
#define ID_DISCONNECTION_NOTIFICATION 21
#define ID_CONNECTION_LOST 22

#define SERVER_PACKET_PRIORITY 2
#define SERVER_PACKET_RELIABILITY 3 // RELIABLE_ORDERED
#define SERVER_PACKET_CHANNEL 0

#define TICK_NO_MORE_PACKETS	(1 << 1)
#define TICK_DISCONNECT			(1 << 2)

namespace Proxy {
	class Client; // forward declaration

	class Server
	{
	private:
		util::athread thread;

	public:
		Server(std::string target_ip, int target_port);
		~Server();
		Server(const Server& other) = delete;

		void Start();
		void Send(unsigned char* data, uint32_t size);
		uint32_t Tick();
		void Close();

		Rust::API::RakPeer pointer;
		Proxy::Client* client;

		bool is_alive;
		std::string target_ip;
		int target_port;
		uint64_t incoming_guid; /* the identifier for the client we are connected to (our pc) */
	};

}