#pragma once

#ifndef animation_h
#define animation_h

#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "util.h"

// probably will add on as we go
const enum class AnimationState { IDLE, RUN, JUMP, HURT, SHOOT, DEATH, WALK, RELOAD, PARRY, ACTION1, ACTION2, ACTION3, ACTION4 };

// probably want it on the gpu because its quicker
// MAKE SURE SPRITESHEET FOLLOWS ANIMATIONSTATE RULES AS FOLLOWS:
// rows should be in this order exactly:
// IDLE, RUN, JUMP, HURT, SHOOT, DEATH, WALK, RELOAD, PARRY, ACTION1, ACTION2, ACTION3, ACTION4
// if no animation for a specific action, leave that row blank
// (i.e. no run animation, leave row 2 blank)
// default direction is right (+x)
struct AnimationHandler {
	// std::unique_ptr<olc::Decal> spriteSheetDecal;
	std::unique_ptr<olc::Sprite> spriteSheet;
	std::unique_ptr<olc::Decal> spriteSheetDecal;

	// spritesheet size, hope this helps!
	vec2D spriteSheetSize;
	// size of the animation frame in the spritesheet
	vec2D partialSize;

	// keeps track of current anim type and frame
	AnimationState currentAnimState;
	int currentASInt;

	int currentFrame;
	int numFrames;
	int FPS = 60;
	float frameDuration;
	float timeSinceLastFrame = 0.0f;

	bool flip = 0;

	AnimationHandler() {}
	AnimationHandler(std::string& filename, AnimationState initAnim, 
		vec2D& partialSpriteSize, int initFrame = 1)
	{
		spriteSheet = std::make_unique<olc::Sprite>(filename);
		spriteSheetDecal = std::make_unique<olc::Decal>(spriteSheet.get());
		spriteSheetSize = get_png_image_dimensions(filename);
		std::cout << filename << " loaded: " << spriteSheetSize.x << " x " << spriteSheetSize.y << " px" << std::endl;

		partialSize = partialSpriteSize;
		currentAnimState = initAnim;
		currentASInt = static_cast<int>(currentAnimState);
		currentFrame = initFrame - 1;

		frameDuration = 1.0f / FPS;
	}
	~AnimationHandler() {}

	void update(olc::PixelGameEngine* engine, vec2D& pos, vec2D& size, vec2D& displayOffset, float& fElapsedTime, bool isTexture) {
		// update animation based on FPS
		timeSinceLastFrame += fElapsedTime;
		if (timeSinceLastFrame > frameDuration) {
			// loop it back 
			currentFrame++;
			if (currentFrame > numFrames) currentFrame = 0;

			timeSinceLastFrame = 0.0f;
		}

		drawAnimation(engine, pos, size, displayOffset, isTexture);
	}

	void setFPS(int fps) {
		FPS = fps;
		frameDuration = 1.0f / FPS;
	}

	void setAnimType(AnimationState animType, int frames, int initFrame = 1) {
		currentAnimState = animType;
		currentASInt = static_cast<int>(currentAnimState);
		numFrames = frames - 1; // because frames are indexed from 0 on the handler
		currentFrame = initFrame - 1;
	}

	void drawAnimation(olc::PixelGameEngine* engine, vec2D& pos, vec2D& size, vec2D& displayOffset,
		bool &isTexture) {
		// partial sprite x = spritesize.x * frame
		// partial sprite y = spritesize.y * animType
		// what is the center parameter for DrawPartialRotatedDecal()?

		//											readjust pos if flip  
		if (isTexture) {
			olc::vf2d vec2DPos = { pos.x, pos.y };
			olc::vf2d vec2DSize = { size.x, size.y };
			olc::vf2d vfDispOffset = { displayOffset.x, displayOffset.y };
			std::array<olc::vf2d, 4U> positions = { 
				// for conventional quad, start at top left, rotate ccw
				vec2DPos + vfDispOffset, 
				{vec2DPos.x + vfDispOffset.x, vec2DPos.y + vec2DSize.y + vfDispOffset.y },
				vec2DPos + vec2DSize + vfDispOffset,
				{vec2DPos.x + vfDispOffset.x + vec2DSize.x, vec2DPos.y + vfDispOffset.y } 
			};
			// if source size is greater than the sprite's size itself, then it fills in the rest
			// (bottom and right) with the pixel color of the rightmost column and lowest row
			// very cool
			engine->DrawPartialWarpedDecal(spriteSheetDecal.get(), positions,
				{ partialSize.x * currentFrame, partialSize.y * currentASInt },
				{ partialSize.x, partialSize.y });
		}
		else {
			engine->DrawPartialRotatedDecal({ pos.x + displayOffset.x + (partialSize.x * flip),
				pos.y + displayOffset.y }, spriteSheetDecal.get(), 0, { 0.0f, 0.0f },
				{ partialSize.x * currentFrame, partialSize.y * currentASInt },
				{ partialSize.x, partialSize.y }, { 1.0f + (-2.0f * (float)flip), 1.0f });
		}
	}

};

#endif