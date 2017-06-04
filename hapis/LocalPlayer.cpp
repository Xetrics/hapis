#include "LocalPlayer.h"

void Rust::LocalPlayer::updatePosition(Rust::Vector3 pos, Rust::Vector3 rot) {
	this->pos = pos;
	this->rot = rot;

	// axes
	Rust::Vector3 ax;
	Rust::Vector3 ay;
	Rust::Vector3 az;
	Math::AnglesToAxes(rot, ax, ay, az);

	float newMatrix[16] = { ax.x, ay.x, az.x, ax.y, ay.y, az.y, ax.z, ay.z, az.z, pos.x, pos.y, pos.z };
	memcpy(this->viewMatrix, newMatrix, sizeof(newMatrix));
}

Rust::LocalPlayer::LocalPlayer(Rust::Vector3 pos, Rust::Vector3 rot, int entityId) {
	this->pos;
	this->rot;
	this->entityId = entityId;
	ZeroMemory(this->viewMatrix, sizeof(this->viewMatrix));
}
