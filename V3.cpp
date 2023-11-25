
#include "V3.h"
#include "M33.h"

using namespace std;

#include <iostream>

V3::V3(float x, float y, float z) {

	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;

}

V3::V3(unsigned int col) {
	this->SetColor(col);
}

float& V3::operator[](int i) {

	return xyz[i];

}

V3 V3::operator+(V3 op1) {

	V3 ret;
	V3& op0 = (*this);
	ret[0] = op0[0] + op1[0];
	ret[1] = op0[1] + op1[1];
	ret[2] = op0[2] + op1[2];
	return ret;

}

V3 V3::operator-(V3 op1) {

	V3 ret;
	V3& op0 = (*this);
	ret[0] = op0[0] - op1[0];
	ret[1] = op0[1] - op1[1];
	ret[2] = op0[2] - op1[2];
	return ret;

}

float V3::operator*(V3 op1) {

	V3& op0 = (*this);
	float ret;
	ret = op0[0] * op1[0] + op0[1] * op1[1] + op0[2] * op1[2];
	return ret;

}

V3 V3::operator/(float scf) {

	V3& v = *this;
	return v * (1.0f / scf);

}

V3 V3::operator*(float scf) {

	V3 ret;
	V3& v = *this;
	ret[0] = v[0] * scf;
	ret[1] = v[1] * scf;
	ret[2] = v[2] * scf;
	return ret;

}

V3 operator*(double scf, V3 v) {
	return v * scf;
}

V3 V3::operator^(V3 v2) {
	// y1z2 - z1y2
	// z1x2 - x1z2
	// x1y2 - y1x2

	V3& v1 = *this;
	V3 ret;
	ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
	ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
	ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return ret;

}

ostream& operator<<(ostream &ostr, V3 v) {

	return ostr << v[0] << " " << v[1] << " " << v[2];

}

// idk about this im just guessing
istream& operator>>(istream& istr, V3 v) {
	return istr >> v[0] >> v[1] >> v[2];
}

V3 V3::normalized() {
	float length = this->length();
	float x = (*this)[0];
	return V3((*this)[0] / length, (*this)[1] / length, (*this)[2] / length);
}

float V3::length() {
	float x = xyz[0];
	float y = xyz[1];
	float z = xyz[2];
	return sqrt(x * x + y * y + z * z);
}

unsigned int V3::GetColor() {

	unsigned int ret;
	unsigned char* rgba = (unsigned char*)&ret;
	V3& v = *this;
	for (int ci = 0; ci < 3; ci++) {
		if (v[ci] < 0.0f)
			rgba[ci] = 0;
		else if (v[ci] >= 1.0f)
			rgba[ci] = 255;
		else
			rgba[ci] = (unsigned char)(v[ci] * 255.0f);
	}
	rgba[3] = 0xFF;
	return ret;

}

void V3::SetColor(unsigned int color) {

	unsigned char* rgba = (unsigned char*)&color;
	V3& v = *this;
	v[0] = (float)rgba[0] / 255.0f;
	v[1] = (float)rgba[1] / 255.0f;
	v[2] = (float)rgba[2] / 255.0f;

}

V3 V3::rotateVector(V3 a, float theta) {
	// Rather than rotating around some arbitrary origin we just rotate in a particular direction
	// So origin stays the same
	return rotatePoint(V3(0.0f, 0.0f, 0.0f), a, theta);
}

V3 V3::rotatePoint(V3 aO, V3 aD, float theta) {
	V3 aux;
	if (fabsf(aD[0]) < fabsf(aD[1])) {
		aux = V3(1.0f, 0.0f, 0.0f);
	}
	else {
		aux = V3(0.0f, 1.0f, 0.0f);
	}

	V3 xa = aD ^ aux;
	V3 ya = aD;
	V3 za = xa ^ ya;

	M33 lcs; lcs[0] = xa; lcs[1] = ya; lcs[2] = za;

	V3& P = *this;
	V3 lP = lcs * (P - aO);
	M33 rotY = M33::RotationMatrix(M33::Y_AXIS, theta * M_PI / 180.0f);
	V3 rlP = rotY * lP;
	V3 ret = lcs.Inverted() * rlP + aO;
	return ret;
}

// n and l have to be unit vectors
V3 V3::Light(V3 n, V3 l, float ka) {

	V3 ret;
	V3& v = *this;

	float kd = l * n;
	kd = (kd > 0.0f) ? kd : 0.0f;
	ret = v * (ka + (1.0f - ka) * kd);

	return ret;

}

V3 V3::Light(V3 P, V3 n, V3 L, float ka) {

	return Light(n, (L - P).normalized(), ka);

}

V3 V3::EdgeEquation(V3 p0, V3 p1, V3 p2) {

	float u0 = p0[0];
	float u1 = p1[0];
	float u2 = p2[0];

	float v0 = p0[1];
	float v1 = p1[1];
	float v2 = p2[1];

	V3 ret;
	ret[0] = v1 - v0;
	ret[1] = u0 - u1;
	ret[2] = v0 * (u1 - u0) - u0 * (v1 - v0);

	V3 p2p = p2;
	p2p[2] = 1.0f;

	if (ret * p2p < 0)
		ret = ret * -1.0f;

	return ret;

}