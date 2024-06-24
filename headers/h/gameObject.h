#pragma once

#ifndef gameObject_h
#define gameObject_h

#include "util.h"
#include "collision.h"
#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"

class GameObject {
public:
	// need this to work with pge
	GameObject();
	GameObject(vec2D initPos, vec2D size, vec2D initVel = { 0, 0 }, vec2D initAccel = { 0, 0 }, bool affectedByGrav = false, bool tangible = true);
	~GameObject();

	void updateHitbox();
	AABB getHitbox();
	vec2D getSize();
	vec2D getCenter();

	vec2D pos;
	vec2D vel;
	vec2D accel;

	bool isTangible;

	//void setDirColState(collisionDirectionState collDirState) { _collDirState = collDirState; }
	//void resetDirColState() { _collDirState = { false, false, false, false, false }; }
	//collisionDirectionState getCollisionState() { return _collDirState; }

protected:
	bool _affectedByGrav;
	AABB _hitbox;
	vec2D _center;

	vec2D _size;
	bool _isColliding;

	AnimationState _animState;

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
	collisionDirectionState dummy;

	vec2D modifiedDir = vec2DMult(objA.vel, elapsedTime);
	vec2D objACenter = objA.getCenter();
	if (checkRayCollision(objACenter, modifiedDir, expandedB, dummy, contactPoint, contactNormal, contactTime)) {
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
	collisionDirectionState dummy;

	vec2D modifiedVel = vec2DMult(circle.vel, elapsedTime);
	vec2D objACenter = circle.getCenter();
	if (checkRayCollision(objACenter, modifiedVel, expandedRect, dummy, contactPoint, contactNormal, contactTime)) {
		if (contactTime >= 0.0f && contactTime <= 1.0f) return true;
	}

	return false;
}

// deprecated, i think
// DEBUG: logic here incorrect
// there is a better thing in place
//collisionDirectionState getObjCollisionDirection(GameObject& mainObject, GameObject& sideObject) {
//
//	// all false by default
//	collisionDirectionState collDirOutput;
//
//	AABB a = mainObject.getHitbox();
//	a.min.x++; a.max.x--; a.min.y++; a.max.y--;
//	AABB b = sideObject.getHitbox();
//	b.min.x++; b.max.x--; b.min.y++; b.max.y--;
//
//	// just check if actually collided first
//	if (!checkAABBCollision(a, b)) return collDirOutput;
//	else collDirOutput.hasCollided = true;
//
//	// DEBUG: dont think this logic works
//	//collDirOutput.left = (float)a.min.x < (float)b.max.x;
//	//collDirOutput.right = (float)a.max.x > (float)b.min.x;
//	//collDirOutput.up = (float)a.min.y < (float)b.max.y;
//	//collDirOutput.down = (float)a.max.y > (float)b.min.y;
//
//	// DEBUG: doesnt work rn but something along these lines
//	// first comparison for both is whats causing issues
//	// pretty sure this is heuristic, but whateva
//	// make sure x axis is actually colliding first
//	if (!(a.max.x < b.min.x || a.min.x > b.max.x)) {
//		std::cout << mainObject.getCenter().x << ", " << sideObject.getCenter().x << std::endl;
//		(mainObject.getCenter().x < sideObject.getCenter().x) ? collDirOutput.right = true : collDirOutput.left = true;
//	}
//
//	// make sure y axis is actually colliding first
//	if (!(a.max.y < b.min.y || a.min.y > b.max.y)) {
//		std::cout << mainObject.getCenter().y << ", " << sideObject.getCenter().y << std::endl;
//		(mainObject.getCenter().y < sideObject.getCenter().y) ? collDirOutput.down = true : collDirOutput.up = true;
//	}
//
//	return collDirOutput;
//
//}

#endif // !gameObject_h
