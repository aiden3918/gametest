#pragma once

#ifndef util_h
#define util_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>

#define PI 3.14159f

// probably will add on as we go
enum AnimationState {IDLE, RUN, WALK, HURT, RELOAD, SHOOT, DEATH, PARRY, ACTION1, ACTION2, ACTION3, ACTION4};

struct vec2D {
	float x = 0.0f;
	float y = 0.0f;
};

struct AABB {
	vec2D min;
	vec2D max;
};

// treating a struct as a vector here feels wrong, but compiler says its ok sooooooo
inline vec2D vec2DAdd(vec2D& a, vec2D& b) { return { a.x + b.x, a.y + b.y }; }
inline vec2D vec2DSub(vec2D& a, vec2D& b) { return { a.x - b.x, a.y - b.y }; }
inline vec2D vec2DMult(vec2D& a, const float& val) { return { val * a.x, val * a.y }; }
inline vec2D vec2DDiv(vec2D& a, const float& val) { return { a.x / val, a.y / val }; }
inline vec2D vec2DElementwiseMult(vec2D& a, vec2D& b) { return { a.x * b.x, a.y * b.y }; }
inline vec2D vec2DElementwiseDiv(vec2D& a, vec2D& b) { return { a.x / b.x, a.y / b.y }; }
inline vec2D dotProduct2D(vec2D& a, vec2D& b) { return { a.x * b.x, a.y * b.y }; }
inline float vec2DMag(vec2D& vec) { return sqrt((vec.x * vec.x) + vec.y * vec.y); }
inline vec2D vec2DNormalise(vec2D& vec) { return vec2DDiv(vec, vec2DMag(vec)); }
inline float vec2DDotProduct(vec2D& vec1, vec2D& vec2) { return (vec1.x * vec2.x) + (vec1.y + vec2.y); }

// d = (v_i)(t) + 0.5(a)(t^2)
inline float getDisp1(float initVel, float accel, float time) { return (initVel * time) + (0.5 * accel * time * time); }
inline float getDisp2(float initVel, float finalVel, float time) { return (time * (initVel + finalVel) / 2); }
inline float getFinalVel1(float initVel, float accel, float time) { return initVel + (time * accel); }
inline float getFinalVel2(float initVel, float accel, float disp) { return sqrt((initVel * initVel) + (2 * accel * disp)); }
inline float lerp(float init, float end, float factor) { return init + (factor * (end - init)); }

// rads to deg and vice versa
inline float radToDeg(float& rad) { return rad * 180.0f / PI; }
inline float degToRad(float& deg) { return deg * PI / 180.0f; }

// distance
inline float getDist(vec2D& a, vec2D& b) { return sqrt(((b.y - a.y) * (b.y - a.y)) + ((b.x - a.x) * (b.x - a.x))); }

// get image size of png
// courtesy of Aliaksei Luferau
// https://stackoverflow.com/a/69105584
inline vec2D get_png_image_dimensions(std::string& file_path)
{
	vec2D size;
	unsigned char buf[8];

	std::ifstream in(file_path);
	in.seekg(16);
	in.read(reinterpret_cast<char*>(&buf), 8);

	size.x = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
	size.y = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + (buf[7] << 0);

	return size;
}

// ------------------------------------------------------------------------------------------------------
// maybe deprecated? (i havent even implemented it yet lol)
enum direction { LEFT, RIGHT, UP, DOWN, NONE };

struct collisionDirectionState {
	bool hasCollided = false; // idk if this is necessary yet
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;

	// std::vector<Tile> collidedTiles; // likely necessary to add logic for intangible tiles
};

struct axisCollisionState {
	bool hasCollided = false;
	bool upOrDown = false;
	bool leftOrRight = false;
};

#endif // !util_h