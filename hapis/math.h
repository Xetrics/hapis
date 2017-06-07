#pragma once

#include "rrapi.h"

#define PI 3.1415927f
#define FOV 90 // temp?
const float DEG2RAD = PI / 180;

namespace Math {
	bool World2Screen(Rust::Vector3 from, Rust::Vector3 rot, float fovDegree, Rust::Vector3 offset, Rust::Vector3 target, Rust::Vector3& vector, float width, float height);
	float Get3dDistance(Rust::Vector3 c1, Rust::Vector3 c2);
}