#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "tm.h"
#include "ppc.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb, *fb3;
	PPC* ppc, *ppc3;
	TM* tms;
	int tmsN;
	void Render();
	void PointRender(PPC* cppc, FrameBuffer* cfb);
	void WFRender(PPC* cppc, FrameBuffer* cfb);
	void FilledRender(PPC* cppc, FrameBuffer* cfb);
	Scene();
	void DBG();
	void NewButton();

	V3 L;
	float ka;
};

extern Scene *scene;