
#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) :
	Fl_Gl_Window(u0, v0, _w, _h, 0) {

	w = _w;
	h = _h;
	pix = new unsigned int[w * h];
	zb = new float[w * h];

}

void FrameBuffer::ClearZB() {


	for (int i = 0; i < w * h; i++)
		zb[i] = 0.0f;

}

void FrameBuffer::draw() {

	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);

}

int FrameBuffer::handle(int event) {

	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			return 0;
		cerr << u << " " << v << "          \r";
		return 0;
	}
	default:
		return 0;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle() {
	int key = Fl::event_key();
	switch (key) {
	case FL_Left: {
		cerr << "INFO: pressed left" << endl;
		
		break;
	}
	default:
		cerr << "INFO: do not understand keypress" << endl;
		return;
	}

}

void FrameBuffer::SetBGR(unsigned int bgr) {

	for (int uv = 0; uv < w * h; uv++)
		pix[uv] = bgr;

}



// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w * h];
		size(w, h);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(const char* fname) {

	TIFF* out = TIFFOpen(fname, "w");

	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}

void FrameBuffer::Set(int u, int v, unsigned int col) {

	int uv = (h - 1 - v) * w + u;
	pix[uv] = col;

}

unsigned int FrameBuffer::Get(int u, int v) {

	int uv = (h - 1 - v) * w + u;
	return pix[uv];

}

float FrameBuffer::GetZ(int u, int v) {

	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return FLT_MAX;
	int uv = (h - 1 - v) * w + u;
	return zb[uv];

}

void FrameBuffer::SetZ(int u, int v, float z) {

	int uv = (h - 1 - v) * w + u;
	zb[uv] = z;

}


void FrameBuffer::SetAsChecker(unsigned int col0, unsigned int col1, int csize) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			int cu = u / csize;
			int cv = v / csize;
			if ((cu + cv) % 2) {
				Set(u, v, col0);
			}
			else
				Set(u, v, col1);
		}
	}

}



void FrameBuffer::Draw2DPoint(V3 p, int psize, unsigned int col) {

	float uf = p[0];
	float vf = p[1];
	int u0 = (int)uf;
	int v0 = (int)vf;
	for (int v = v0 - psize / 2; v <= v0 + psize / 2; v++)
		for (int u = u0 - psize / 2; u <= u0 + psize / 2; u++) {
			if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
				continue;
			float oldz = GetZ(u, v);
			if (oldz > p[2])
				continue;
			SetZ(u, v, p[2]);
			Set(u, v, col);
		}
}

void FrameBuffer::Draw2DCircle(V3 p, int r, unsigned int col) {

	float uf = p[0];
	float vf = p[1];
	int u0 = (int)uf;
	int v0 = (int)vf;
	for (int v = v0 - r / 2; v <= v0 + r / 2; v++)
		for (int u = u0 - r / 2; u <= u0 + r / 2; u++) {
			if (u < 0 || u > w - 1 || v < 0 || v > h - 1 || ((u - u0)*(u - u0) + (v - v0)*(v - v0)) > r)
				continue;
			float oldz = GetZ(u, v);
			if (oldz > p[2])
				continue;
			SetZ(u, v, p[2]);
			Set(u, v, col);
		}
}


void FrameBuffer::Draw3DPoint(V3 P, V3 colv, int pSize, PPC* ppc) {

	V3 pP;
	if (!ppc->Project(P, pP))
		return;
	Draw2DPoint(pP, pSize, colv.GetColor());

}

void FrameBuffer::Draw3DRoundPoint(V3 P, V3 colv, int r, PPC* ppc) {

	V3 pP;
	if (!ppc->Project(P, pP))
		return;
	Draw2DCircle(pP, r, colv.GetColor());

}


