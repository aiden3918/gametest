#include "../h/background.h"

BackgroundSet::BackgroundSet() {}
BackgroundSet::BackgroundSet(std::string bgID, std::string backRef, std::string midRef, std::string frontRef) {
	id = bgID;
	backFileRef = backRef;
	middleFileRef = midRef;
	frontFileRef = frontRef;
	backSprite = std::make_unique<olc::Sprite>(backFileRef);
	middleSprite = std::make_unique<olc::Sprite>(middleFileRef);
	frontSprite = std::make_unique<olc::Sprite>(frontFileRef);
	backDecal = std::make_unique<olc::Decal>(backSprite);
	middleDecal = std::make_unique<olc::Decal>(middleSprite);
	frontDecal = std::make_unique<olc::Decal>(frontDecal);
}
BackgroundSet::~BackgroundSet() {}

Background::Background() {}
/*
Background::Background(bgSpriteRefVec spriteRefs) {
	backgrounds = {};

	for (int i = 0; i < spriteRefs.size(); i++) {

		const std::string s[3] = { spriteRefs[i].first[0], spriteRefs[i].first[1], spriteRefs[i].first[2] };
		BackgroundSet bgSet;
		
		bgSet.id = spriteRefs[i].second;

		bgSet.backFileRef = s[0];
		bgSet.middleFileRef = s[1];
		bgSet.frontFileRef = s[2];

		bgSet.backSprite = std::make_unique<olc::Sprite>(s[0]);
		bgSet.middleSprite = std::make_unique<olc::Sprite>(s[1]);
		bgSet.frontSprite = std::make_unique<olc::Sprite>(s[2]);

		bgSet.backDecal = std::make_unique<olc::Decal>(bgSet.backSprite);
		bgSet.middleDecal = std::make_unique<olc::Decal>(bgSet.middleSprite);
		bgSet.frontDecal = std::make_unique<olc::Decal>(bgSet.frontDecal);

		backgrounds.push_back(bgSet);
	}
}
*/
Background::~Background() {}

void Background::update(olc::PixelGameEngine* engine, vec2D& playerPos) {
	engine->DrawDecal({ 0, 0 }, currentBackground.backDecal.get());
	engine->DrawDecal({ 0, 0 }, currentBackground.middleDecal.get());
	engine->DrawDecal({ 0, 0 }, currentBackground.frontDecal.get());
}

bool Background::setBackground(std::string id) {

	// linear search because i cant be bothered to sort the vector :)
	for (auto &b : backgrounds) {
		if (b.id == id) {
			currentBackground.copy(b); 
			return true;
		}
	}

	return false;
}

void Background::addBackground(BackgroundSet& bgSet) {
	backgrounds.push_back(bgSet);
}