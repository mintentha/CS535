#pragma once

#include "V3.h"

using namespace std;


class M33 {
public:
	static const int X_AXIS = 0;
	static const int Y_AXIS = 1;
	static const int Z_AXIS = 2;
	V3 rows[3];
	M33() {};
	M33(V3 row1, V3 row2, V3 row3);
	static M33 RotationMatrix(int principal_axis, float theta);
	V3& operator[](int i);
	void SetColumn(int i, V3 colv);
	V3 GetColumn(int i);
	V3 operator*(V3 v);
	M33 operator*(M33 m1);
	M33 Inverted();
	M33 Transposed();
	friend ostream& operator<<(ostream& ostr, M33 mat);
	friend istream& operator>>(istream& istr, M33 mat);
};