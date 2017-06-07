#pragma once

#include <Windows.h>

#include "rrapi.h"
#include "math.h"

namespace Rust {
	class LocalPlayer {
	public:
		Rust::Vector3 pos;
		Rust::Vector3 rot;
		int entityId;

		void updatePosition(Rust::Vector3 pos, Rust::Vector3 rot);
		LocalPlayer(Rust::Vector3 pos, Rust::Vector3 rot, int entityId);
	};
}

extern Rust::LocalPlayer* localPlayer;