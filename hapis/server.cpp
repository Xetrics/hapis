#include "server.h"
#include "util.h"

void ListenThread(Proxy::Server* server)
{
	while (server->alive)
	{
		char packet = RustNetAPI::NET_Receive(server->RakNetServer);
		if (packet)
		{
			uint32_t size = RustNetAPI::NETRCV_LengthBits(server->RakNetServer) / 8;
			unsigned char* raw_data = (unsigned char*)RustNetAPI::NETRCV_RawData(server->RakNetServer);
			unsigned char id = raw_data[0];

			printf("[Server] Packet received from client, ID: %d, size: %d\n", id, size);

			if (id == NEW_INCOMING_CONNECTION)
			{
				server->incoming_guid = RustNetAPI::NETRCV_GUID(server->RakNetServer);

				server->GameServerClient = new Proxy::Client(server->target_ip, server->target_port, server);
				while (!RustNetAPI::NET_Receive(server->GameServerClient)) Sleep(10); /* wait for connection success packet */

				unsigned char client_id = ((unsigned char*)RustNetAPI::NETRCV_RawData(server->GameServerClient))[0];
				server->GameServerClient->Start();
			}
			else
			{
				// just send along the client packet to the server
				RustNetAPI::NETSND_Start(server->GameServerClient->RakNetClient);
				RustNetAPI::NETSND_WriteBytes(server->GameServerClient->RakNetClient, raw_data, size);
				RustNetAPI::NETSND_Send(server->GameServerClient->RakNetClient, server->GameServerClient->incoming_guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
			}
		}
	}
}

Proxy::Server::Server(std::string target_ip, int target_port)
{
	this->RakNetServer = RustNetAPI::NET_Create();
	this->alive = false;
	this->target_ip = target_ip;
	this->target_port = target_port;
	this->incoming_guid = 0;

	if (RustNetAPI::NET_StartServer(this->RakNetServer, "127.0.0.1", SERVER_PORT, SERVER_MAX_CONNECTIONS) != 0)
	{
		printf("[Server] ERROR: Unable to start server on port %d\n", SERVER_PORT);
		return;
	}

	printf("[Server] Listening on port %d\n", SERVER_PORT);

	this->alive = true;
}

void Proxy::Server::Start()
{
	thread = util::athread(ListenThread, this);
}

void Proxy::Server::SetGUID(uint64_t guid) {
	this->guid = guid;
}