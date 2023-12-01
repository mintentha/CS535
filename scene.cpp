#include "scene.h"

#include "V3.h"
#include "M33.h"
#include "CubeMap.h"

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>

Scene::Scene() {


	cgi = 0;
	soi = 0;
	needInitHW = 1;

	hwfb = 0;


	gui = new GUI();
	gui->show();

	int u0 = 16;
	int v0 = 40;
	int h = 400;
	int w = 600;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("first");
	fb->show();
	fb->SetBGR(0xFF0000FF);
	fb->redraw();

	gui->uiw->position(u0, v0 + fb->h + v0);

	float hfov = 55.0f;
	ppc = new PPC(hfov, fb->w, fb->h);
	ppc->translate(PPC::DIR_UP_DOWN, 15.0f);
	tmsN = 2;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1K.bin");
	//	tms[0].LoadBin("geometry/teapot57K.bin");
	V3 tv(20.0f, -20.0f, -200.0f);
	tms[0].Translate(tv);
	tms[0].on = true;
	tms[0].shininess = 0.6f;

	FrameBuffer* texture1 = new FrameBuffer(10, 10, 128, 128);
	texture1->LoadTiff("textures/fuzz.tiff");

	AABB aabb = tms[0].GetAABB();
	tms[1].SetQuad(
		V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]),
		V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]),
		V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]),
		V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2])
	);
	V3 qc = tms[1].GetCenter();
	tms[1].Translate(qc * -1.0f);
	tms[1].Scale(3.0f);
	tms[1].Translate(qc);
	tms[1].SetAllColors(V3(0.9f, 0.9f, 0.9f));
	tms[1].texture = texture1;
	tms[1].on = true;

	FrameBuffer *cross = new FrameBuffer(0, 0, 0, 0);
	cross->LoadTiff("uffizi_cross.tiff");
	cross->label("Cross");
	//cross->show();
	CubeMap *cubeMap = new CubeMap(cross);
	delete cross;
	this->cube = cubeMap;

	fb3 = new FrameBuffer(u0, v0, w, h);
	fb3->position(u0 + w + u0, v0);
	fb3->label("third");
	fb3->show();
	fb3->SetBGR(0xFFFFFFFF);
	fb3->redraw();
	ppc3 = new PPC(30.0f, fb3->w, fb3->h);
	ppc3->SetPose(ppc3->C + V3(100.0f, 200.0f, 200.0f), tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));

	L = ppc->C;
	L = tms[0].GetCenter() + V3(-100.0f, 90.0f, -50.0f);
	ka = 0.5f;
	shfb = NULL;
	shppc = NULL;
	shadowsOn = 0;

	shadowsEnabled = false;
	wfEnabled = false;
}

void Scene::PointRender(PPC* cppc, FrameBuffer* cfb) {
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderPoints(cfb, psize, cppc);
	}
	cfb->redraw();
}

void Scene::DrawBackground(PPC* ppc, FrameBuffer *cfb) {
	cfb->SetBGR(0xFF880000);
	if (cube) {
		for (int u = 0; u < ppc->w; u++) {
			for (int v = 0; v < ppc->h; v++) {
				V3 dir;
				ppc->UnProject(V3(u + 0.5f, v + 0.5f, 1.0f), dir);
				dir = dir - ppc->C;
				unsigned int col = cube->DirectionLookup(dir);
				cfb->Set(u, v, col);
			}
		}
	}
	cfb->ClearZB();
}

void Scene::WFRender(PPC* cppc, FrameBuffer* cfb) {
	DrawBackground(cppc, cfb);
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderWF(cfb, cppc);
	}
	cfb->redraw();
}

void Scene::FilledRender(PPC* cppc, FrameBuffer* cfb) {
	DrawBackground(cppc, cfb);
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		if (!tms[tmi].on)
			continue;
		tms[tmi].RenderFilled(cfb, cppc);
	}
	cfb->redraw();
}

void Scene::RenderHW(PPC *cppc, FrameBuffer *cfb) {

	int shaders = 1;
	if (needInitHW) {
		glEnable(GL_DEPTH_TEST);
		// TEXTUREHINT: create textures
			// load tiff from file to FrameBuffer
			// tell hardware how many textures you want for it to create handles (texture ID's or pointers)
			// pass pix data of FrameBuffer to hardware to actually create texture
		if (shaders) {
			cgi = new CGInterface();
			cgi->PerSessionInit();
			soi = new ShaderOneInterface();
			soi->PerSessionInit(cgi);
		}
		needInitHW = 0;
	}

	if (shaders) {
		cgi->EnableProfiles();
		soi->PerFrameInit();
	}


	glClearColor(0.75f, 0.5f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ppc->SetIntrinsicsHW();
	ppc->SetExtrinsicsHW();

	for (int tmi = 0; tmi < tmsN; tmi++) {
		if (!tms[tmi].on)
			continue;
		tms[tmi].RenderHW();
	}


	if (shaders)
		cgi->DisableProfiles();

}

void Scene::Render() {
	if (shadowsEnabled && shppc && shfb) {
		shadowsOn = 0;
		if (wfEnabled) {
			WFRender(shppc, shfb);
		} else {
			FilledRender(shppc, shfb);
		}
		shadowsOn = 1;
	}
	if (wfEnabled) {
		WFRender(ppc, fb);
	} else {
		FilledRender(ppc, fb);
	}
	if (hwfb) {
		hwfb->redraw();
	}
	if (wfEnabled) {
		WFRender(ppc3, fb3);
	} else {
		FilledRender(ppc3, fb3);
	}
	float visFocalLength = 20.0f;
	ppc->Visualize(visFocalLength, ppc3, fb3);
	//fb3->Draw3DPoint(L, V3(1.0f, 1.0f, 0.0f), 17, ppc3);
	shadowsOn = 0;
}

void Scene::DBG() {
	ppc->C = ppc->C + ppc->GetVD() * 100.0f;
	//fb->hide();
	fb3->hide();
	int u0 = 10;
	int v0 = 40;
	int h = 400;
	int w = 600;
	hwfb = new FrameBuffer(u0 + fb->w + u0, v0, w, h);
	hwfb->label("hw");
	hwfb->show();
	hwfb->ishw = 1;
	Render();
	return;
	int framesN = 1000;
	int fi = 0;
	int fps = 30;
	std::chrono::duration<double> frame_length(1.0 / fps);
	auto last_frame = std::chrono::high_resolution_clock::now();
	if (shadowsEnabled) {
		if (!shfb) {
			shfb = new FrameBuffer(10, 10 + fb->h + 50, 256, 256);
		}
		if (!shppc) {
			shppc = new PPC(50.0f, shfb->w, shfb->h);
		}
		shfb->show();
		shfb->label("shadow view");
	}
	L = tms[0].GetCenter() + V3(-150.0f, 0.0f, 0.0f);
	//ppc->rotate(PPC::TILT, -90.0f);
	while (fi < framesN) {
		auto now = std::chrono::high_resolution_clock::now();
		if (now - last_frame >= frame_length) {
			morphAnimation = (float)fi / (float)(framesN - 1);
			Render();
			Fl::check();
			fi++;
		}
	}
	return;
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}



