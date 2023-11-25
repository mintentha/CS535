#pragma once


#include "V3.h"
#include "ppc.h"
#include "framebuffer.h"

class TM {
public:
	V3 *verts, *colors;
	int vertsN;
	unsigned int *tris;
	int trisN;
	TM() : verts(0), vertsN(0), tris(0), trisN(0), colors(0) {};
	void SetQuad(V3 v0, V3 v1, V3 v2, V3 v3);
	void Allocate(int vsN, int trsN);
	void LoadBin(char *fname);
	void RenderPoints(FrameBuffer *fb, int psize, PPC *ppc);
	void Translate(V3 tv);
};