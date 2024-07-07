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
#include "entity.h"
#include <vector>

#include <sstream>
#include <fstream>

class Environment {
public:
	Environment();
	Environment(std::string &worldDataFile);
	~Environment();

	// bool gameObjCollidedWithEnv(GameObject& gameObject, GameObject& collidedTile);

	void drawTiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D& displayOffset);
	void drawProjectiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D &mouse, vec2D& displayOffset);
	void drawEntities(olc::PixelGameEngine* pge, float fElapsedTime, vec2D& mouse, vec2D& displayOffset);

	void addTile(std::string name, vec2D& initPos, vec2D& size, vec2D initVel = { 0, 0 }, vec2D initAccel = { 0, 0 }, olc::Pixel color = olc::BLACK, bool affectedByGrav = false, bool tangible = true);
	void addTile(Tile& newTile);

	void addProjectile(std::string name, vec2D initPos, float size, ProjShape shape, bool friendly, vec2D initVel = { 0, 0 },
		vec2D initAccel = { 0, 0 }, olc::Pixel initColor = olc::BLACK, bool affectedByGrav = false,
		bool tangible = true, bool parriable = true);
	void addProjectile(Projectile& projectile);

	void addEntity(Entity& entity);
	void addEntity(std::string name, vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType,
		AIType aiType, float damage = 1.0f, bool affectedByGrav = true, bool tangible = true);
	void handleEntityTileCollisions(float &fElapsedTime);
	void handleEntityProjCollisions(float& fElapsedTime);

	std::vector<Tile> getTangibleTiles();
	std::vector<Tile> getIntangibleTiles();
	std::vector<Entity> getEntities();

protected:
	std::vector<Tile> _tangibleTiles;
	std::vector<Tile> _intangibleTiles;
	std::vector<Projectile> _projectiles;
	std::vector<Entity> _entities;

	void _eraseProj(int &index);
	void _deleteEntity(int& index);
};

#endif
