#include "server.h"
#include "util.h"

void ListenThread(Proxy::Server* server)
{
	while (server->alive)
	{
		while (RustNetAPI::NET_Receive(server->RakNetServer))
		{
			uint32_t size = RustNetAPI::NETRCV_LengthBits(server->RakNetServer) / 8;
			unsigned char* data = (unsigned char*)RustNetAPI::NETRCV_RawData(server->RakNetServer);

			printf("[Server] Packet received from client, ID: %d, size: %d\n", data[0], size);

			if (data[0] == NEW_INCOMING_CONNECTION)
			{
				server->incoming_guid = RustNetAPI::NETRCV_GUID(server->RakNetServer);

				/* connect */
				server->GameServerClient = new Proxy::Client(server->target_ip, server->target_port, server);

				/* wait for connection success packet */
				while (!RustNetAPI::NET_Receive(server->GameServerClient->RakNetClient)) Sleep(10); /* wait for connection success packet */

				/* store the server we are connecting to's identifier */
				server->GameServerClient->incoming_guid = RustNetAPI::NETRCV_GUID(server->GameServerClient->RakNetClient);

				/* check if we successfully connected */
				unsigned char client_id = ((unsigned char*)RustNetAPI::NETRCV_RawData(server->GameServerClient->RakNetClient))[0];
				if (client_id == CONNECTION_REQUEST_ACCEPTED)
				{
					/* start receiving packets from the server */
					server->GameServerClient->connected = true;
					server->GameServerClient->Start();

					printf("[Client] Connected to game server: %s:%d\n", server->target_ip.c_str(), server->target_port);

					/* raknet should implicitly send a CONNECTION_REQUEST_ACCEPTED packet to the client, we don't have to do anything ? */
				}
				else
				{
					/* tell the client we couldn't connect to the server */
					printf("[Client] Failed to connect to server, closing connection to client...\n");
					server->Close();
				}
			}
			else
			{
				/* forward packet to server */
				server->GameServerClient->Send(data, size);
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

void Proxy::Server::Send(unsigned char* data, uint32_t size)
{
	if (!this->incoming_guid) return;
	RustNetAPI::NETSND_Start(this->RakNetServer);
	RustNetAPI::NETSND_WriteBytes(this->RakNetServer, data, size);
	RustNetAPI::NETSND_Send(this->RakNetServer, this->incoming_guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
}

void Proxy::Server::Close()
{
	RustNetAPI::NET_Close(this->RakNetServer);
	this->RakNetServer = 0;
	this->alive = false;
	this->incoming_guid = 0;
}
