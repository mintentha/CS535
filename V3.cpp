
#include "V3.h"
#include "M33.h"

using namespace std;

#include <ostream>

V3::V3(float x, float y, float z) {

	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;

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

V3 V3::rotateVector(V3 a, float theta) {
	// Rather than rotating around some arbitrary origin we just rotate in a particular direction
	// So origin stays the same
	return rotatePoint(V3(0.0f, 0.0f, 0.0f), a, theta);
}

V3 V3::rotatePoint(V3 o_a, V3 a, float theta) {
	V3 aux;
	// Choose auxilliary axis with smaller dot product with a
	// x dot a = 1*a_x + 0*a_y + 0*a_z = a_x
	// y dot a = 0*a_x + 1*a_y + 0*a_z = a_y
	if (abs(a[0]) < abs(a[1])) {
		aux[0] = 1;
		aux[1] = 0;
		aux[2] = 0;
	}
	else {
		aux[0] = 0;
		aux[1] = 1;
		aux[2] = 0;
	}
	V3 b = (aux ^ a).normalized();
	V3 c = (a ^ b).normalized();
	M33 basisMat = M33();
	basisMat.SetColumn(0, a.normalized());
	basisMat.SetColumn(1, b);
	basisMat.SetColumn(2, c);
	// Get point in new coordinate system
	// To rotate we need to be at origin and translate back
	// We could translate to origin and back in new coordinate system,
	// but we would need to convert o_a to new coordinate system
	// so instead we just do the translation in original coordinate system
	V3 pPrime = basisMat * (*this - o_a);
	// we put a as first axis so we rotate around x axis
	V3 pPrimePrime = M33(M33::X_AXIS, theta * M_PI / 180.0f) * pPrime;
	// Now we 
	V3 pR = (basisMat.Transposed() * pPrimePrime) + o_a;
	return pR;
}