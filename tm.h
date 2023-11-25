#pragma once


#include "V3.h"
#include "ppc.h"
#include "framebuffer.h"
#include "AABB.h"

class TM {
public:
	V3* verts, *colors, *normals;
	int vertsN;
	unsigned int *tris;
	int trisN;
	TM() : verts(0), vertsN(0), tris(0), trisN(0), colors(0) {};
	void SetQuad(V3 v0, V3 v1, V3 v2, V3 v3);
	void Allocate(int vsN, int trsN);
	void LoadBin(char *fname);
	void RenderPoints(FrameBuffer *fb, int psize, PPC *ppc);
	void RenderWF(FrameBuffer* fb, PPC* ppc);
	void RenderFilled(FrameBuffer *fb, PPC *ppc);
	void Translate(V3 tv);
	V3 GetCenter();
	void Rotate(V3 aO, V3 aD, float theta);
	void Light(V3 L, float ka);
	void VisualizeNormals(float d, PPC* visppc, FrameBuffer* visfb);
	void SetAllColors(V3 col);
	void SetCenter(V3 center);
	AABB GetAABB(); // 2 element array where each element is each corner
	void Scale(float amt);
	//void FitAABB(AABB new_aabb);
};