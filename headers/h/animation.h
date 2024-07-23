#pragma once

#ifndef animation_h
#define animation_h

#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "util.h"

// probably will add on as we go
const enum AnimationState { IDLE, RUN, JUMP, HURT, SHOOT, DEATH, WALK, RELOAD, PARRY, ACTION1, ACTION2, ACTION3, ACTION4 };

// probably want it on the gpu because its quicker
// MAKE SURE SPRITESHEET FOLLOWS ANIMATIONSTATE RULES AS FOLLOWS:
// rows should be in this order exactly:
// IDLE, RUN, JUMP, HURT, SHOOT, DEATH, WALK, RELOAD, PARRY, ACTION1, ACTION2, ACTION3, ACTION4
// if no animation for a specific action, leave that row blank
// (i.e. no run animation, leave row 2 blank)
// default direction is right (+x)
struct animationHandler {
	// std::unique_ptr<olc::Decal> spriteSheetDecal;
	std::unique_ptr<olc::Sprite> spriteSheet;
	std::unique_ptr<olc::Decal> spriteSheetDecal;

	// spritesheet size, hope this helps!
	vec2D spriteSheetSize;
	// size of the animation frame in the spritesheet
	vec2D partialSize;

	// keeps track of current anim type and frame
	AnimationState currentAnimState;
	int currentFrame;
	int numFrames;
	int FPS = 60;
	float frameDuration;
	float timeSinceLastFrame = 0.0f;
	
	bool flip = 0;

	// animations and how many frames they have
	// std::vector<std::pair<AnimationState, int>> animationsData;
	
	// data of the current animation (type and number of frames);
	// std::pair<AnimationState, int> currentAnimData;


	void init(std::string& filename, AnimationState initAnim, vec2D& partialSpriteSize, int initFrame = 0) {
		spriteSheet = std::make_unique<olc::Sprite>(filename);
		// spriteSheetDecal = std::make_unique<olc::Decal>(spriteSheet);
		spriteSheetSize = get_png_image_dimensions(filename);
		std::cout << filename << " loaded: " << spriteSheetSize.x << " x " << spriteSheetSize.y << " px" << std::endl;

		partialSize = partialSpriteSize;
		// currentAnimData = initAnim;
		currentAnimState = initAnim;
		currentFrame = initFrame;

		// spriteSheetDecal = std::make_unique<olc::Decal>(spriteSheet);

		frameDuration = 1.0f / FPS;
	}

	void update(olc::PixelGameEngine *engine, vec2D & screenPos, float& fElapsedTime) {
		// update animation based on FPS
		timeSinceLastFrame += fElapsedTime;
		if (timeSinceLastFrame > frameDuration) {
			// loop it back 
			currentFrame++;
			if (currentFrame > numFrames) currentFrame = 0;

			timeSinceLastFrame = 0.0f;
		}

		drawAnimation(engine, screenPos);
	}

	void setFPS(int fps) {
		FPS = fps;
		frameDuration = 1.0f / FPS;
	}

	void setAnimType(AnimationState animType, int frames, int initFrame = 0) {
		currentAnimState = animType;
		numFrames = frames - 1; // because frames are counted from 0
		currentFrame = initFrame;
	}

	void drawAnimation(olc::PixelGameEngine* engine, vec2D& screenPos) {
		// partial sprite x = spritesize.x * frame
		// partial sprite y = spritesize.y * animType
		// use sprite sheet because decal cannot flip
		engine->DrawPartialSprite((int)screenPos.x, (int)screenPos.y, spriteSheet.get(),
			(int)(partialSize.x * currentFrame), (int)(partialSize.y * currentAnimState),
			(int)partialSize.x, (int)partialSize.y, 1U, 0 + flip);
	}

};

#endif