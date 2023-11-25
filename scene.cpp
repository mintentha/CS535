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
	fb->label("first");
	fb->show();
	fb->SetBGR(0xFF0000FF);
	fb->redraw();
	gui->uiw->position(u0, v0 + fb->h + v0);

	float hfov = 55.0f;
	ppc = new PPC(hfov, fb->w, fb->h);
	tmsN = 5;
	tms = new TM[tmsN];
	tms[0].LoadBin("geometry/teapot1K.bin");
	tms[1].LoadBin("geometry/teapot1K.bin");
	tms[2].LoadBin("geometry/teapot1K.bin");
	tms[3].LoadBin("geometry/teapot1K.bin");
	tms[4].LoadBin("geometry/teapot1K.bin");
	//tms[0].LoadBin("geometry/teapot57K.bin");
	V3 tv(20.0f, -20.0f, -200.0f);
	for (int i = 0; i < tmsN; i++) {
		tms[i].Translate(tv);
		tms[i].Scale(0.5f);
	}
	tms[0].Translate(V3(-25.0f, 0.0f, 0.0f));
	tms[0].Rotate(tms[0].GetCenter(), V3(1.0f, 0.0f, 0.0f), 30);
	tms[1].Translate(V3(60.0f, 0.0f, -75.0f));
	tms[1].Rotate(tms[1].GetCenter(), V3(1.0f, 1.0f, 1.0f), 45);
	tms[1].Scale(0.5f);
	tms[2].Translate(V3(30.0f, -10.0f, -20.0f));
	tms[2].Rotate(tms[2].GetCenter(), V3(1.0f, 0.0f, 0.0f), -20);
	tms[2].Scale(0.25f);
	tms[3].Translate(V3(-60.0f, 10.0f, 20.0f));
	tms[3].Rotate(tms[3].GetCenter(), V3(-1.0f, 0.0f, 0.0f), 20);
	tms[3].Scale(0.25f);
	tms[4].Translate(V3(20.0f, 5.0f, 50.0f));
	tms[4].Scale(0.6f);
	tms[4].Rotate(tms[4].GetCenter(), V3(1.0f, 0.0f, 0.0f), 30);

	fb3 = new FrameBuffer(u0, v0, w, h);
	fb3->position(u0 + w + u0, v0);
	fb3->label("third");
	fb3->show();
	fb3->SetBGR(0xFFFFFFFF);
	fb3->redraw();
	ppc3 = new PPC(30.0f, fb3->w, fb3->h);
	ppc3->SetPose(ppc3->C + V3(100.0f, 200.0f, 200.0f), tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));


	L = ppc->C;
	ka = 0.5f;

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

	cfb->SetBGR(0xFFFFFFFF);
	cfb->ClearZB();
	int psize = 5;
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderFilled(cfb, cppc);
	}
	cfb->redraw();
}

void Scene::Render() {
	FilledRender(ppc, fb);
	FilledRender(ppc3, fb3);
	ppc->Visualize(20.0f, ppc3, fb3);
	//fb3->Draw3DPoint(L, V3(1.0f, 1.0f, 0.0f), 17, ppc3);
}

void Scene::DBG() {

	PPC ppc0(*ppc);
	PPC ppc1(*ppc);
	V3 tmc = tms[0].GetCenter();
	V3 LaP = tmc;
	V3 newC = LaP + V3(0.0f, 50.0f, 50.0f);
	ppc1.SetPose(newC, LaP, V3(0.0f, 1.0f, 0.0f));
	int framesN = 300;
	int fi = 0;
	int fps = 30;
	std::chrono::duration<double> frame_length(1.0 / fps);
	auto last_frame = std::chrono::high_resolution_clock::now();
	while (fi < framesN) {
		auto now = std::chrono::high_resolution_clock::now();
		if (now - last_frame >= frame_length) {
			// for first five second, rotate teapots, for next 5 seconds, move camera
			// 5 seconds = 5 * 30 frames = 150 frames
			last_frame = now;
			fi++;
			//demoPoint = demoPoint.rotatePoint(demoAxisOrigin, demoAxisDirection, 1.0f);
			//std::cout << "a: " << ppc->a << std::endl << "b: " << ppc->b << std::endl << "c: " << ppc->c << std::endl;
			//ppc->rotate(PPC::TILT, 1.0f);
			//tms[0].Scale(0.99f);
			if (fi <= framesN / 2) {
				// rotate teapots
				V3 a_o_0(20.0f, 16.0f, -180.0f);
				V3 a_d_0 = V3(1.0f, 1.0f, 1.0f).normalized();
				//fb->Draw3DSegment(a_o_0 - a_d_0 * 100, V3(1.0f, 0.0f, 0.0f), a_o_0 + a_d_0 * 100, V3(1.0f, 0.0f, 0.0f), ppc);
				tms[0].Rotate(a_o_0, a_d_0, 1.0f);
				V3 a_o_1(-10.0f, -5.0f, -150.0f);
				V3 a_d_1 = V3(-2.0f, 0.5f, 0.0f).normalized();
				tms[1].Rotate(a_o_1, a_d_1, -0.5f);
				V3 a_o_2(-10.0f, -5.0f, -150.0f);
				V3 a_d_2 = V3(-2.0f, 0.5f, 0.0f).normalized();
				tms[2].Rotate(a_o_2, a_d_2, -0.25f);
				V3 a_o_3 = tms[3].GetCenter();
				V3 a_d_3 = V3(1.0f, -0.25f, 0.0f).normalized();
				tms[3].Rotate(a_o_3, a_d_3, 1.5f);
				V3 a_o_4 = tms[4].GetCenter();
				V3 a_d_4 = V3(0.0f, 1.0f, 10.0f).normalized();
				tms[4].Rotate(a_o_4, a_d_4, -3.0f);
			}
			if (fi == framesN / 2) {
				tmc = tms[1].GetCenter();
				LaP = tmc;
				newC = LaP + V3(0.0f, 50.0f, 50.0f);
				ppc1.SetPose(newC, LaP, V3(0.0f, 1.0f, 0.0f));
			}
			if (fi >= framesN / 2)
				ppc->SetInterpolated(&ppc0, &ppc1, fi - framesN / 2, framesN / 2);
			Render();
			Fl::check();
		}
	}
	*ppc = ppc0;
	return;
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}



