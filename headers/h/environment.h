#pragma once

#ifndef environment_h
#define environment_h

#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "util.h"
#include "tile.h"
#include "gameObject.h"
#include "collision.h"
#include "projectile.h"
#include <vector>

class Environment {
public:
	Environment();
	~Environment();

	bool gameObjCollidedWithEnv(GameObject& gameObject, GameObject& collidedTile);

	void drawTiles(olc::PixelGameEngine* pge, float fElapsedTime);
	void drawProjectiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D &mouse);

	void addTile(vec2D& initPos, vec2D& size, vec2D initVel = { 0, 0 }, vec2D initAccel = { 0, 0 }, olc::Pixel color = olc::BLACK, bool affectedByGrav = false, bool tangible = true);
	void addTile(Tile& newTile);

	void addProjectile(vec2D initPos, float size, ProjShape shape, bool friendly, vec2D initVel = { 0, 0 },
		vec2D initAccel = { 0, 0 }, olc::Pixel initColor = olc::BLACK, bool affectedByGrav = false,
		bool tangible = true, bool parriable = true);
	void addProjectile(Projectile& projectile);

	std::vector<Tile> getTangibleTiles();
	std::vector<Tile> getIntangibleTiles();

protected:
	std::vector<Tile> _tangibleTiles;
	std::vector<Tile> _intangibleTiles;
	std::vector<Projectile> _projectiles;

	void _eraseProj(int &index);
};

#endif
