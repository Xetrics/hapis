#include "math.h"

// this function could/should probably be reduced to allocate less vectors
/*bool Math::World2Screen(Rust::Vector3 camerapos, Rust::Vector3 camerarot, Rust::Vector3 world, Rust::Vector3 &screen, int screenHeight, int screenWidth)
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
}*/

float DegToRad(float deg)
{
	return (float)((double)deg * 0.017453292519943295);
}

bool Math::World2Screen(Rust::Vector3 from, Rust::Vector3 rot, float fovDegree, Rust::Vector3 offset, Rust::Vector3 target, Rust::Vector3& vector, float width, float height) {
	if (rot.x < -180.0f) rot.x += 360.0f;
	if (rot.x > 180.0f)  rot.x -= 360.0f;

	Rust::Vector3 newRot = { DegToRad(rot.x), DegToRad(rot.y), DegToRad(rot.z) };
	float cosX = cos(newRot.x), cosY = cos(newRot.y), cosZ = cos(newRot.z),
		sinX = sin(newRot.x), sinY = sin(newRot.y), sinZ = sin(newRot.z);

	float array[3][3] = {
		{ cosZ * cosY - sinZ * sinX * sinY, -cosX * sinZ, cosZ * sinY + cosY * sinZ * sinX },
		{ cosY * sinZ + cosZ * sinX * sinY,  cosZ * cosX, sinZ * sinY - cosZ * cosY * sinX },
		{ -cosX * sinY, sinX, cosX * cosY }
	};

	Rust::Vector3 vector3 = { target.x - (from.x + offset.x), target.y - (from.y + offset.y), -target.z - (-from.z + offset.z) };
	vector3 = { 
		array[0][0] * vector3.x + array[0][1] * vector3.y + array[0][2] * vector3.z,
		array[1][0] * vector3.x + array[1][1] * vector3.y + array[1][2] * vector3.z,
		-(array[2][0] * vector3.x + array[2][1] * vector3.y + array[2][2] * vector3.z)
	};

	bool result = false;
	vector.x = 0;
	vector.y = 0;

	if (vector3.z > 0.0f) {
		float focalLength = (float)((double)(height / 2.0f) / tan((double)(DegToRad(fovDegree) / 2.0f)));
		vector.x = focalLength * vector3.x / vector3.z + width / 2.0f;
		vector.y = focalLength * -vector3.y / vector3.z + height / 2.0f;
		result = true;
	}

	return result;
}

float Math::Get3dDistance(Rust::Vector3 c1, Rust::Vector3 c2)
{
	//(Dx*Dx+Dy*Dy+Dz*Dz)^.5 
	float dx = c2.x - c1.x;
	float dy = c2.y - c1.y;
	float dz = c2.z - c1.z;

	return sqrt((float)(dx * dx + dy * dy + dz * dz));
}