void FrameBuffer::Draw2DRectangle(V3 p0, V3 p1, unsigned int col) {
	int u0 = min(p0[0], p1[0]);
	int v0 = min(p0[1], p1[1]);
	int u1 = max(p0[0], p1[0]);
	int v1 = max(p0[1], p1[1]);
	for (int u = u0; u <= u1; u++) {
		for (int v = v0; v <= v1; v++) {
			Set(u, v, col);
		}
	}
}

void FrameBuffer::Draw3DSegment(V3 V0, V3 c0, V3 V1, V3 c1, PPC* ppc) {

	V3 pV0;
	if (!ppc->Project(V0, pV0))
		return;
	if (pV0[0] < 0.0f || pV0[0] > (float)w || pV0[1] < 0.0f || pV0[1] > (float) h)
		return;
	V3 pV1;
	if (!ppc->Project(V1, pV1))
		return;
	if (pV1[0] < 0.0f || pV1[0] > (float)w || pV1[1] < 0.0f || pV1[1] > (float) h)
		return;

	int samplesN;
	float uspan = fabsf(pV0[0] - pV1[0]);
	float vspan = fabsf(pV0[1] - pV1[1]);
	samplesN = (uspan < vspan) ? (int)vspan : (int)uspan;
	samplesN += 2;

	V3 cp;
	V3 dp = (pV1 - pV0) / (float)(samplesN - 1);
	V3 cc;
	V3 dc = (c1 - c0) / (float)(samplesN - 1);
	int si;
	for (si = 0, cp = pV0, cc = c0; si < samplesN; si++, cp = cp + dp, cc = cc + dc) {
		Draw2DPoint(cp, 1, cc.GetColor());
	}

}

void FrameBuffer::DrawXYRectangle(V3 p0, V3 p1, V3 col, PPC* ppc) {
	if (p0[2] != p1[2])
		return;
	int x0 = min(p0[0], p1[0]);
	int x1 = max(p0[0], p1[0]);
	int y0 = min(p0[1], p1[1]);
	int y1 = max(p0[1], p1[1]);
	for (int x = x0; x <= x1; x++) {
		for (int y = y0; y <= y1; y++) {
			Draw3DPoint(V3(x, y, p0[2]), col, 1, ppc);
		}
	}
	// This approach kinda has an issue because if its deep then the points are two sparse with many holes
}

void FrameBuffer::DrawXZRectangle(V3 p0, V3 p1, V3 col, PPC* ppc) {
	if (p0[1] != p1[1])
		return;
	int x0 = min(p0[0], p1[0]);
	int x1 = max(p0[0], p1[0]);
	int z0 = min(p0[2], p1[2]);
	int z1 = max(p0[2], p1[2]);
	for (int x = x0; x <= x1; x++) {
		for (int z = z0; z <= z1; z++) {
			Draw3DPoint(V3(x, p0[1], z), col, 1, ppc);
		}
	}
}

void FrameBuffer::DrawYZRectangle(V3 p0, V3 p1, V3 col, PPC* ppc) {
	if (p0[0] != p1[0])
		return;
	int y0 = min(p0[1], p1[1]);
	int y1 = max(p0[1], p1[1]);
	int z0 = min(p0[2], p1[2]);
	int z1 = max(p0[2], p1[2]);
	for (int y = y0; y <= y1; y++) {
		for (int z = z0; z <= z1; z++) {
			Draw3DPoint(V3(p0[0], y, z), col, 1, ppc);
		}
	}
}
void FrameBuffer::VisualizeSamples(PPC* ppc, PPC* visppc, FrameBuffer* visfb, float zPlane = 0.0f) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			V3 pP(.5f + (float)u, .5f + (float)v, GetZ(u, v));
			if (zPlane != 0.0f)
				pP[2] = zPlane;
			V3 P;
			if (ppc->UnProject(pP, P) && Get(u, v) != 0xFFFFFFFF) {
				V3 colv; colv.SetColor(Get(u, v));
				visfb->Draw3DPoint(P, colv, 1, visppc);
			}
		}
	}

}
