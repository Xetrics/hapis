#pragma once

#include <cstdint>
#include "client.h"
#include "server.h"

void OnRustPacketReceived(Proxy::Client* client, unsigned char* data, uint32_t size);
void OnRustPacketSent(Proxy::Server* server, unsigned char* data, uint32_t size);