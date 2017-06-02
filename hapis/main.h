#pragma once

#include <cstdint>

void OnRustPacketReceived(unsigned char* data, uint32_t size);
void OnRustPacketSent(unsigned char* data, uint32_t size);