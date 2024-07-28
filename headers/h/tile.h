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

	// when creating a sprite for the tile, make sure the sprite is the same size as the specified size
	Tile(std::string name, vec2D initPos, vec2D size, vec2D initVel = { 0, 0 }, 
		vec2D initAccel = { 0, 0 }, const std::string spriteSheet = "NULL",
		olc::Pixel initColor = olc::BLACK, bool affectedByGrav = false, bool tangible = true);
	~Tile();

	void update(float& fElapsedTime);
	void draw(olc::PixelGameEngine* engine, vec2D displayOffset, float& fElapsedTime);

public:
	olc::Pixel color;
};

#endif