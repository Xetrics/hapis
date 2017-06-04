#pragma once

#include <cstdint>
#include <unordered_map>

#include "client.h"
#include "server.h"

// entity_id, position
extern std::unordered_map<int, Rust::Vector3> players;
extern Rust::LocalPlayer* localPlayer;

void OnRustPacketReceived(Proxy::Client* client, unsigned char* data, uint32_t size);
void OnRustPacketSent(Proxy::Server* server, unsigned char* data, uint32_t size);