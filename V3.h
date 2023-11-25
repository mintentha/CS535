
#pragma once

#include <ostream>
#include <istream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

class V3 {
public:
	float xyz[3];
	V3() {};
	V3(float x, float y, float z);
	float& operator[](int i);
	V3 operator+(V3 op1);
	V3 operator-(V3 op1);
	float operator*(V3 op1);
	V3 operator*(float scf);
	V3 operator/(float scf);
	V3 operator^(V3 op1);
	friend ostream& operator<<(ostream& ostr, V3 v);
	friend istream& operator>>(istream& istr, V3 v);
	V3 normalized();
	float length();
	unsigned int GetColor();
	V3 rotatePoint(V3 o_a, V3 a, float theta);
	V3 rotateVector(V3 a, float theta);
};