#pragma once

#ifndef gameObject_h
#define gameObject_h

#include "util.h"
#include "collision.h"
#include "animation.h"
#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"

class GameObject {
public:
	// need this to work with pge
	GameObject();
	GameObject(std::string name, vec2D initPos, vec2D size, vec2D initVel = { 0, 0 }, 
		vec2D initAccel = { 0, 0 }, bool affectedByGrav = false, bool tangible = true);
	~GameObject();

	void updateHitbox();
	AABB getHitbox();
	vec2D getSize();
	vec2D getCenter();

	void fillBasicRect(olc::PixelGameEngine* engine, vec2D &displayOffset);

	vec2D pos;
	vec2D vel;
	vec2D accel;

	std::string getName();

	bool isTangible;

	olc::Pixel color;

	std::string spriteSheetFilename;
	AnimationHandler* animHandler;

protected:
	std::string _name;

	bool _affectedByGrav;
	AABB _hitbox;
	vec2D _center;

	vec2D _size;
	bool _isColliding;

	float _internalCtr;
	 
	// probably deprecated
	/*collisionDirectionState _collDirState;
	axisCollisionState _axisColState;*/

};

// obj A is the main one, obj B is the other one
inline bool checkDynamicRectVsRectCollision(GameObject& objA, GameObject& objB, float& elapsedTime, vec2D& contactPoint, vec2D& contactNormal, float& contactTime) {
	// if stationary, assume not colliding (assumes no rects are initialy overlapping)
	if (objA.vel.x == 0 && objA.vel.y == 0) return false;

	// expand hitbox
	float xExpansion = (objA.getSize().x / 2.0f);
	float yExpansion = (objA.getSize().y / 2.0f);
	AABB objBHitbox = objB.getHitbox();
	AABB expandedB = { {objBHitbox.min.x - xExpansion, objBHitbox.min.y - yExpansion},
		{objBHitbox.max.x + xExpansion, objBHitbox.max.y + yExpansion} };

	vec2D modifiedDir = vec2DMult(objA.vel, elapsedTime);
	vec2D objACenter = objA.getCenter();
	if (checkRayCollision(objACenter, modifiedDir, expandedB, contactPoint, contactNormal, contactTime)) {
		if (contactTime >= 0.0f && contactTime <= 1.0f) return true;
	}

	return false;
}

// does not accurately check corners
// no it doesnt :(
inline bool checkDynamicCircleVsRectCollision(GameObject &circle, float &radius, GameObject& rect, float& elapsedTime, vec2D& contactPoint, vec2D& contactNormal, float& contactTime) {
	// if stationary, assume not colliding (assumes no rects are initialy overlapping)
	if (circle.vel.x == 0 && circle.vel.y == 0) return false;

	// expand hitbox
	float xExpansion = (radius / 2.0f);
	float yExpansion = (radius / 2.0f);
	AABB rectHitbox = rect.getHitbox();
	AABB expandedRect = { {rectHitbox.min.x - xExpansion, rectHitbox.min.y - yExpansion},
		{rectHitbox.max.x + xExpansion, rectHitbox.max.y + yExpansion} };

	vec2D modifiedVel = vec2DMult(circle.vel, elapsedTime);
	vec2D objACenter = circle.getCenter();
	if (checkRayCollision(objACenter, modifiedVel, expandedRect, contactPoint, contactNormal, contactTime)) {
		if (contactTime >= 0.0f && contactTime <= 1.0f) return true;
	}

	return false;
}

#endif // !gameObject_h
