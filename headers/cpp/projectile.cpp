#include "../h/projectile.h"

Projectile::Projectile() {}

// for linear/circle projectiles
Projectile::Projectile(std::string name, vec2D initPos, float size, ProjShape shape, bool friendly, vec2D initVel, vec2D initAccel, olc::Pixel initColor, bool affectedByGrav, bool tangible, bool parriable) {
	name = _name;

	pos = initPos;
	vel = initVel;
	accel = initAccel;
	isTangible = tangible;
	color = initColor;
	isFriendly = friendly;
	_affectedByGrav = affectedByGrav;
	_shape = shape;
	_parriable = parriable;
	(_shape == ProjShape::CIRCLE) ? _radius = size : _length = size;

	if (affectedByGrav) accel.y = 500.0f;

	updateHitbox();

}

// for rectangular projectiles (usually melee)
Projectile::Projectile(std::string name, vec2D initPos, vec2D size, bool friendly, vec2D initVel, vec2D initAccel, olc::Pixel initColor, bool affectedByGrav, bool tangible, bool parriable) {
	name = _name;

	pos = initPos;
	vel = initVel;
	accel = initAccel;
	isTangible = tangible;
	color = initColor;
	isFriendly = friendly;
	_affectedByGrav = affectedByGrav;
	_shape = ProjShape::RECT;
	_size = size;
	_parriable = parriable;

	if (affectedByGrav) accel.y = 500.0f;

	updateHitbox();

}
Projectile::~Projectile() {}

void Projectile::update(float& fElapsedTime) {

	// vec2D oldPos = pos;
	vel.x += accel.x * fElapsedTime;
	vel.y += accel.y * fElapsedTime;

	pos.x += vel.x * fElapsedTime;
	pos.y += vel.y * fElapsedTime;

	updateHitbox();

	lifespanCtr += fElapsedTime;
}

void Projectile::draw(olc::PixelGameEngine* engine, vec2D& displayOffset) {

	// trail effect for line
	switch (_shape) {
	case ProjShape::CIRCLE: engine->FillCircle({ (int)pos.x, (int)pos.y }, _radius, color); break;
	case ProjShape::LINE: {
		// thicker bullet
		for (int i = -1; i < 2; i++) {
			engine->DrawLineDecal({
				(pos.x - vel.x * 0.01f) + displayOffset.x + i,
				(pos.y - vel.y * 0.01f) + displayOffset.y + i }, {
				pos.x + displayOffset.x + i,
				pos.y + displayOffset.y + i },
				color);
		}
		break;
	}
	case ProjShape::RECT: {
		fillBasicRect(engine, displayOffset);
		break;
	}
	}
}

// hitbox is set based on shape
void Projectile::updateHitbox() {
	switch (_shape) {
	case ProjShape::LINE:
		_center = pos;
		_hitbox = { pos, pos };
		break;
	case ProjShape::CIRCLE:
		_center = pos;
		_hitbox = {
			{_center.x - _radius, _center.y - _radius},
			{_center.x + _radius, _center.y + _radius}
		};
		break;
	case ProjShape::RECT:
		_hitbox = { {pos.x, pos.y}, {pos.x + _size.x, pos.y + _size.y} };
		_center = { pos.x + (_size.x / 2.0f), pos.y + (_size.y / 2.0f) };
		break;
	}

}

ProjShape Projectile::getShape() { return _shape; }
bool Projectile::isParriable() { return _parriable; }

//bool Projectile::checkCollision(Environment* env, Tile& possibleTileCollided) {
//	for (auto& i : env->getTangibleTiles()) {
//		if (_shape == CIRCLE) {
//			// might have to create a dynamic circle rect collision lol
//			// same idea with dynamic rect vs rect collision, make a ray for circle
//
//			// actually, probably just have to reverse velocities on collision for bouncing
//			// dont eve have to do dynamic
//			if (checkCircleRectCollision())
//		}
//		else {
//			if (checkRayCollision())
//		}
//	}
//
//	return false;
//}