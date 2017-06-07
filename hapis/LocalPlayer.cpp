#include "LocalPlayer.h"

void Rust::LocalPlayer::updatePosition(Rust::Vector3 pos, Rust::Vector3 rot) {
	this->pos = pos;
	this->rot = rot;
}

Rust::LocalPlayer::LocalPlayer(Rust::Vector3 pos, Rust::Vector3 rot, int entityId) {
	this->pos = pos;
	this->rot = rot;
	this->entityId = entityId;
	this->updatePosition(pos, rot);
}