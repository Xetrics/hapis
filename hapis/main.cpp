#include <Windows.h>
#include <iostream>
#include <inttypes.h>
#include <string>
#include <stdlib.h>

#include "rrapi.h"
#include "server.h"
#include "overlay.h"

void OnRustPacketReceived(unsigned char* data, uint32_t size)
{

}

void OnRustPacketSent(unsigned char* data, uint32_t size)
{

}

int main(int argc, const char* argv[])
{

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Overlay::Init, NULL, NULL, NULL);
	//Overlay::DrawFilledRectangle(0, 0, 25, 25, 1, 255, 255, 255);
	std::string target_ip;
	int target_port;

	
	if (argc < 3) {
		printf("Server IP: ");
		getline(std::cin, target_ip);
		printf("Server Port: ");
		std::cin >> target_port;
	} else {
		target_ip = argv[1];
		target_port = atoi(argv[2]);
	}

	try
	{
		RustNetAPI::Init();

		Proxy::Server* server = new Proxy::Server(target_ip, target_port);
		server->Start();

		while (server->is_alive) Sleep(1000);
		
		return 0;
	}
	catch (std::exception e)
	{
		printf("ERROR: %s\n", e.what());
		return 1;
	}
}