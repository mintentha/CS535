#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"

class PPC;

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int *pix; // pixel array
	int w, h;
	FrameBuffer(int u0, int v0, int _w, int _h);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	void LoadTiff(char* fname);
	void SaveAsTiff(char* fname);
};