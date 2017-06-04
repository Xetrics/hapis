#include <Windows.h>
#include <iostream>
#include <inttypes.h>
#include <string>
#include <stdlib.h>

#include "rrapi.h"
#include "server.h"
#include "overlay.h"
#include "message.h"
#include "main.h"
#include "protos\main.pb.h"

std::unordered_map<int, Rust::Vector3> players;
Rust::LocalPlayer* localPlayer;

void OnRustPacketReceived(Proxy::Client* client, unsigned char* data, uint32_t size)
{
	if (data[0] == Rust::MessageType::EntityPosition)
	{
		//Rust::EntityPositionMessage* message = (Rust::EntityPositionMessage*)Rust::Message::MessageFromPacket(client->pointer, data, size);

		Rust::EntityPositionMessage message;
		message.Deserialize(client->pointer);

		/*printf("EntityPosition packet received!\n\t- Entity ID: %d\n\t- Position: %f, %f, %f\n\t- Rotation: %f, %f, %f\n",
			message.entity_id,
			message.position.x, message.position.y, message.position.z,
			message.rotation.x, message.rotation.y, message.rotation.z);*/

		//*(float*)(data + 5) = -25.8;
		//*(float*)(data + 9) = 16;
		//*(float*)(data + 13) = -155.4;

		auto entity = players.find(message.entity_id);
		if (entity != players.end()) {
			players[message.entity_id] = Rust::Vector3{ message.position.x, message.position.y, message.position.z };
		
			if (localPlayer->entityId = message.entity_id)
				localPlayer->updatePosition(message.position, message.rotation);
		}
	}
	else if (data[0] == Rust::MessageType::ConsoleCommand)
	{
		Rust::ConsoleCommandMessage message;
		message.Deserialize(client->pointer);

		printf("Console command received from server, command: %s\n", message.command.c_str());
	}
	else if (data[0] == Rust::MessageType::Entities)
	{
		//printf("Entities packet, num: %d, ", *(int*)(data + 1));

		std::string strdata;
		strdata.append((const char*)(data + 5), size - 5);

		Entity entity;
		entity.ParseFromString(strdata);

		if (entity.has_baseplayer() && entity.has_basenetworkable()) {
			//printf("Got entity packet for player %s, entity id: %d\n", entity.baseplayer().name().c_str(), entity.basenetworkable().uid());
			players[entity.basenetworkable().uid()] = Rust::Vector3{ 0, 0, 0 };

			if (entity.baseplayer().has_metabolism)
				localPlayer = new Rust::LocalPlayer(Rust::Vector3{ 0,0,0 }, Rust::Vector3{ 0,0,0 }, entity.basenetworkable().uid());
		}
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
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Overlay::Init, NULL, NULL, NULL);

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