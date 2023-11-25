#include "M33.h"

M33::M33(V3 row1, V3 row2, V3 row3) {
	rows[0] = row1;
	rows[1] = row2;
	rows[2] = row3;
}

M33 M33::RotationMatrix(int principal_axis, float theta) {
	M33 ret;
	switch(principal_axis) {
		case X_AXIS:
			ret.rows[0] = { 1, 0, 0 };
			ret.rows[1] = { 0, cos(theta), -sin(theta) };
			ret.rows[2] = { 0, sin(theta), cos(theta) };
			break;
		case Y_AXIS:
			ret.rows[0] = { cos(theta), 0, sin(theta) };
			ret.rows[1] = { 0, 1, 0 };
			ret.rows[2] = { -sin(theta), 0, cos(theta) };
			break;
		case Z_AXIS:
			ret.rows[0] = { cos(theta), -sin(theta), 0 };
			ret.rows[1] = { sin(theta), cos(theta), 0 };
			ret.rows[2] = { 0, 0, 1 };
			break;
	}
	return ret;
}

V3& M33::operator[](int i) {

	return rows[i];

}

V3 M33::GetColumn(int i) {

	M33& m = *this;
	V3 ret;
	ret[0] = m[0][i];
	ret[1] = m[1][i];
	ret[2] = m[2][i];
	return ret;

}

void M33::SetColumn(int i, V3 colv) {

	M33& m = *this;
	m[0][i] = colv[0];
	m[1][i] = colv[1];
	m[2][i] = colv[2];

}

V3 M33::operator*(V3 v) {

	M33& m = *this;
	V3 ret;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;
}

M33 M33::Inverted() {

	M33 ret;
	V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	V3 _a = b ^ c; _a = _a / (a * _a);
	V3 _b = c ^ a; _b = _b / (b * _b);
	V3 _c = a ^ b; _c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}

M33 M33::Transposed() {
	M33 ret;
	ret[0] = this->GetColumn(0);
	ret[1] = this->GetColumn(1);
	ret[2] = this->GetColumn(2);
	return ret;
}


M33 M33::operator*(M33 m1) {

	M33& m0 = *this;
	M33 ret;
	ret.SetColumn(0, m0 * m1.GetColumn(0));
	ret.SetColumn(1, m0 * m1.GetColumn(1));
	ret.SetColumn(2, m0 * m1.GetColumn(2));
	return ret;
}

ostream& operator<<(ostream& ostr, M33 mat) {

	return ostr << mat[0] << " " << mat[1] << " " << mat[2];

}

// idk about this im just guessing
istream& operator>>(istream& istr, M33 mat) {
	return istr >> mat[0] >> mat[1] >> mat[2];
}
