#pragma once
#include "V3.h"
#define _USE_MATH_DEFINES
#include <math.h>

class PPC {
public:
	static const int DIR_UP_DOWN = 0;
	static const int DIR_LEFT_RIGHT = 1;
	static const int DIR_FRONT_BACK = 2;
	static const int PAN = 0;
	static const int TILT = 1;
	static const int ROLL = 2;
	V3 a, b, c, C;
	int w, h;
	PPC(float hfov, int _w, int _h);
	int Project(V3 P, V3& pP);
	void translate(int dir, float amt);
	void rotate(int axis, float theta);
};