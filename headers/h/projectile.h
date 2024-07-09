#pragma once

#ifndef projectile_h
#define projectile_h

#include "gameObject.h"
#include "../extern-lib/olcPixelGameEngine.h"
#include "util.h"
#include "collision.h"
// #include "environment.h"

const enum class ProjShape {LINE, CIRCLE};

class Projectile : public GameObject {
public:
	// need this to work with pge
	Projectile();
	Projectile(std::string name, vec2D initPos, float size, ProjShape shape, bool friendly, vec2D initVel = {0, 0},
		vec2D initAccel = {0, 0}, olc::Pixel initColor = olc::BLACK, bool affectedByGrav = false, bool tangible = true,
		 bool parriable = true);
	~Projectile();

	void update(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D& mouse, vec2D& displayOffset);
	void updateHitbox();
	ProjShape getShape();
	bool isParriable();
	// bool checkCollision(Environment* env, Tile& possibleTileCollided);

	olc::Pixel color;
	int bounces = 1;
	int pierce = 1;
	bool isFriendly;

	float dmg = 1.0f;

protected:
	bool _parriable;
	float _length;
	float _radius;
	ProjShape _shape;
};

#endif
