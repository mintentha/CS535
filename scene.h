#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "tm.h"
#include "ppc.h"
#include "CubeMap.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb, *fb3;
	PPC* ppc, *ppc3;
	TM* tms;
	CubeMap *cube;
	int tmsN;
	void Render();
	void PointRender(PPC* cppc, FrameBuffer* cfb);
	void WFRender(PPC* cppc, FrameBuffer* cfb);
	void FilledRender(PPC* cppc, FrameBuffer* cfb);
	void DrawBackground(PPC* ppc, FrameBuffer* cfb);
	Scene();
	void DBG();
	void NewButton();
	bool shadowsEnabled;
	V3 L;
	float ka;
	int shadowsOn;
	FrameBuffer* shfb;
	PPC* shppc;
};

extern Scene *scene;