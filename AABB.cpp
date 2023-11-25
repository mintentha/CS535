#include "AABB.h"
#include <iostream>

AABB::AABB(V3 cornerLow, V3 cornerHigh) {
	this->cornerLow = cornerLow;
	this->cornerHigh = cornerHigh;
}

AABB::AABB(V3* verts, int vertsN) {
	if (vertsN <= 0) {
		cornerLow = cornerHigh = V3(0.0f, 0.0f, 0.0f);
		return;
	}
	cornerLow = cornerHigh = verts[0];
	for (int i = 1; i < vertsN; i++) {
		for (int j = 0; j < 3; j++) {
			if (verts[i][j] < cornerLow[j]) {
				cornerLow[j] = verts[i][j];
			}
			if (verts[i][j] > cornerHigh[j]) {
				cornerHigh[j] = verts[i][j];
			}
		}
	}
}

float AABB::GetDX() {
	return cornerHigh[0] - cornerLow[0];
}

float AABB::GetDY() {
	return cornerHigh[1] - cornerLow[1];
}

float AABB::GetDZ() {
	return cornerHigh[2] - cornerLow[2];
}

float AABB::GetDiagonalLength() {
	return (cornerHigh - cornerLow).length();
}

V3 AABB::GetCenter() {
	return cornerHigh - cornerLow;
}