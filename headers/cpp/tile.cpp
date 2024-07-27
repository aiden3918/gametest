#include "../h/tile.h"

Tile::Tile() {}
Tile::Tile(std::string name, vec2D initPos, vec2D size, vec2D initVel, vec2D initAccel, 
	std::string spriteSheet, olc::Pixel initColor, bool affectedByGrav, bool tangible) 
{
	_name = name;

	pos = initPos;
	vel = initVel;
	accel = initAccel;
	color = initColor;
	isTangible = tangible;
	spriteSheetFilename = spriteSheet;
	animHandler = new AnimationHandler(spriteSheet, AnimationState::IDLE, size);
	animHandler->setFPS(1);

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

void Tile::draw(olc::PixelGameEngine* engine, vec2D displayOffset, float& fElapsedTime) {
	if (spriteSheetFilename == "NULL") {
		std::cout << "tiles are null" << std::endl;
		engine->FillRectDecal({ (pos.x + displayOffset.x), (pos.y + displayOffset.y) },
			{ _size.x, _size.y }, color);
	} else {
		std::cout << "spritesheet is not NULL" << std::endl;
		animHandler->update(engine, pos, _size, displayOffset, fElapsedTime, true);
	}

}