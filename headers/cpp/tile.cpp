#include "../h/tile.h"

Tile::Tile() {}
Tile::Tile(std::string name, vec2D initPos, vec2D size, vec2D initVel, vec2D initAccel, olc::Pixel initColor, bool affectedByGrav, bool tangible) {
	_name = name;

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

void Tile::update(float& fElapsedTime) {
	vel.x += accel.x * fElapsedTime;
	vel.y += accel.y * fElapsedTime;

	pos.x += vel.x * fElapsedTime;
	pos.y += vel.y * fElapsedTime;

	updateHitbox();
}

void Tile::draw(olc::PixelGameEngine* engine, vec2D displayOffset) {
	engine->FillRectDecal({ (pos.x + displayOffset.x), (pos.y + displayOffset.y) },
		{ _size.x, _size.y }, color);
}