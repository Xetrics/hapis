#include <Windows.h>
#include <iostream>
#include <inttypes.h>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <chrono>


#include "rrapi.h"
#include "stringpool.h"
#include "server.h"
#include "settings.h"
#include "overlay.h"
#include "message.h"
#include "main.h"
#include "protos\main.pb.h"

#define if_setting(set, code) { if(settings->set){ code; } }

std::unordered_map<int, Rust::Vector3> players;
Rust::LocalPlayer* localPlayer;
Settings* settings = new Settings();

int randomgen(int max, int min) // totally not skidded
{
	srand(time(NULL));
	int random = rand() % max + min;
	return random;
}

google::protobuf::int64 staticTime = 0;

void OnRustPacketReceived(Proxy::Client* client, unsigned char* data, uint32_t size)
{
	if (data[0] == Rust::MessageType::EntityPosition)
	{
		Rust::EntityPositionMessage message;
		message.Deserialize(client->pointer);

		if (players.find(message.entity_id) != players.end()) {
			players[message.entity_id] = Rust::Vector3{ message.position.x, message.position.y, message.position.z };
		}
		else if (message.entity_id == localPlayer->entityId) {
			localPlayer->updatePosition(message.position, message.rotation);
		}

	}
	else if (data[0] == Rust::MessageType::Entities)
	{
		std::string strdata;
		strdata.append((const char*)(data + 5), size - 5);

		Entity entity;
		entity.ParseFromString(strdata);


		if (entity.has_baseplayer() && entity.has_basenetworkable()) {
			if (entity.baseplayer().has_metabolism()) {
				localPlayer = new Rust::LocalPlayer(Rust::Vector3{ 0, 0, 0 }, Rust::Vector3{ 0, 0, 0 }, entity.basenetworkable().uid());
			}
			else {
				players[entity.basenetworkable().uid()] = Rust::Vector3{ 0, 0, 0 };
			}
		} else if (entity.has_environment()) {
			Environment* env = new Environment(entity.environment());
			//if (staticTime == 0)
			//	staticTime = env->datetime();
			//env->set_datetime(staticTime);

			if_setting(weather->always_day, env->set_datetime(24000));
			if_setting(weather->no_fog, env->set_fog(-1));
			if_setting(weather->no_rain, env->set_rain(-1));
			if_setting(weather->no_clouds, env->set_clouds(-1));
			if_setting(weather->no_wind, env->set_wind(-1));

			entity.release_environment();
			entity.set_allocated_environment(env);
			std::string newEntityData = entity.SerializeAsString();
			memcpy(data + 5, &newEntityData[0], newEntityData.size());
		}
	}
	else if (data[0] == Rust::MessageType::EntityDestroy) {
		Rust::EntityDestroyMessage message;
		message.Deserialize(client->pointer);


		if (players.find(message.entity_id) != players.end()) {
			players.erase(message.entity_id);
		}
	}
	else if (data[0] == Rust::MessageType::Approved)
	{
		std::string strdata;
		strdata.append((const char*)(data + 1), size - 1);

		Approval approval;

		approval.ParseFromString(strdata);
		printf("Approval packet received!\n"
			"\t- level: %s\n"
			"\t- seed: %d\n"
			"\t- size: %d\n",
			approval.level().c_str(), approval.levelseed(), approval.levelsize());
	}
	else if (data[0] == Rust::MessageType::ConsoleCommand)
	{
		Rust::ConsoleCommandMessage message;
		message.Deserialize(client->pointer);

		printf("ConsoleCommand: %s\n", message.command.c_str());
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
	}
	else {
		target_ip = argv[1];
		target_port = atoi(argv[2]);
	}

	try
	{
		Rust::API::Init();
		Rust::StringPool::Initialize();

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