#pragma once

#include "rrapi.h"

#define PI 3.1415927f
#define FOV 90 // temp?
const float DEG2RAD = PI / 180;

namespace Math {
	bool World2Screen(Rust::Vector3 camerapos, Rust::Vector3 camerarot, Rust::Vector3 world, Rust::Vector3 & screen, int screenHeight, int screenWidth);
}