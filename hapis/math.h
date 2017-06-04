#pragma once

#include "rrapi.h"

#define PI 3.1415927f
const float DEG2RAD = PI / 180;

namespace Math {
	void AnglesToAxes(Rust::Vector3 angles, Rust::Vector3 &left, Rust::Vector3 &up, Rust::Vector3 &forward);
	bool WorldToScreen(Rust::Vector3 pos, Rust::Vector3 &screen, float matrix[16], int windowWidth, int windowHeight);
}