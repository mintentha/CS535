#include "ppc.h"
#include "M33.h"

PPC::PPC(float hfov, int _w, int _h) {

	w = _w;
	h = _h;
	a = V3(1.0f, 0.0f, 0.0f); // horizontal
	b = V3(0.0f, -1.0f, 0.0f); // vertical
	C = V3(0.0f, 0.0f, 0.0f); // camera
	float hfovRad = hfov * 3.14159f / 180.0f;
	c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f * tanf(hfovRad / 2.0f)));
	// depth
}

int PPC::Project(V3 P, V3 &pP) {
	// 0 point behind head
	// 1 point in front of head

	M33 M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);

	M33 Minv = M.Inverted();
	M33 tmp = M * Minv;
	V3 Q = Minv * (P - C);
	if (Q[2] <= 0.0f)
		return 0;

	pP = V3(Q[0] / Q[2], Q[1] / Q[2], 1.0f / Q[2]);
	return 1;

}

void PPC::translate(int dir, float amt) {
	switch (dir) {
		case DIR_UP_DOWN:
			C = C + b * amt;
			break;
		case DIR_LEFT_RIGHT:
			C = C + a * amt;
			break;
		case DIR_FRONT_BACK:
			C = C + (a ^ b) * amt;
			break;
	}
}

void PPC::rotate(int axis, float theta) {
	float rad = theta * M_PI / 180.0f;
	switch (axis) {
		case PAN:
			a = a.rotateVector(b, rad);
			c = c.rotateVector(b, rad);
			break;
		case TILT:
			b = b.rotateVector(a, rad);
			c = c.rotateVector(a, rad);
			break;
		case ROLL:
			V3 aD = (a ^ b).normalized();
			a = a.rotateVector(aD, rad);
			b = b.rotateVector(aD, rad);
			c = c.rotateVector(aD, rad);
			break;
	}
}
