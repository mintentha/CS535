#include "AABB.h"
#include <iostream>

AABB::AABB(V3 cornerLow, V3 cornerHigh) {
	this->cornerLow = cornerLow;
	this->cornerHigh = cornerHigh;
}

AABB::AABB(V3 firstPoint) {
	cornerLow = firstPoint;
	cornerHigh = firstPoint;
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

void AABB::AddPoint(V3 newPoint) {

	for (int i = 0; i < 3; i++) {
		if (newPoint[i] < corners[0][i])
			corners[0][i] = newPoint[i];
		if (newPoint[i] > corners[1][i])
			corners[1][i] = newPoint[i];
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

int AABB::ClipWithImageFrame(int w, int h) {

	if (corners[0][0] >= w)
		return 0;
	if (corners[1][0] < 0)
		return 0;
	if (corners[0][1] >= h)
		return 0;
	if (corners[1][1] < 0)
		return 0;

	if (corners[0][0] < 0)
		corners[0][0] = 0;
	if (corners[0][1] < 0)
		corners[0][1] = 0;

	if (corners[1][0] >= w)
		corners[1][0] = (float)w - 0.001f;
	if (corners[1][1] >= h)
		corners[1][1] = (float)h - 0.001f;


	return 1;

}