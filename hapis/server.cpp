#include "server.h"

Proxy::Server::Server(std::string target_ip, int target_port) {
	this->RakNetServer = RustNetAPI::NET_Create();
	this->alive = false;
	this->target_ip = target_ip;
	this->target_port = target_port;
	this->guid = 0;

	// manual memory managment bad :/
	char* memory_buffer = new char[4194304]; // 4 MB
	this->rcvBuf = reinterpret_cast<unsigned char*>(memory_buffer);
}

void Proxy::Server::listen() {
	RustNetAPI::NET_StartServer(this->RakNetServer, "0.0.0.0", SERVER_PORT, SERVER_MAX_CONNECTIONS);
	printf("Server listening on %d", SERVER_PORT);
	this->alive = true;

	while (this->alive) {
		char packet = RustNetAPI::NET_Receive(this->RakNetServer);

		if (packet) {
			int size = (RustNetAPI::NETRCV_LengthBits(this->RakNetServer) / 8); // turn into define?
			RustNetAPI::NETRCV_ReadBytes(this->RakNetServer, this->rcvBuf, size);
			byte packetId = this->rcvBuf[0];

			// execute hacks based on packet id later

			if (packetId = NEW_INCOMING_CONNECTION) {
				// set up new connection
				uint64_t clientGUID = RustNetAPI::NETRCV_GUID(this->RakNetServer);
				this->GameServerClient = new Proxy::Client(this->target_ip, this->target_port, clientGUID, this);
				this->GameServerClient->listen();

				while (this->guid == 0) Sleep(10);

				int size = (RustNetAPI::NETRCV_LengthBits(this->RakNetServer) / 8); // turn into define?
				RustNetAPI::NETRCV_ReadBytes(this->RakNetServer, this->rcvBuf, size);
				RustNetAPI::NETSND_Start(this->GameServerClient->RakNetClient);
				RustNetAPI::NETSND_WriteBytes(this->GameServerClient->RakNetClient, this->rcvBuf, size);
				RustNetAPI::NETSND_Send(this->GameServerClient->RakNetClient, this->guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
			}
			else {
				// just send along the client packet to the server
				RustNetAPI::NETSND_Start(this->GameServerClient->RakNetClient);
				RustNetAPI::NETSND_WriteBytes(this->GameServerClient->RakNetClient, this->rcvBuf, size);
				RustNetAPI::NETSND_Send(this->GameServerClient->RakNetClient, this->guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
			}
		}
	}
}

void Proxy::Server::SetGUID(uint64_t guid) {
	this->guid = guid;
}