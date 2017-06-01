#include "client.h"
#include "util.h"

void ListenThread(Proxy::Client* client)
{
	while (client->is_connected)
	{
		while (RustNetAPI::NET_Receive(client->pointer))
		{
			uint32_t size = RustNetAPI::NETRCV_LengthBits(client->pointer) / 8;
			unsigned char* data = (unsigned char*)RustNetAPI::NETRCV_RawData(client->pointer);

			printf("[Client] Packet received from game server, ID: %d, size: %d\n", data[0], size);

			// TODO: handle client disconnecting from game server ? (ID_DISCONNECTION_NOTIFICATION, ID_CONNECTION_LOST, etc)

			if (data[0] == ID_DISCONNECTION_NOTIFICATION)
			{
				printf("[Client] Disconnection notification received, disconnecting...\n");
				client->server->Close();
				client->Close();
				return;
			}

			/* game server sent a packet to proxy, forward to client */
			client->server->Send(data, size);
		}

		Sleep(PROXY_TICK_MS);
	}
}

Proxy::Client::Client(std::string target_ip, int target_port, Proxy::Server* server)
{
	this->pointer = RustNetAPI::NET_Create();
	this->is_connected = false;
	this->target_ip = target_ip;
	this->target_port = target_port;
	this->server = server;

	if (RustNetAPI::NET_StartClient(this->pointer, this->target_ip.c_str(), (uint16_t)target_port, CLIENT_MAX_RETRIES, CLIENT_RETRY_DELAY, CLIENT_TIMEOUT) != 0)
	{
		printf("[Client] Unable to connect to server %s:%d\n", this->target_ip.c_str(), target_port);
		return;
	}

	printf("[Client] Client started, attempt made to connect to game server: %s:%d\n", this->target_ip.c_str(), target_port);
}

Proxy::Client::~Client()
{
	Close();
}

void Proxy::Client::Start()
{
	thread = util::athread(ListenThread, this);
}

void Proxy::Client::Send(unsigned char* data, uint32_t size)
{
	if (!this->incoming_guid) return;
	RustNetAPI::NETSND_Start(this->pointer);
	RustNetAPI::NETSND_WriteBytes(this->pointer, data, size);
	RustNetAPI::NETSND_Send(this->pointer, this->incoming_guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
}

void Proxy::Client::Close()
{
	RustNetAPI::NET_Close(this->pointer);
	this->pointer = 0;
	this->is_connected = false;
	this->incoming_guid = 0;
}