#pragma once

#include "V3.h"

class AABB {
public:
	V3 corners[2];
	V3 &cornerLow = corners[0];
	V3 &cornerHigh = corners[1];
	AABB(V3 cornerLow, V3 cornerHigh);
	AABB(V3 *verts, int vertsN);
	AABB(V3 firstPoint);
	void AddPoint(V3 newPoint);
	float GetDX();
	float GetDY();
	float GetDZ();
	float GetDiagonalLength();
	V3 GetCenter();
	int ClipWithImageFrame(int w, int h);
};