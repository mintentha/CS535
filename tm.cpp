#include "tm.h"

using namespace std;

#include <fstream>
#include <iostream>
#include "M33.h"

void TM::Allocate(int vsN, int trsN) {
	
	vertsN = vsN;
	trisN = trsN;
	verts = new V3[vertsN];
	colors = new V3[vertsN];
	tris = new unsigned int[trisN * 3];

}


void TM::SetQuad(V3 v0, V3 v1, V3 v2, V3 v3) {

	Allocate(4, 2);
	verts[0] = v0;
	verts[1] = v1;
	verts[2] = v2;
	verts[3] = v3;
	int tri = 0;
	tris[3 * tri + 0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;

}


void TM::LoadBin(char *fname) {

	ifstream ifs(fname, ios::binary);
	if (ifs.fail()) {
		cerr << "INFO: cannot open file: " << fname << endl;
		return;
	}

	ifs.read((char*)&vertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
		return;
	}
	if (verts)
		delete verts;
	verts = new V3[vertsN];

	ifs.read(&yn, 1); // cols 3 floats
	V3 *normals = 0;
	if (colors) {
		delete colors;
		colors = 0;
	}
	if (yn == 'y') {
		colors = new V3[vertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (normals)
		delete normals;
	normals = 0;
	if (yn == 'y') {
		normals = new V3[vertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[vertsN * 2];
	}


	ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

	if (colors) {
		ifs.read((char*)colors, vertsN * 3 * sizeof(float)); // load cols
	}

	if (normals)
		ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}

void TM::RenderPoints(FrameBuffer* fb, int psize, PPC* ppc) {

	for (int vi = 0; vi < vertsN; vi++) {
		fb->Draw3DRoundPoint(verts[vi], colors[vi], psize, ppc);
	}

}

void TM::RenderWF(FrameBuffer* fb, PPC* ppc) {

	for (int tri = 0; tri < trisN; tri++) {
		for (int ei = 0; ei < 3; ei++) {
			int nei = (ei + 1) % 3;
			unsigned int vi0 = tris[3 * tri + ei];
			unsigned int vi1 = tris[3 * tri + nei];
			fb->Draw3DSegment(verts[vi0], colors[vi0], verts[vi1], colors[vi1], ppc);
		}
	}

}

void TM::RenderFilled(FrameBuffer* fb, PPC* ppc) {
	//std::cout << "----------------------------------" << std::endl;
	// Don't want to have to re-project the same point for every single triangle it's in
	// So make a new array of the projected points for faster operation
	V3* projected_verts = new V3[vertsN];
	for (int vert = 0; vert < vertsN; vert++) {
		if (!ppc->Project(verts[vert], projected_verts[vert])) {
			// should not be projected, keep track of this
			verts[vert] = V3(-INFINITY, -INFINITY, -INFINITY);
		}
	}
	for (int tri = 0; tri < 3*trisN; tri += 3) {
		int tri_vert_inds[] = { tris[tri], tris[tri + 1], tris[tri + 2] };
		V3 tri_proj_verts[] = { projected_verts[tri_vert_inds[0]],
								projected_verts[tri_vert_inds[1]],
								projected_verts[tri_vert_inds[2]] };
		if (tri_proj_verts[0][0] == -INFINITY ||
			tri_proj_verts[1][0] == -INFINITY ||
			tri_proj_verts[2][0] == -INFINITY) {
			continue;
		}
		AABB aabb(tri_proj_verts, 3);
		// Edge expressions
		V3 a, b, c;
		float sidedness; // temp variable for each edge
		// First edge through 0 and 1
		a[0] = tri_proj_verts[1][1] - tri_proj_verts[0][1];
		b[0] = -tri_proj_verts[1][0] + tri_proj_verts[0][0];
		c[0] = -tri_proj_verts[0][0] * tri_proj_verts[1][1] + tri_proj_verts[0][1] * tri_proj_verts[1][0];
		sidedness = a[0] * tri_proj_verts[2][0] + b[0] * tri_proj_verts[2][1] + c[0];
		if (sidedness < 0) {
			a[0] *= -1;
			b[0] *= -1;
			c[0] *= -1;
		}
		// Second edge through 1 and 2
		a[1] = tri_proj_verts[2][1] - tri_proj_verts[1][1];
		b[1] = -tri_proj_verts[2][0] + tri_proj_verts[1][0];
		c[1] = -tri_proj_verts[1][0] * tri_proj_verts[2][1] + tri_proj_verts[1][1] * tri_proj_verts[2][0];
		sidedness = a[1] * tri_proj_verts[0][0] + b[1] * tri_proj_verts[0][1] + c[1];
		if (sidedness < 0) {
			a[1] *= -1;
			b[1] *= -1;
			c[1] *= -1;
		}
		// Third edge through 2 and 0
		a[2] = tri_proj_verts[0][1] - tri_proj_verts[2][1];
		b[2] = -tri_proj_verts[0][0] + tri_proj_verts[2][0];
		c[2] = -tri_proj_verts[2][0] * tri_proj_verts[0][1] + tri_proj_verts[2][1] * tri_proj_verts[0][0];
		sidedness = a[2] * tri_proj_verts[1][0] + b[2] * tri_proj_verts[1][1] + c[2];
		if (sidedness < 0) {
			a[2] *= -1;
			b[2] *= -1;
			c[2] *= -1;
		}
		// Slides have float bbox[2][2] where first ind is x/y, second int is low/high
		/*std::cout << "cornerHigh " << aabb.cornerHigh << std::endl <<
					 "cornerLow " << aabb.cornerLow << std::endl;*/
		int left = aabb.cornerLow[0] + 0.5f;
		int right = aabb.cornerHigh[0] - 0.5f;
		int top = aabb.cornerLow[1] + 0.5f;
		int bottom = aabb.cornerHigh[1] - 0.5f;
		/*std::cout << "left: " << left << std::endl <<
					 "right: " << right << std::endl <<
					 "top: " << top << std::endl <<
					 "bottom: " << bottom << std::endl;*/
		// Need to interpolate color and z
		// LSSI matrix is each row is u_i, v_i, 1
		// Matrix multiples by [A, B, C] to get [r_0, r_1, r_2]
		M33 LSSI_mat(tri_proj_verts[0], tri_proj_verts[1], tri_proj_verts[2]);
		LSSI_mat.SetColumn(2, V3(1.0f, 1.0f, 1.0f));
		M33 LSSI_mat_inv = LSSI_mat.Inverted();
		V3 z_values(tri_proj_verts[0][2], tri_proj_verts[1][2], tri_proj_verts[2][2]);
		V3 z_int = LSSI_mat_inv * z_values; // multiply by point to interpolate z
		M33 c_values;
		c_values.SetColumn(0, colors[tri_vert_inds[0]]);
		c_values.SetColumn(1, colors[tri_vert_inds[1]]);
		c_values.SetColumn(2, colors[tri_vert_inds[2]]);
		// rows are r,g,b
		M33 c_int;
		c_int[0] = LSSI_mat_inv * c_values[0];
		c_int[1] = LSSI_mat_inv * c_values[1];
		c_int[2] = LSSI_mat_inv * c_values[2];
		// now multiplying point by each row gives r, g, and b respectively
		// multiplying matrix by vector is the vector of each of the rows multiplied by it
		int currPixX, currPixY;
		V3 currEELS = a * (left + 0.5f) + b*(top + 0.5f) + c;
		// this is the same as (a, b, c) * (x, y, 1)
		// and each increment increases along with v and u so its the same as this for each step
		/*M33 edge_expr;
		edge_expr.SetColumn(0, a);
		edge_expr.SetColumn(1, b);
		edge_expr.SetColumn(2, c);*/
		// right now its mostly working but doesnt look exactly the same so im going back to the first method
		for (int v = top; v <= bottom; v++, currEELS = currEELS + b) {
			V3 currEE = currEELS;
			for (int u = left; u <= right; u++, currEE = currEE + a) {
				V3 pix(u, v, 1.0f);
				//V3 currEE = edge_expr * pix;
				if (currEE[0] < 0.0f || currEE[1] < 0.0f || currEE[2] < 0.0f) {
					continue;
				}
				// Determine if pixel is inside triangle or outside

				//std::cout << u << " " << "v" << std::endl;
				// Interpolate z and color
				float z = z_int * pix;
				// would using pixel with new z make it more accurate than using 1 as z?
				if (u < 0 || u > fb->w - 1 || v < 0 || v > fb->h - 1  || fb->GetZ(u, v) > z)
					continue;
				V3 color = c_int * pix;
				/*float red = c_int[0] * pix;
				float green = c_int[1] * pix;
				float blue = c_int[2] * pix;
				V3 color(red, green, blue);*/
				fb->Set(u, v, color.GetColor());
				fb->SetZ(u, v, z);
			}
		}
	}
	delete[] projected_verts;
}

void TM::Translate(V3 tv) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + tv;
	}

}

V3 TM::GetCenter() {

	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++)
		ret = ret + verts[vi];
	ret = ret / (float)vertsN;
	return ret;

}


void TM::Rotate(V3 aO, V3 aD, float theta) {

	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi].rotatePoint(aO, aD, theta);

}

