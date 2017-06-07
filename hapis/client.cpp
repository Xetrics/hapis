#include "client.h"
#include "util.h"
#include "main.h"
#include "message.h"

uint32_t Proxy::Client::Tick()
{
	if (!this->is_connected)
		return TICK_DISCONNECT;

	if (!Rust::API::NET_Receive(this->pointer))
		return TICK_NO_MORE_PACKETS;

	uint32_t size = Rust::API::NETRCV_LengthBits(this->pointer) / 8;
	unsigned char* data = (unsigned char*)Rust::API::NETRCV_RawData(this->pointer);

	//printf("[Client] Packet received from game server, ID: %d (%s), size: %d\n", data[0], Rust::Message::TypeToName((Rust::MessageType)data[0]), size);

	switch (data[0])
	{
	case ID_DISCONNECTION_NOTIFICATION:
		printf("[Client] Disconnection notification received from game server, disconnecting...\n");
		this->server->Close();
		this->Close();
		return TICK_DISCONNECT;
	case ID_CONNECTION_LOST:
		printf("[Client] Connection to game server lost, disconnecting from client and closing...\n");
		this->server->is_alive = false;
		this->server->Send(data, size);
		this->Close();
		return TICK_DISCONNECT;
	}

	/* game server sent a packet to proxy, forward to client */
	OnRustPacketReceived(this, data, size);
	this->server->Send(data, size);

	return 0;
}

Proxy::Client::Client(std::string target_ip, int target_port, Proxy::Server* server)
{
	this->pointer = Rust::API::NET_Create();
	this->is_connected = false;
	this->target_ip = target_ip;
	this->target_port = target_port;
	this->server = server;

	if (Rust::API::NET_StartClient(this->pointer, this->target_ip.c_str(), (uint16_t)target_port, CLIENT_MAX_RETRIES, CLIENT_RETRY_DELAY, CLIENT_TIMEOUT) != 0)
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

void Proxy::Client::Send(unsigned char* data, uint32_t size)
{
	if (!this->incoming_guid) return;
	Rust::API::NETSND_Start(this->pointer);
	Rust::API::NETSND_WriteBytes(this->pointer, data, size);
	Rust::API::NETSND_Send(this->pointer, this->incoming_guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
}

void Proxy::Client::Close()
{
	if (this->pointer && this->is_connected)
	{
		Rust::API::NET_Close(this->pointer);
		this->pointer = 0;
		this->is_connected = false;
		this->incoming_guid = 0;
	}
}