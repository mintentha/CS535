#pragma once
#include "framebuffer.h"

class CubeMap {
	static const int TOP = 0;
	static const int LEFT = 1;
	static const int FRONT = 2;
	static const int RIGHT = 3;
	static const int DOWN = 4;
	static const int BACK = 5;

	FrameBuffer *sides[6];
	FrameBuffer *&top = sides[TOP];
	FrameBuffer *&left = sides[LEFT];
	FrameBuffer *&front = sides[FRONT];
	FrameBuffer *&right = sides[RIGHT];
	FrameBuffer *&down = sides[DOWN];
	FrameBuffer *&back = sides[BACK];

	PPC *ppcs[6];
	int w;
	int lastFaceFound = FRONT;

	static int getFace(V3 dir);

	public:
		CubeMap(FrameBuffer *cross);
		~CubeMap();
		int getW();
		unsigned int DirectionLookup(V3 dir);
};