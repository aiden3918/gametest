#include "../h/background.h"

Background::Background() {}
Background::~Background() {}

void Background::setBackground(std::string id, const std::string backRef, const std::string midRef, 
	const std::string frontRef, vec2D screenSize) {

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

	_screenSize = screenSize;

}

void Background::update(olc::PixelGameEngine* engine, vec2D& playerPos, vec2D& displayOffset) {
	if (_backSprite.get() == nullptr || _middleSprite.get() == nullptr ||
		_frontSprite.get() == nullptr) return;

	// calculate relative x positions of all og background
	_midX = -playerPos.x / 3.0f;
	_frontX = -playerPos.x / 1.5f;

	// get background data
	if (engine->GetKey(olc::B).bPressed) {
		std::cout << "original mid bg x pos: " << _midX<< std::endl;
		std::cout << "original front bg x pos: " << _frontX << std::endl;
	}

	// draw them
	engine->DrawDecal({ 0, 0 }, _backDecal.get());
	engine->DrawDecal({ _midX, 0 }, _middleDecal.get());
	engine->DrawDecal({ _frontX, 0 }, _frontDecal.get());

	// fill in any spaces based on player position
	// if x is greater than 0, draw to the left
	// if x is less than 0, draw to the right
	float currentMidX = _midX;
	float currentFrontX = _frontX;
	int limiter = 0; // since im using for loops

	while (currentMidX > 0.0f && limiter < 10) {
		currentMidX -= _screenSize.x;
		engine->DrawDecal({ currentMidX, 0 }, _middleDecal.get());
		limiter++;
	}
	limiter = 0;

	while (currentMidX < 0.0f && limiter < 10) {
		currentMidX += _screenSize.x;
		engine->DrawDecal({ currentMidX, 0 }, _middleDecal.get());
		limiter++;
	}
	limiter = 0;

	while (currentFrontX > 0.0f && limiter < 10) {
		currentFrontX -= _screenSize.x;
		engine->DrawDecal({ currentFrontX, 0 }, _frontDecal.get());
		limiter++;
	}
	limiter = 0;

	while (currentFrontX < 0.0f && limiter < 10) {
		currentFrontX += _screenSize.x;
		engine->DrawDecal({ currentFrontX, 0 }, _frontDecal.get());
		limiter++;
	}
	limiter = 0;

}
