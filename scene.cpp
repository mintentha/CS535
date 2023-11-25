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
	tmsN = 4;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1K.bin");
	//	tms[0].LoadBin("geometry/teapot57K.bin");
	V3 tv(20.0f, -20.0f, -200.0f);
	tms[0].Translate(tv);
	//tms[0].on = false;


	FrameBuffer* texture = new FrameBuffer(10, 10, 128, 128);
	texture->SetAsChecker(0xFF000000, 0xFFAAAAAA, 32);
	texture->label("Texture");
	//texture->show();

	tms[1].SetQuad(V3(-100.0f, 100.0f, -350.0f),
		V3(-100.0f, -100.0f, -350.0f),
		V3(100.0f, -100.0f, -350.0f),
		V3(100.0f, 100.0f, -350.0f));
	tms[1].texture = texture;
	tms[1].on = false;

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


	tms[3].LoadBin("geometry/bunny.bin");
	//	tms[0].LoadBin("geometry/teapot57K.bin");
	tms[3].SetCenter(tms[0].GetCenter());
	tms[3].Scale(125.0f);
	tms[3].Translate(V3(70.0f, 0.0f, 20.0f));
	AABB bunnybb = tms[3].GetAABB();
	tms[3].Translate(V3(0.0f, -bunnybb.cornerLow[1] + aabb.cornerLow[1] + 10.0f, 0.0f));
	tms[3].SetAllColors(V3(1.0f, 0.5f, 0.76f));

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
	int framesN = 600;
	int fi = 0;
	int fps = 30;
	std::chrono::duration<double> frame_length(1.0 / fps);
	auto last_frame = std::chrono::high_resolution_clock::now();

	shfb = new FrameBuffer(10, 10 + fb->h + 50, 256, 256);
	shppc = new PPC(50.0f, shfb->w, shfb->h);
	ppc->SetPose(V3(0.0f, 40.0f, 0.0f), tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
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
			std::string filename("frames/");
			filename = filename + std::to_string(fi);
			filename = filename + ".tiff";
			fb->SaveAsTiff(filename.c_str());
			Fl::check();
			fi++;
		}
	}
	return;
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}



