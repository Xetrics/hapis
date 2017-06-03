#include "server.h"
#include "util.h"
#include "main.h"
#include "message.h"

void ListenThread(Proxy::Server* server)
{
	while (server->is_alive)
	{
		while (Rust::API::NET_Receive(server->pointer))
		{
			uint32_t size = Rust::API::NETRCV_LengthBits(server->pointer) / 8;
			unsigned char* data = (unsigned char*)Rust::API::NETRCV_RawData(server->pointer);

			//printf("[Server] Packet received from client, ID: %d (%s), size: %d\n", data[0], Rust::Message::TypeToName((Rust::MessageType)data[0]), size);

			switch (data[0])
			{
				case NEW_INCOMING_CONNECTION:
				{
					server->incoming_guid = Rust::API::NETRCV_GUID(server->pointer);

					/* connect */
					server->client = new Proxy::Client(server->target_ip, server->target_port, server);

					/* wait for connection success packet */
					while (!Rust::API::NET_Receive(server->client->pointer)) Sleep(10); 

					/* store the server we are connecting to's identifier */
					server->client->incoming_guid = Rust::API::NETRCV_GUID(server->client->pointer);

					/* check if we successfully connected */
					unsigned char client_id = ((unsigned char*)Rust::API::NETRCV_RawData(server->client->pointer))[0];
					if (client_id == CONNECTION_REQUEST_ACCEPTED)
					{
						/* start receiving packets from the server */
						server->client->is_connected = true;
						server->client->Start();

						printf("[Client] Connected to game server: %s:%d\n", server->target_ip.c_str(), server->target_port);

						/* raknet should implicitly send a CONNECTION_REQUEST_ACCEPTED packet to the client, we don't have to do anything ? */
					}
					else
					{
						/* tell the client we couldn't connect to the server */
						printf("[Client] Failed to connect to server, closing connection to client...\n");
						server->Close();
					}

					break;
				}
				case ID_DISCONNECTION_NOTIFICATION: /* client told proxy that they're disconnecting */
					printf("[Server] Disconnection notification received from client, disconnecting...\n");
					server->client->Close(); /* send notification to game server */
					server->Close();
					return;
				case ID_CONNECTION_LOST: /* client lost connection to proxy */
					printf("[Server] Connection to client lost, disconnecting from game server and closing...\n");
					/* stop 'client's receive loop and send ID_CONNECTION_LOST to game server */
					server->client->is_connected = false;
					server->client->Send(data, size);
					server->Close();
					return;
				default:
					OnRustPacketSent(server, data, size);
					server->client->Send(data, size);
			}
		}

		Sleep(PROXY_TICK_MS);
	}
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
		this->thread.terminate();
	}
}
