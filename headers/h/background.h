#pragma once

#ifndef background_h
#define background_h

#include "util.h"
#include "../extern-lib/olcPixelGameEngine.h"

// will include parallax effect
// for best results, make the background the same resolution as the game (1280 * 720) px
class Background {
public:
	Background();
	// Background(bgSpriteRefVec spriteRefs);
	~Background();

	void setBackground(std::string id, const std::string backRef, const std::string midRef, 
		const std::string frontRef);
	void update(olc::PixelGameEngine* engine, vec2D& playerPos);

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
};

#endif