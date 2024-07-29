#pragma once

#ifndef background_h
#define background_h

#include "util.h"
#include "../extern-lib/olcPixelGameEngine.h"

// will include parallax effect
// background WIDTHS do NOT have to be the same size as the SCREEN WIDTH, it will align the backdrops
// automatically :)
class Background {
public:
	Background();
	~Background();

	void setBackground(std::string id, const std::string backRef, const std::string midRef, 
		const std::string frontRef, vec2D screenSize);
	void update(olc::PixelGameEngine* engine, vec2D& playerPos, vec2D& displayOffset);

private:
	std::string _id;

	std::string _backFileRef;
	std::string _middleFileRef;
	std::string _frontFileRef;

	std::unique_ptr<olc::Sprite> _backSprite;
	std::unique_ptr<olc::Sprite> _middleSprite;
	std::unique_ptr<olc::Sprite> _frontSprite;

	std::unique_ptr<olc::Decal> _backDecal;
	std::unique_ptr<olc::Decal> _middleDecal;
	std::unique_ptr<olc::Decal> _frontDecal;

	vec2D _screenSize;
	vec2D _middleSpriteSize;
	vec2D _frontSpriteSize;

	float _midX;
	float _frontX;

};

#endif