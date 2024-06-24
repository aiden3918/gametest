#include "../h/tile.h"

Tile::Tile() {}
Tile::Tile(vec2D initPos, vec2D size, vec2D initVel, vec2D initAccel, olc::Pixel initColor, bool affectedByGrav, bool tangible) {
	pos = initPos;
	vel = initVel;
	accel = initAccel;
	color = initColor;
	isTangible = tangible;

	_size = size;
	_affectedByGrav = affectedByGrav;
	if (affectedByGrav) accel.y = 500.0f;

	updateHitbox();
}
Tile::~Tile() {}

void Tile::update(olc::PixelGameEngine* engine, float elapsedTime) {
	updateHitbox();
	engine->FillRect({ (int)pos.x, (int)pos.y }, { (int)_size.x, (int)_size.y }, color);
}