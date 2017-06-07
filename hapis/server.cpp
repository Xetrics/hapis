#include "server.h"
#include "util.h"
#include "main.h"
#include "message.h"

void ListenThread(Proxy::Server* server)
{
	while (true)
	{
		uint32_t server_result = server->Tick();
		if (server_result & TICK_DISCONNECT)
			break; 

		uint32_t client_result = server->client ? server->client->Tick() : 0;
		if (client_result & TICK_DISCONNECT)
			break;

		if ((server_result & client_result) & TICK_NO_MORE_PACKETS) // neither the server nor client have any packets to process right now
			Sleep(PROXY_TICK_MS);
	}
}

uint32_t Proxy::Server::Tick()
{
	if (!this->is_alive)
		return TICK_DISCONNECT;

	if (!Rust::API::NET_Receive(this->pointer))
		return TICK_NO_MORE_PACKETS;

	uint32_t size = Rust::API::NETRCV_LengthBits(this->pointer) / 8;
	unsigned char* data = (unsigned char*)Rust::API::NETRCV_RawData(this->pointer);

	//printf("[Server] Packet received from client, ID: %d (%s), size: %d\n", data[0], Rust::Message::TypeToName((Rust::MessageType)data[0]), size);

	switch (data[0])
	{
	case NEW_INCOMING_CONNECTION:
	{
		this->incoming_guid = Rust::API::NETRCV_GUID(this->pointer);

		/* connect */
		this->client = new Proxy::Client(this->target_ip, this->target_port, this);

		/* wait for connection success packet */
		while (!Rust::API::NET_Receive(this->client->pointer)) Sleep(10);

		/* store the server we are connecting to's identifier */
		this->client->incoming_guid = Rust::API::NETRCV_GUID(this->client->pointer);

		/* check if we successfully connected */
		unsigned char client_id = ((unsigned char*)Rust::API::NETRCV_RawData(this->client->pointer))[0];
		if (client_id == CONNECTION_REQUEST_ACCEPTED)
		{
			/* start receiving packets from the server */
			this->client->is_connected = true;

			printf("[Client] Connected to game server: %s:%d\n", this->target_ip.c_str(), this->target_port);

			/* raknet should implicitly send a CONNECTION_REQUEST_ACCEPTED packet to the client, we don't have to do anything ? */
		}
		else
		{
			/* tell the client we couldn't connect to the server */
			printf("[Client] Failed to connect to server, closing connection to client...\n");
			this->Close();
		}

		break;
	}
	case ID_DISCONNECTION_NOTIFICATION: /* client told proxy that they're disconnecting */
		printf("[Server] Disconnection notification received from client, disconnecting...\n");
		this->client->Close(); /* send notification to game server */
		this->Close();
		return TICK_DISCONNECT;
	case ID_CONNECTION_LOST: /* client lost connection to proxy */
		printf("[Server] Connection to client lost, disconnecting from game server and closing...\n");
		/* stop 'client's receive loop and send ID_CONNECTION_LOST to game server */
		this->client->is_connected = false;
		this->client->Send(data, size);
		this->Close();
		return TICK_DISCONNECT;
	default:
		OnRustPacketSent(this, data, size);
		this->client->Send(data, size);
	}

	return 0;
}

Proxy::Server::Server(std::string target_ip, int target_port)
{
	this->pointer = Rust::API::NET_Create();
	this->is_alive = false;
	this->target_ip = target_ip;
	this->target_port = target_port;
	this->incoming_guid = 0;

	if (Rust::API::NET_StartServer(this->pointer, "127.0.0.1", SERVER_PORT, SERVER_MAX_CONNECTIONS) != 0)
	{
		printf("[Server] ERROR: Unable to start server on port %d\n", SERVER_PORT);
		return;
	}

	printf("[Server] Listening on port %d\n", SERVER_PORT);

	this->is_alive = true;
}

Proxy::Server::~Server()
{
	Close();
}

void Proxy::Server::Start()
{
	thread = util::athread(ListenThread, this);
}

void Proxy::Server::Send(unsigned char* data, uint32_t size)
{
	if (!this->incoming_guid) return;
	Rust::API::NETSND_Start(this->pointer);
	Rust::API::NETSND_WriteBytes(this->pointer, data, size);
	Rust::API::NETSND_Send(this->pointer, this->incoming_guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
}

void Proxy::Server::Close()
{
	if (this->pointer && this->is_alive)
	{
		Rust::API::NET_Close(this->pointer);
		this->pointer = 0;
		this->is_alive = false;
		this->incoming_guid = 0;
	}
}
