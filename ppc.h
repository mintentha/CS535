#pragma once

#include "V3.h"
#define _USE_MATH_DEFINES
#include <math.h>

class FrameBuffer;
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
	int UnProject(V3 pP, V3& P);
	void translate(int dir, float amt);
	void rotate(int axis, float theta);
	void revolve(int axis, float theta, V3 center);
	void SetPose(V3 newC, V3 LaP, V3 upgv);
	float GetF();
	void SetF(float f);
	void SetInterpolated(PPC* ppc1, PPC* ppc2, int fi, int fN);
	V3 GetVD();
	void Visualize(float visf, PPC* ppc, FrameBuffer* fb);
	V3 GetPixelCenter(int u, int v);
	V3 GetRay(int u, int v);
};