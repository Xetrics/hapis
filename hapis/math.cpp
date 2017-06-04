#include "math.h"

void Math::AnglesToAxes(Rust::Vector3 angles, Rust::Vector3 & left, Rust::Vector3 & up, Rust::Vector3 & forward) {
	const float DEG2RAD = 3.141593f / 180;
	float sx, sy, sz, cx, cy, cz, theta;

	// rotation angle about X-axis (pitch)
	theta = angles.x * DEG2RAD;
	sx = sinf(theta);
	cx = cosf(theta);

	// rotation angle about Y-axis (yaw)
	theta = angles.y * DEG2RAD;
	sy = sinf(theta);
	cy = cosf(theta);

	// rotation angle about Z-axis (roll)
	theta = angles.z * DEG2RAD;
	sz = sinf(theta);
	cz = cosf(theta);

	// determine left axis
	left.x = cy*cz;
	left.y = sx*sy*cz + cx*sz;
	left.z = -cx*sy*cz + sx*sz;

	// determine up axis
	up.x = -cy*sz;
	up.y = -sx*sy*sz + cx*cz;
	up.z = cx*sy*sz + sx*cz;

	// determine forward axis
	forward.x = sy;
	forward.y = -sx*cy;
	forward.z = cx*cy;
}

bool Math::WorldToScreen(Rust::Vector3 pos, Rust::Vector3 & screen, float matrix[16], int windowWidth, int windowHeight) {
	/* Matrix-vector Product, multiplying world(eye) coordinates by projection matrix */
	Rust::Vector3 clipCords;
	clipCords.x = pos.x*matrix[0] + pos.y*matrix[4] + pos.z*matrix[8] + matrix[12];
	clipCords.y = pos.x*matrix[1] + pos.y*matrix[5] + pos.z*matrix[9] + matrix[13];
	clipCords.z = pos.x*matrix[2] + pos.y*matrix[6] + pos.z*matrix[10] + matrix[14];
	int w = pos.x*matrix[3] + pos.y*matrix[7] + pos.z*matrix[11] + matrix[15];

	/* perspective division, dividing by w = Normalized Device Coordinates */
	Rust::Vector3 normalized;
	normalized.x = clipCords.x / w;
	normalized.y = clipCords.y / w;
	normalized.z = clipCords.z / w;

	screen.x = (windowWidth / 2 * normalized.x) + (normalized.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * normalized.y) + (normalized.y + windowHeight / 2);

	return w < 0.1f;
}


