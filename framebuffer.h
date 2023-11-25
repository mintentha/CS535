#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"
#include "ppc.h"

class PPC;

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int* pix; // pixel array // 0xAABBGGRR; // red 0xFF0000FF
	int w, h;
	float* zb;
	FrameBuffer(int u0, int v0, int _w, int _h);
	void ClearZB();
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, unsigned int col);
	unsigned int Get(int u, int v);
	float GetZ(int u, int v);
	void SetZ(int u, int v, float z);
	void SetAsChecker(unsigned int col0, unsigned int col1, int csize);
	void LoadTiff(char* fname);
	void SaveAsTiff(char* fname);
	void Draw2DCircle(V3 p, int r, unsigned int col);
	void Draw2DPoint(V3 p, int psize, unsigned int col);
	void Draw3DPoint(V3 P, V3 colv, int psize, PPC* ppc);
	void Draw3DRoundPoint(V3 P, V3 colv, int psize, PPC* ppc);
	void Draw2DRectangle(V3 p0, V3 p1, unsigned int col);
	void DrawXYRectangle(V3 p0, V3 p1, V3 col, PPC* ppc);
	void DrawXZRectangle(V3 p0, V3 p1, V3 col, PPC* ppc);
	void DrawYZRectangle(V3 p0, V3 p1, V3 col, PPC* ppc);
	void Draw3DSegment(V3 V0, V3 c0, V3 V1, V3 c1, PPC* ppc);
	void VisualizeSamples(PPC* ppc, PPC* visppc, FrameBuffer* visfb, float zplane);
};