void TM::Light(V3 L, float ka) {

	for (int vi = 0; vi < vertsN; vi++) {
		colors[vi] = colors[vi].Light(verts[vi], normals[vi], L, ka);
	}

}



void TM::VisualizeNormals(float d, PPC* visppc, FrameBuffer* visfb) {

	if (!normals)
		return;

	for (int vi = 0; vi < vertsN; vi++) {
		visfb->Draw3DSegment(verts[vi], colors[vi], verts[vi] + normals[vi] * d, V3(1.0f, 0.0f, 0.0f), visppc);
	}

}


void TM::SetAllColors(V3 col) {

	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = col;

}

void TM::SetCenter(V3 center) {
	Translate(center - GetCenter());
}

AABB TM::GetAABB() {
	return AABB(verts, vertsN);
}

void TM::Scale(float amt) {
	V3 center = GetCenter();
	for (int i = 0; i < vertsN; i++) {
		verts[i] = ((verts[i] - center) * amt) + center;
	}
}

/*
void TM::FitAABB(AABB new_aabb) {
	SetCenter(new_aabb.GetCenter());
	AABB old_aabb = GetAABB();
	float factorDx = new_aabb.GetDX() / old_aabb.GetDX();
	float factorDy = new_aabb.GetDY() / old_aabb.GetDY();
	float factorDz = new_aabb.GetDZ() / old_aabb.GetDZ();
	float minFactor = min(factorDx, factorDy, factorDz);
	if (minFactor == 1) {
		return;
	}
	if (minFactor < 1) {
		// We must shrink but only as much as we need to
		// Would be the maximum scale factor which is also < 1
		if (factorDx > 1) {
			factorDx = 0;
		}
		if (factorDy > 1) {
			factorDy = 0;
		}
		if (factorDz > 1) {
			factorDz = 0;
		}
		Scale(max(factorDx, factorDy, factorDz));
		return;
	}
	// otherwise we grow, but only as much as we're able to on one axis
	Scale(minFactor);
}
*/