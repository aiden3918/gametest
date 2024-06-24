#include "../h/gameObject.h"

GameObject::GameObject() {}
GameObject::GameObject(vec2D initPos, vec2D size, vec2D initVel, vec2D initAccel, bool affectedByGrav, bool tangible) {
	pos = initPos;
	vel = initVel;
	accel = initAccel;
	isTangible = tangible;
	_affectedByGrav = affectedByGrav;
	_size = size;

	if (affectedByGrav) accel.y = 500.0f;

	updateHitbox();
}
GameObject::~GameObject() {}

void GameObject::updateHitbox() {
	_hitbox = { {pos.x, pos.y}, {pos.x + _size.x, pos.y + _size.y} };
	_center = { pos.x + (_size.x / 2.0f), pos.y + (_size.y / 2.0f) };
}

AABB GameObject::getHitbox() { return _hitbox; }
vec2D GameObject::getSize() { return _size; }
vec2D GameObject::getCenter() { return _center; }
