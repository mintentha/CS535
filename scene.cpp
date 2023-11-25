#include "scene.h"

#include "V3.h"
#include "M33.h"

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
	tmsN = 6;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1K.bin");
	//	tms[0].LoadBin("geometry/teapot57K.bin");
	V3 tv(20.0f, -20.0f, -200.0f);
	tms[0].Translate(tv);
	tms[0].on = false;


	FrameBuffer* texture1 = new FrameBuffer(10, 10, 128, 128);
	texture1->LoadTiff("textures/brick.tiff");
	FrameBuffer* texture2 = new FrameBuffer(10, 10, 128, 128);
	texture2->LoadTiff("textures/chalkboard.tiff");
	FrameBuffer* texture3 = new FrameBuffer(10, 10, 128, 128);
	texture3->LoadTiff("textures/pillar.tiff");
	FrameBuffer* texture4 = new FrameBuffer(10, 10, 128, 128);
	texture4->LoadTiff("textures/fuzz.tiff");
	FrameBuffer* texture5 = new FrameBuffer(10, 10, 128, 128);
	texture5->LoadTiff("textures/reflection.tiff");
	//texture->show();

	AABB aabb = tms[0].GetAABB();
	tms[2].SetQuad(
		V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]),
		V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[1][2]),
		V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[1][2]),
		V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2])
	);
	V3 qc = tms[2].GetCenter();
	tms[2].Translate(qc * -1.0f);
	tms[2].Scale(3.0f);
	tms[2].Translate(qc);
	tms[2].SetAllColors(V3(0.9f, 0.9f, 0.9f));
	tms[2].texture = texture1;


	tms[1].SetQuad(
		V3(aabb.corners[0][0], aabb.corners[0][1], aabb.corners[0][2]),
		V3(aabb.corners[0][0], 150.0f, aabb.corners[0][2]),
		V3(aabb.corners[0][0] - 20, 150.0f, aabb.corners[1][2]),
		V3(aabb.corners[0][0] - 20, aabb.corners[0][1], aabb.corners[1][2]));
	tms[1].texture = texture2;
	tms[1].on = true;
	tms[1].QuadTextureSize(0.5f, 0.25f);


	tms[3].SetQuad(V3(-100.0f, 100.0f, -350.0f),
		V3(-100.0f, -100.0f, -350.0f),
		V3(100.0f, -100.0f, -350.0f),
		V3(100.0f, 100.0f, -350.0f));
	tms[3].texture = texture3;
	tms[3].on = true;
	tms[3].QuadTextureSize(0.5f, 0.25f);

	tms[4].SetQuad(V3(-100.0f, 100.0f, -350.0f),
		V3(-100.0f, -100.0f, -350.0f),
		V3(-200.0f, -100.0f, -350.0f),
		V3(-200.0f, 100.0f, -350.0f));
	tms[4].texture = texture4;
	tms[4].on = true;

	tms[5].SetQuad(
		V3(aabb.corners[1][0], aabb.corners[0][1], aabb.corners[0][2]),
		V3(aabb.corners[1][0], 150.0f, aabb.corners[0][2]),
		V3(aabb.corners[1][0] + 50, 150.0f, aabb.corners[1][2]),
		V3(aabb.corners[1][0] + 50, aabb.corners[0][1], aabb.corners[1][2]));
	tms[5].texture = texture5;
	tms[5].on = true;

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
	shfb = 0;
	shppc = 0;
	shadowsOn = 0;
}

void Scene::PointRender(PPC* cppc, FrameBuffer* cfb) {

	cfb->SetBGR(0xFFFFFFFF);
	cfb->ClearZB();
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderPoints(cfb, psize, cppc);
	}
	cfb->redraw();
}

void Scene::WFRender(PPC* cppc, FrameBuffer* cfb) {

	cfb->SetBGR(0xFFFFFFFF);
	cfb->ClearZB();
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderWF(cfb, cppc);
	}
	cfb->redraw();
}

void Scene::FilledRender(PPC* cppc, FrameBuffer* cfb) {

	cfb->SetBGR(0xFF880000);
	cfb->ClearZB();
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		if (!tms[tmi].on)
			continue;
		tms[tmi].RenderFilled(cfb, cppc);
	}
	cfb->redraw();
}

void Scene::Render() {
	FilledRender(ppc, fb);
	FilledRender(ppc3, fb3);
	float visFocalLength = 20.0f;
	ppc->Visualize(visFocalLength, ppc3, fb3);
	//fb3->Draw3DPoint(L, V3(1.0f, 1.0f, 0.0f), 17, ppc3);
}

void Scene::DBG() {
	int framesN = 300;
	int fi = 0;
	int fps = 30;
	std::chrono::duration<double> frame_length(1.0 / fps);
	auto last_frame = std::chrono::high_resolution_clock::now();
	shfb = new FrameBuffer(10, 10 + fb->h + 50, 256, 256);
	shppc = new PPC(50.0f, shfb->w, shfb->h);
	shfb->show();
	shfb->label("shadow view");
	L = tms[0].GetCenter() + V3(-150.0f, 0.0f, 0.0f);
	while (fi < framesN) {
		auto now = std::chrono::high_resolution_clock::now();
		if (now - last_frame >= frame_length) {
			L = L + V3(0.0f, 0.2f, 0.0f);
			shppc->SetPose(L, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
			shadowsOn = 0;
			FilledRender(shppc, shfb);
			shadowsOn = 1;
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



