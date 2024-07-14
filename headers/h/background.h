#pragma once

#ifndef background_h
#define background_h

#include "util.h"
#include "../extern-lib/olcPixelGameEngine.h"

//typedef std::vector<std::pair<std::string[3], std::string>> bgSpriteRefVec;
//struct BGSpriteRefVec {
//	std::string refs[3];
//	std::string id;
//};

// will include parallax effect
// for best results, make the background the same resolution as the game (1280 * 720) px
struct BackgroundSet {
	
	BackgroundSet();
	BackgroundSet(std::string bgID, std::string backRef, std::string midRef, std::string frontRef);
	~BackgroundSet();

	std::string id;

	std::string backFileRef;
	std::string middleFileRef;
	std::string frontFileRef;

	std::unique_ptr<olc::Sprite> backSprite;
	std::unique_ptr<olc::Sprite> middleSprite;
	std::unique_ptr<olc::Sprite> frontSprite;

	std::unique_ptr<olc::Decal> backDecal;
	std::unique_ptr<olc::Decal> middleDecal;
	std::unique_ptr<olc::Decal> frontDecal;

	// have to do this because copy assignment operator for this struct 
	// is deleted due to unique_ptr
	void copy(BackgroundSet &bgSetToCopy) {
		backFileRef = bgSetToCopy.backFileRef;
		middleFileRef = bgSetToCopy.middleFileRef;
		frontFileRef = bgSetToCopy.frontFileRef;

		backSprite = std::make_unique<olc::Sprite>(backFileRef);
		middleSprite = std::make_unique<olc::Sprite>(middleFileRef);
		frontSprite = std::make_unique<olc::Sprite>(frontFileRef);

		backDecal = std::make_unique<olc::Decal>(backSprite);
		middleDecal = std::make_unique<olc::Decal>(middleSprite);
		frontDecal = std::make_unique<olc::Decal>(frontSprite);

	}

	//BackgroundSet operator=(BackgroundSet bgSetToCopy) {
	//	BackgroundSet returnBgSet;
	//	returnBgSet.backFileRef = bgSetToCopy.backFileRef;
	//	returnBgSet.middleFileRef = bgSetToCopy.middleFileRef;
	//	returnBgSet.frontFileRef = bgSetToCopy.frontFileRef;
	//	return returnBgSet;
	//}

};


class Background {
public:
	Background();
	// Background(bgSpriteRefVec spriteRefs);
	~Background();

	void update(olc::PixelGameEngine* engine, vec2D& playerPos);
	bool setBackground(std::string id);
	void addBackground(BackgroundSet &bgSet);

	std::vector<BackgroundSet> backgrounds;
	BackgroundSet currentBackground;
	// idk what to put here bruh
};

#endif