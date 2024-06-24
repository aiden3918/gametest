#pragma once

#ifndef tile_h
#define tile_h

#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "util.h"
#include "gameObject.h"
#include <vector>

class Tile : public GameObject {
public:
	Tile();
	Tile(vec2D initPos, vec2D size, vec2D initVel = { 0, 0 }, vec2D initAccel = { 0, 0 }, olc::Pixel initColor = olc::BLACK, bool affectedByGrav = false, bool tangible = true);
	~Tile();

	void update(olc::PixelGameEngine* engine, float elapsedTime);

public:
	olc::Pixel color;
};

#endif