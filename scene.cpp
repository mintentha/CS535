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
	tmsN = 1;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1K.bin");
	//	tms[0].LoadBin("geometry/teapot57K.bin");
	V3 tv(20.0f, -20.0f, -200.0f);
	tms[0].Translate(tv);
	tms[0].on = true;
	tms[0].shininess = 0.6f;

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

void Scene::Render() {
	if (shadowsEnabled) {
		shadowsOn = 0;
		FilledRender(shppc, shfb);
		shadowsOn = 1;
	}
	FilledRender(ppc, fb);
	FilledRender(ppc3, fb3);
	float visFocalLength = 20.0f;
	ppc->Visualize(visFocalLength, ppc3, fb3);
	//fb3->Draw3DPoint(L, V3(1.0f, 1.0f, 0.0f), 17, ppc3);
	shadowsOn = 0;
}

void Scene::DBG() {
	int framesN = 600;
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
			//L = L + V3(0.0f, 0.2f, 0.0f);
			if (shadowsEnabled) {
				shppc->SetPose(L, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
			}
			Render();
			Fl::check();
			if (fi < 200) {
				ppc->revolve(PPC::PAN, 1.0f, tms[0].GetCenter());
			} else if (fi < 400) {
				ppc->revolve(PPC::TILT, 1.0f, tms[0].GetCenter());
			} else {
				ppc->revolve(PPC::ROLL, 1.0f, tms[0].GetCenter());
			}
			fi++;
		}
	}
	return;
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}



