#include "CubeMap.h"

#include <iostream>

CubeMap::CubeMap(FrameBuffer *cross) {
	int hfov = 90.0f;
	w = cross->w / 3.0f;
	if (w != cross->h / 4.0f) {
		throw std::runtime_error("Invalid dimensions for cubemap");
	}
	for (int i = 0; i < 6; i++) {
		sides[i] = new FrameBuffer(0, 0, w, w);
		ppcs[i] = new PPC(hfov, w, w);
		//sides[i]->show();
	}
	// Would be convenient to just rotate them like this
	// but rotating them is not perfect
	// so instead let's manually set a, b, c
	
	//ppcs[TOP]->rotate(PPC::TILT, 90.0f);
	//ppcs[DOWN]->rotate(PPC::TILT, -90.0f);
	//ppcs[FRONT]; // leave as is
	//ppcs[BACK]->rotate(PPC::TILT, 180.0f);
	//ppcs[LEFT]->rotate(PPC::PAN, -90.0f);
	//ppcs[RIGHT]->rotate(PPC::PAN, 90.0f);
	//std::cout << "TOP:" << std::endl << ppcs[TOP]->a << std::endl << ppcs[TOP]->b << std::endl << ppcs[TOP]->c << std::endl;
	//std::cout << "LEFT:" << std::endl << ppcs[LEFT]->a << std::endl << ppcs[LEFT]->b << std::endl << ppcs[LEFT]->c << std::endl;
	//std::cout << "FRONT:" << std::endl << ppcs[FRONT]->a << std::endl << ppcs[FRONT]->b << std::endl << ppcs[FRONT]->c << std::endl;
	//std::cout << "RIGHT:" << std::endl << ppcs[RIGHT]->a << std::endl << ppcs[RIGHT]->b << std::endl << ppcs[RIGHT]->c << std::endl;
	//std::cout << "DOWN:" << std::endl << ppcs[DOWN]->a << std::endl << ppcs[DOWN]->b << std::endl << ppcs[DOWN]->c << std::endl;
	//std::cout << "BACK:" << std::endl << ppcs[BACK]->a << std::endl << ppcs[BACK]->b << std::endl << ppcs[BACK]->c << std::endl;
	// -(float) w / 2.0f, (float) h / 2.0f, -(float) w / 2.0f
	ppcs[TOP]->a = V3(1.0f, 0.0f, 0.0f);
	ppcs[TOP]->b = V3(0.0f, 0.0f, -1.0f);
	ppcs[TOP]->c = V3(- (float) w / 2.0f,  (float) w / 2.0f,  (float) w / 2.0f);

	ppcs[LEFT]->a = V3(0.0f, 0.0f, -1.0f);
	ppcs[LEFT]->b = V3(0.0f, -1.0f, 0.0f);
	ppcs[LEFT]->c = V3(- (float) w / 2.0f,  (float) w / 2.0f,  (float) w / 2.0f);

	ppcs[FRONT]->a = V3(1.0f, 0.0f, 0.0f);
	ppcs[FRONT]->b = V3(0.0f, -1.0f, 0.0f);
	ppcs[FRONT]->c = V3(- (float) w / 2.0f,  (float) w / 2.0f, - (float) w / 2.0f);

	ppcs[RIGHT]->a = V3(0.0f, 0.0f, 1.0f);
	ppcs[RIGHT]->b = V3(0.0f, -1.0f, 0.0f);
	ppcs[RIGHT]->c = V3( (float) w / 2.0f,  (float) w / 2.0f, - (float) w / 2.0f);

	ppcs[DOWN]->a = V3(1.0f, 0.0f, 0.0f);
	ppcs[DOWN]->b = V3(0.0f, 0.0f, 1.0f);
	ppcs[DOWN]->c = V3(- (float) w / 2.0f, - (float) w / 2.0f, - (float) w / 2.0f);

	ppcs[BACK]->a = V3(1.0f, 0.0f, 0.0f);
	ppcs[BACK]->b = V3(0.0f, 1.0f, 0.0f);
	ppcs[BACK]->c = V3(- (float) w / 2.0f, - (float) w / 2.0f,  (float) w / 2.0f);

	top->label("Top");
	left->label("Left");
	front->label("Front");
	right->label("Right");
	down->label("Down");
	back->label("Back");
	for (int u = 0; u < w; u++) {
		for (int v = 0; v < w; v++) {
			top->Set(u, v, cross->Get(u + w, v));
			left->Set(u, v, cross->Get(u, v + w));
			front->Set(u, v, cross->Get(u + w, v + w));
			right->Set(u, v, cross->Get(u + 2 * w, v + w));
			down->Set(u, v, cross->Get(u + w, v + 2 * w));
			back->Set(u, v, cross->Get(u + w, v + 3 * w));
		}
	}
}

CubeMap::~CubeMap() {
	for (int i = 0; i < 6; i++) {
		delete sides[i];
	}
	delete[] sides;
}

int CubeMap::getFace(V3 dir) {
	// actually only gets axis of face
	int max_axis = 0;
	int max_axis_size = fabsf(dir[0]);
	for (int i = 1; i < 3; i++) {
		if (fabsf(dir[i]) > max_axis_size) {
			max_axis = i;
			int max_axis_size = fabsf(dir[i]);
		}
	}
	switch (max_axis) {
		case 0:
			if (dir[0] < 0) {
				return CubeMap::LEFT;
			} else {
				return CubeMap::RIGHT;
			}
		case 1:
			if (dir[1] > 0) {
				return CubeMap::TOP;
			}
			else {
				return CubeMap::DOWN;
			}
		case 2:
			if (dir[2] < 0) {
				return CubeMap::FRONT;
			}
			else {
				return CubeMap::BACK;
			}
	}
}

unsigned int CubeMap::DirectionLookup(V3 dir) {

	// dir is equal to [a b c] [u \\ v \\ 1]

	// instructions say to start with face where previous lookup was found
	// but there may be a better way to determine which face to use
	// simply by looking at the dominant term in the direction
	//dir = dir.normalized();
	//std::cout << face << std::endl;
	// now project dir onto face
	V3 p(FLT_MAX, FLT_MAX, FLT_MAX);
	for (int i = 0; i < 6; i++) {
		int face = (lastFaceFound + i) % 6;
		if (ppcs[face]->Project(dir, p) && 0 < p[0] && p[0] < w && 0 < p[1] && p[1] < w) {
			lastFaceFound = face;
			break;
		}
	}
	if (p[0] == FLT_MAX) {
		// not found on any face, error
		return 0x00000000;
	}
	//std::cout << p << std::endl;
	/*if (p[1] < 0) {
		std::cout << dir << std::endl;
		std::cout << p << std::endl;
	}*/

	/*
	int face = getFace(dir);
	V3 p;
	ppcs[face]->Project(dir, p);
	V3 col = sides[face]->BilinearInterpolate(p[0], p[1], false);
	*/

	V3 col = sides[lastFaceFound]->BilinearInterpolate(p[0], p[1], false);
	return col.GetColor();
}

int CubeMap::getW() {
	return w;
}