#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "tm.h"
#include "ppc.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb;
	PPC* ppc;
	TM* tms;
	int tmsN;
	void Render();
	Scene();
	void DBG();
	void NewButton();
	V3 demoPoint;
	V3 demoAxisOrigin;
	V3 demoAxisDirection;
};

extern Scene *scene;