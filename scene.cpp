#include "scene.h"

#include "V3.h"
#include "M33.h"

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>

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
}


void Scene::DBG() {
	cerr << "INFO: pressed DBG" << endl;
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}


