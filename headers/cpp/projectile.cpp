#include "../h/projectile.h"

Projectile::Projectile() {}
Projectile::Projectile(vec2D initPos, float size, ProjShape shape, bool friendly, vec2D initVel, vec2D initAccel, olc::Pixel initColor, bool affectedByGrav, bool tangible, bool parriable) {
	pos = initPos;
	vel = initVel;
	accel = initAccel;
	isTangible = tangible;
	color = initColor;
	isFriendly = friendly;
	_affectedByGrav = affectedByGrav;
	_shape = shape;
	(_shape == CIRCLE) ? _radius = size : _length = size;

	if (affectedByGrav) accel.y = 500.0f;

}
Projectile::~Projectile() {}

void Projectile::update(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D& mouse) {

	vel.x += accel.x * fElapsedTime;
	vel.y += accel.y * fElapsedTime;

	// vec2D oldPos = pos;

	pos.x += vel.x * fElapsedTime;
	pos.y += vel.y * fElapsedTime;

	// trail effect for line
	if (_shape == CIRCLE) engine->FillCircle({ (int)pos.x, (int)pos.y }, _radius, color);
	else {
		// thicker bullet
		for (int i = -1; i < 2; i++) {
			engine->DrawLine({ (int)(pos.x - vel.x * fElapsedTime) + i, (int)(pos.y - vel.y * fElapsedTime) + i }, { (int)pos.x + i, (int)pos.y + i }, color);
		}
	}
		//else engine->DrawLine({ (int)(pos.x -= 10), (int)(pos.y -= 10) }, { (int)pos.x, (int)pos.y }, color);
}

// does nothing now because its not a rect
void Projectile::updateHitbox() { return; }
ProjShape Projectile::getShape() { return _shape; }

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