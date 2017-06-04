#include <Windows.h>
#include <iostream>
#include <inttypes.h>
#include <string>
#include <stdlib.h>

#include "rrapi.h"
#include "server.h"
#include "overlay.h"
#include "message.h"
#include "protos\main.pb.h"

void OnRustPacketReceived(Proxy::Client* client, unsigned char* data, uint32_t size)
{
	if (data[0] == Rust::MessageType::EntityPosition)
	{
		//Rust::EntityPositionMessage* message = (Rust::EntityPositionMessage*)Rust::Message::MessageFromPacket(client->pointer, data, size);

		/*Rust::EntityPositionMessage message;
		message.Deserialize(client->pointer);

		printf("EntityPosition packet received!\n\t- Entity ID: %d\n\t- Position: %f, %f, %f\n\t- Rotation: %f, %f, %f\n",
			message.entity_id,
			message.position.x, message.position.y, message.position.z,
			message.rotation.x, message.rotation.y, message.rotation.z);*/

		//*(float*)(data + 5) = -25.8;
		//*(float*)(data + 9) = 16;
		//*(float*)(data + 13) = -155.4;
	}
	else if (data[0] == Rust::MessageType::ConsoleCommand)
	{
		Rust::ConsoleCommandMessage message;
		message.Deserialize(client->pointer);

		printf("Console command received from server, command: %s\n", message.command.c_str());
	}
	else if (data[0] == Rust::MessageType::Entities)
	{
		printf("Entities packet, num: %d, ", *(int*)(data + 1));

		std::string strdata;
		strdata.append((const char*)(data + 5), size - 5);

		Entity entity;
		entity.ParseFromString(strdata);

		printf("group: %d, uid: %d, name: %s\n",
			entity.has_basenetworkable() ? entity.basenetworkable().group() : -1,
			entity.has_basenetworkable() ? entity.basenetworkable().uid() : -1,
			entity.has_baseplayer() ? entity.baseplayer().name().c_str() : "(not a player)");
	}
	else if (data[0] == Rust::MessageType::Approved)
	{
		std::string strdata;
		strdata.append((const char*)(data + 1), size - 1);

		Approval approval;
		
		try {
			approval.ParseFromString(strdata);
			printf("Approval packet received!\n"
				"\t- level: %s\n"
				"\t- seed: %d\n"
				"\t- size: %d\n",
				approval.level().c_str(), approval.levelseed(), approval.levelsize());
		}
		catch (google::protobuf::FatalException e)
		{
			printf("Fatal exception occured: %s\n", e.what());
		}
		catch (std::exception e)
		{
			printf("Exception occured: %s\n", e.what());
		}
		catch (...)
		{
			printf("Crash occured\n");
		}
	}
}

void OnRustPacketSent(Proxy::Server* server, unsigned char* data, uint32_t size)
{

}

int main(int argc, const char* argv[])
{
	// nathan fix ur overlay it uses like 50000000000% cpu
	// CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Overlay::Init, NULL, NULL, NULL);
	// Overlay::DrawFilledRectangle(0, 0, 25, 25, 1, 255, 255, 255);

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
		Rust::API::Init();

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