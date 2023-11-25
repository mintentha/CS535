#pragma once

#include "V3.h"

class AABB {
public:
	V3 cornerLow;
	V3 cornerHigh;
	AABB(V3 cornerLow, V3 cornerHigh);
	AABB(V3 *verts, int vertsN);
	float GetDX();
	float GetDY();
	float GetDZ();
	float GetDiagonalLength();
	V3 GetCenter();
};