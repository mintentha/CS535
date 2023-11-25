#include "tm.h"
#include "M33.h"
#include "scene.h"

using namespace std;

#include <fstream>
#include <iostream>

void TM::Allocate(int vsN, int trsN) {

	vertsN = vsN;
	trisN = trsN;
	verts = new V3[vertsN];
	pverts = new V3[vertsN];
	colors = new V3[vertsN];
	normals = new V3[vertsN];
	tcs = new V3[vertsN];
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
	normals[0] =
		normals[1] =
		normals[2] =
		normals[3] = ((verts[1] - verts[0]) ^ (verts[2] - verts[0])).normalized();

	tcs[0][0] = 0.0f;
	tcs[0][1] = 0.0f;

	tcs[1][0] = 0.0f;
	tcs[1][1] = 1.0f;

	tcs[2][0] = 1.0f;
	tcs[2][1] = 1.0f;

	tcs[3][0] = 1.0f;
	tcs[3][1] = 0.0f;

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
	pverts = new V3[vertsN];

	ifs.read(&yn, 1); // cols 3 floats
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
	for (int vert = 0; vert < vertsN; vert++) {
		if (!ppc->Project(verts[vert], pverts[vert])) {
			// should not be projected, keep track of this
			pverts[vert][0] = FLT_MAX;
		}
	}
	for (int tri = 0; tri < 3 * trisN; tri += 3) {
		int tri_vert_inds[] = { tris[tri], tris[tri + 1], tris[tri + 2] };
		V3 tri_proj_verts[] = { pverts[tri_vert_inds[0]],
								pverts[tri_vert_inds[1]],
								pverts[tri_vert_inds[2]] };
		if (tri_proj_verts[0][0] == FLT_MAX) {
			continue;
		}
		AABB aabb(tri_proj_verts, 3);
		aabb.ClipWithImageFrame(fb->w, fb->h);
		int left = (int)aabb.corners[0][0];
		int right = (int)aabb.corners[1][0];
		int top = (int)aabb.corners[0][1];
		int bottom = (int)aabb.corners[1][1];

		// set up edge equations
		M33 eeqM;
		V3 dummy;
		for (int ei = 0; ei < 3; ei++)
			eeqM[ei] = dummy.EdgeEquation(tri_proj_verts[ei],
				tri_proj_verts[(ei + 1) % 3], tri_proj_verts[(ei + 2) % 3]);

		// set up linear screen space interpolation of rasterization parameters
		M33 ssim;
		ssim[0] = tri_proj_verts[0];
		ssim[1] = tri_proj_verts[1];
		ssim[2] = tri_proj_verts[2];
		ssim.SetColumn(2, V3(1.0f, 1.0f, 1.0f));
		ssim = ssim.Inverted();

		// set up linear screen space interpolation of colors
		M33 vcols;
		vcols[0] = colors[tri_vert_inds[0]];
		vcols[1] = colors[tri_vert_inds[1]];
		vcols[2] = colors[tri_vert_inds[2]];
		M33 colsABCs;
		colsABCs = (ssim * vcols).Transposed();

		// set up linear screen space interpolation of texture coordinates
		M33 vtcs;
		M33 tcsABCs;
		if (tcs) {
			vtcs[0] = tcs[tri_vert_inds[0]];
			vtcs[1] = tcs[tri_vert_inds[1]];
			vtcs[2] = tcs[tri_vert_inds[2]];
			tcsABCs = (ssim * vtcs).Transposed();
		}


		// set up linear screen space interpolation of 1/w
		V3 zABC = ssim * V3(tri_proj_verts[0][2], tri_proj_verts[1][2], tri_proj_verts[2][2]);

		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixCenter(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 es = eeqM * pixCenter;
				// if on wrong side of any of the edges continue
				if (es[0] < 0.0f || es[1] < 0.0f || es[2] < 0.0f)
					continue;

				// if farther than what has already been seen at current pixel, continue
				float currz = zABC * pixCenter;
				float oldz = fb->GetZ(u, v);
				if (oldz > currz)
					continue;
				fb->SetZ(u, v, currz);

				if (texture) {
					V3 currtcs = tcsABCs * pixCenter;
					int tu = (int)(currtcs[0] * (float)texture->w);
					int tv = (int)(currtcs[1] * (float)texture->h);
					fb->Set(u, v, texture->Get(tu, tv));
				}
				else {
					V3 currColor = colsABCs * pixCenter;
					fb->Set(u, v, currColor.GetColor());
				}

				if (scene->shadowsOn) {
					// if in shadow set to black for now
					V3 P; ppc->UnProject(V3(pixCenter[0], pixCenter[1], currz), P);
					V3 LP;
					if (!scene->shppc->Project(P, LP))
						continue;
					int shu = (int)LP[0];
					int shv = (int)LP[1];
					float shz = scene->shfb->GetZ(shu, shv);
					if (shz == FLT_MAX)
						continue;
					if (shz > LP[2] + 0.05f)
						fb->Set(u, v, 0xFF000000);
				}

			}
		}
	}
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