#include "scene.h"

#include "V3.h"
#include "M33.h"

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

Scene::Scene() {


	gui = new GUI();
	gui->show();

	int u0 = 16;
	int v0 = 40;
	int h = 400;
	int w = 600;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("Framebuffer");
	fb->show();
	fb->SetBGR(0xFF0000FF);
	fb->redraw();
	gui->uiw->position(u0, v0 + fb->h + v0);

	float hfov = 55.0f;
	ppc = new PPC(hfov, fb->w, fb->h);
	demoPoint = V3(5.0f, 12.0f, -120.0f);
	demoAxisOrigin = V3(20.0f, 16.0f, -180.0f);
	demoAxisDirection = V3(1.0f, 1.0f, 1.0f).normalized();
	/*
	tmsN = 1;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1K.bin");
	V3 tv(20.0f, -20.0f, -200.0f);
	tms[0].Translate(tv);
	*/
}

void Scene::Render() {

	fb->SetBGR(0xFFFFFFFF);
	fb->ClearZB();
	fb->DrawAxis(demoAxisOrigin, demoAxisDirection, V3(1.0f, 0.0f, 0.0f), ppc);
	fb->Draw3DRoundPoint(demoPoint, V3(0.0f, 1.0f, 0.0f), 20, ppc);
	//
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderPoints(fb, psize, ppc);
	}
	fb->redraw();
}

void Scene::DBG() {

	int framesN = 360;
	int fi = 0;
	int fps = 30;
	std::chrono::duration<double> frame_length(1.0 / fps);
	auto last_frame = std::chrono::high_resolution_clock::now();
	while (fi < framesN) {
		auto now = std::chrono::high_resolution_clock::now();
		if (now - last_frame >= frame_length) {
			last_frame = now;
			fi++;
			demoPoint = demoPoint.rotatePoint(demoAxisOrigin, demoAxisDirection, 1.0f);
			//std::cout << "a: " << ppc->a << std::endl << "b: " << ppc->b << std::endl << "c: " << ppc->c << std::endl;
			//ppc->rotate(PPC::TILT, 1.0f);
			Render();
			Fl::check();
		}
	}
	return;
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}



