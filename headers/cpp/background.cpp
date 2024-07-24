#include "../h/background.h"

/*
BackgroundSet::BackgroundSet() {}
//BackgroundSet::BackgroundSet(std::string bgID, std::string backRef, std::string midRef, std::string frontRef) {
//	id = bgID;
//	backFileRef = backRef;
//	middleFileRef = midRef;
//	frontFileRef = frontRef;
//	//
//	backSprite = std::make_unique<olc::Sprite>(backFileRef);
//	middleSprite = std::make_unique<olc::Sprite>(middleFileRef);
//	frontSprite = std::make_unique<olc::Sprite>(frontFileRef);
//	//
//	backDecal = std::make_unique<olc::Decal>(backSprite.get());
//	middleDecal = std::make_unique<olc::Decal>(middleSprite.get());
//	frontDecal = std::make_unique<olc::Decal>(frontSprite.get());
//}
BackgroundSet::~BackgroundSet() {}
*/

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

		bgSet.backDecal = std::make_unique<olc::Decal>(bgSet.backSprite.get());
		bgSet.middleDecal = std::make_unique<olc::Decal>(bgSet.middleSprite.get());
		bgSet.frontDecal = std::make_unique<olc::Decal>(bgSet.frontDecal.get());

		backgrounds.push_back(bgSet);
	}
}
*/
Background::~Background() {}

void Background::setBackground(std::string id, const std::string backRef, const std::string midRef, 
	const std::string frontRef) {

	_id = id;
	_backFileRef = backRef;
	_middleFileRef = midRef;
	_frontFileRef = frontRef;
	//
	_backSprite = std::make_unique<olc::Sprite>(_backFileRef);
	_middleSprite = std::make_unique<olc::Sprite>(_middleFileRef);
	_frontSprite = std::make_unique<olc::Sprite>(_frontFileRef);
	//
	_backDecal = std::make_unique<olc::Decal>(_backSprite.get());
	_middleDecal = std::make_unique<olc::Decal>(_middleSprite.get());
	_frontDecal = std::make_unique<olc::Decal>(_frontSprite.get());

}

void Background::update(olc::PixelGameEngine* engine, vec2D& playerPos) {
	if (_backSprite.get() == nullptr || _middleSprite.get() == nullptr ||
		_frontSprite.get() == nullptr) return;

	//engine->DrawSprite({ 0, 0 }, _backSprite.get(), 1U);
	//engine->DrawSprite({ 0, 0 }, _middleSprite.get(), 1U);
	//engine->DrawSprite({ 0, 0 }, _frontSprite.get(), 1U);

	engine->DrawDecal({ 0, 0 }, _backDecal.get());
	engine->DrawDecal({ 0, 0 }, _middleDecal.get());
	engine->DrawDecal({ 0, 0 }, _frontDecal.get());
}
