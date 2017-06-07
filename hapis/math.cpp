#include "math.h"

// this function could/should probably be reduced to allocate less vectors
bool Math::World2Screen(Rust::Vector3 camerapos, Rust::Vector3 camerarot, Rust::Vector3 world, Rust::Vector3 &screen, int screenHeight, int screenWidth)
{
	// Translate by inverse camera pos
	Rust::Vector3 point;
	Rust::Vector3 icamerapos = Rust::Vector3{ camerapos.x, camerapos.y, -camerapos.z };
	Rust::Vector3 iworld = Rust::Vector3{ world.x, world.y, -world.z };
	iworld = iworld - icamerapos;

	// Construct rotation matrix
	float cosX = cos(camerarot.x);
	float cosY = cos(camerarot.y);
	float cosZ = cos(camerarot.z);
	float sinX = sin(camerarot.x);
	float sinY = sin(camerarot.y);
	float sinZ = sin(camerarot.z);

	float matrix[3][3] = {
		{ cosZ * cosY - sinZ * sinX * sinY, -cosX * sinZ, cosZ * sinY + cosY * sinZ * sinX },
		{ cosY * sinZ + cosZ * sinX * sinY, cosZ * cosX, sinZ * sinY - cosZ * cosY * sinX },
		{ -cosX * sinY, sinX, cosX * cosY }
	};

	// apply rotation to point
	Rust::Vector3 rotatedPoint{
		matrix[0][0] * iworld.x + matrix[0][1] * iworld.y + matrix[0][2] * iworld.z,
		matrix[1][0] * iworld.x + matrix[1][1] * iworld.y + matrix[1][2] * iworld.z,
		matrix[2][0] * iworld.x + matrix[2][1] * iworld.y + matrix[2][2] * iworld.z
	};

	// revert inverse operation
	point = Rust::Vector3{ rotatedPoint.x, rotatedPoint.y, -rotatedPoint.z };


	// calculate screen point
	float focalLength = (screenHeight / 2) / tan(FOV / 2);
	screen.x = focalLength * point.x / point.z + screenWidth / 2;
	screen.y = focalLength * point.y / point.z + screenHeight / 2;

	return rotatedPoint.z > 0;
}
