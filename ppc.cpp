#include "ppc.h"
#include "M33.h"
#include "framebuffer.h"

PPC::PPC(float hfov, int _w, int _h) {

	w = _w;
	h = _h;
	a = V3(1.0f, 0.0f, 0.0f); // horizontal
	b = V3(0.0f, -1.0f, 0.0f); // vertical
	C = V3(0.0f, 0.0f, 0.0f); // camera
	float hfovRad = hfov * M_PI / 180.0f;
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
	V3 Q = Minv * (P - C);
	if (Q[2] <= 0.0f)
		return 0;

	pP = V3(Q[0] / Q[2], Q[1] / Q[2], 1.0f / Q[2]);
	return 1;

}

int PPC::UnProject(V3 pP, V3& P) {

	if (pP[2] == 0.0f) {
		return 0;
	}

	P = C + (a * pP[0] + b * pP[1] + c) / pP[2];

	return 1;

}

void PPC::translate(int dir, float amt) {
	switch (dir) {
		case DIR_UP_DOWN:
			C = C - b * amt;
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
	V3 aD(0.0f, 0.0f, 0.0f);
	switch (axis) {
		case PAN:
			aD = b;
			a = a.rotateVector(aD, -theta);
			c = c.rotateVector(aD, -theta);
			break;
		case TILT:
			aD = a;
			b = b.rotateVector(aD, theta);
			c = c.rotateVector(aD, theta);
			break;
		case ROLL:
			aD = a ^ b;
			a = a.rotateVector(aD, -theta);
			b = b.rotateVector(aD, -theta);
			c = c.rotateVector(aD, -theta);
			break;
	}
}

void PPC::revolve(int axis, float theta, V3 center) {
	V3 aD(0.0f, 0.0f, 0.0f);
	switch (axis) {
		case PAN:
			aD = (b * -1.0f);
			break;
		case TILT:
			aD = (a * -1.0f);
			break;
		case ROLL:
			aD = a ^ b;
			break;
	}
	C = C.rotatePoint(center, aD, theta);
	a = a.rotateVector(aD, theta);
	b = b.rotateVector(aD, theta);
	c = c.rotateVector(aD, theta);
}


float PPC::GetF() {

	V3 vd = (a ^ b).normalized();
	return c * vd;

}

void PPC::SetF(float f) {
	// want f = c * (a^b)
	c = c * f / GetF();
}

void PPC::SetPose(V3 newC, V3 LaP, V3 upgv) {

	V3 newa, newb, newc;

	V3 newvd = (LaP - newC).normalized();
	newa = (newvd ^ upgv).normalized();
	newb = (newvd ^ newa).normalized();
	float f = GetF();
	newc = newvd * f - newa * (float)(w / 2) - newb * (float)h / 2;

	a = newa;
	b = newb;
	c = newc;
	C = newC;

}

V3 PPC::GetVD() {

	return (a ^ b).normalized();

}

void PPC::SetInterpolated(PPC* ppc0, PPC* ppc1, int fi, int fN) {

	*this = *ppc0;

	float ff = (float)fi / (float)(fN - 1);
	V3 newC = ppc0->C + (ppc1->C - ppc0->C) * ff;
	V3 newvd = ppc0->GetVD() + (ppc1->GetVD() - ppc0->GetVD()) * ff;
	V3 LaP = newC + newvd * 100.0f;
	V3 upgv = (ppc0->b + (ppc1->b - ppc0->b) * ff) * -1.0f;

	SetPose(newC, LaP, upgv);

}

void PPC::Visualize(float visf, PPC* ppc, FrameBuffer* fb) {

	fb->Draw3DPoint(C, V3(0.0f, 0.0f, 0.0f), 7, ppc);

	float scf = visf / GetF();
	V3 visa = a * scf;
	V3 visb = b * scf;
	V3 visc = c * scf;

	fb->Draw3DPoint(C + visc, V3(1.0f, 0.0f, 0.0f), 5, ppc);
	fb->Draw3DPoint(C + visc + visa * (float)w, V3(0.0f, 1.0f, 0.0f), 5, ppc);
	fb->Draw3DPoint(C + visc + visb * (float)h, V3(0.0f, 0.0f, 1.0f), 5, ppc);

	fb->Draw3DSegment(C, V3(0.0f, 0.0f, 0.0f), C + visc, V3(0.0f, 0.0f, 0.0f), ppc);

	fb->Draw3DSegment(C + visc, V3(0.0f, 0.0f, 0.0f), C + visc + visa * (float)w, V3(0.0f, 0.0f, 0.0f), ppc);
	fb->Draw3DSegment(C + visc + visa * (float)w, V3(0.0f, 0.0f, 0.0f),
		C + visc + visa * (float)w + visb * (float)h, V3(0.0f, 0.0f, 0.0f),
		ppc);
	fb->Draw3DSegment(C + visc + visa * (float)w + visb * (float)h, V3(0.0f, 0.0f, 0.0f),
		C + visc + visb * (float)h, V3(0.0f, 0.0f, 0.0f),
		ppc);
	fb->Draw3DSegment(C + visc + visb * (float)h, V3(0.0f, 0.0f, 0.0f),
		C + visc, V3(0.0f, 0.0f, 0.0f),
		ppc);

}

V3 PPC::GetPixelCenter(int u, int v) {

	return C + a * (.5f + (float)u) + b * (.5f + (float)v) + c;

}

V3 PPC::GetRay(int u, int v) {

	return (GetPixelCenter(u, v) - C).normalized();

}

void PPC::SetIntrinsicsHW() {

	glViewport(0, 0, w, h);
	float zNear = 1.0f;
	float zFar = 10000.0f;
	float scf = zNear / GetF();
	float left = -(float)w / 2.0f * scf;
	float right = +(float)w / 2.0f * scf;
	float bottom = -(float)h / 2.0f * scf;
	float top = +(float)h / 2.0f * scf;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);

}

void PPC::SetExtrinsicsHW() {

	V3 lap = C + GetVD();
	glLoadIdentity();
	gluLookAt(C[0], C[1], C[2], lap[0], lap[1], lap[2], -b[0], -b[1], -b[2]);

}