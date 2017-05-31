#include "client.h"

Proxy::Client::Client(std::string target_ip, int target_port, uint64_t guid, Proxy::Server* server) {
	this->RakNetClient = RustNetAPI::NET_Create();
	this->connected = false;
	this->target_ip = target_ip;
	this->target_port = target_port;
	this->guid = guid;
	this->ProxyServer = server;

	// manual memory managment bad :/
	char* memory_buffer = new char[4194304]; // 4 MB
	this->rcvBuf = reinterpret_cast<unsigned char*>(memory_buffer);
}

void Proxy::Client::listen() {
	RustNetAPI::NET_StartClient(this->RakNetClient, this->target_ip.c_str(), (uint16_t)target_port, CLIENT_MAX_RETRIES, CLIENT_RETRY_DELAY, CLIENT_TIMEOUT);
	this->connected = true;
	
	printf("Client connected to game server: %s:%d", this->target_ip.c_str(), this->target_port);

	// we cant do anything until we have the server GUID
	while (this->ProxyServer->guid == 0) {
		while (!RustNetAPI::NET_Receive(this->RakNetClient)) Sleep(10);

		uint64_t serverGUID = RustNetAPI::NETRCV_GUID(this->RakNetClient);
		this->ProxyServer->SetGUID(serverGUID);
	}

	// commence proxying :>
	while (connected) {
		char packet = RustNetAPI::NET_Receive(this->RakNetClient);
		int size = (RustNetAPI::NETRCV_LengthBits(this->RakNetClient) / 8); // turn into define?
		RustNetAPI::NETRCV_ReadBytes(this->RakNetClient, this->rcvBuf, size);
		RustNetAPI::NETSND_Start(this->ProxyServer->RakNetServer);
		RustNetAPI::NETSND_WriteBytes(this->ProxyServer->RakNetServer, this->rcvBuf, size);
		RustNetAPI::NETSND_Send(this->ProxyServer->RakNetServer, this->guid, SERVER_PACKET_PRIORITY, SERVER_PACKET_RELIABILITY, SERVER_PACKET_CHANNEL);
	}
}