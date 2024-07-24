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
