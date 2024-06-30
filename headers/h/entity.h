#pragma once

#ifndef entity_h
#define entity_h

#include "iostream"
#include "../extern-lib/olcPixelGameEngine.h"
#include "gameObject.h"
#include "util.h"

const enum EntityType { DUMMY, FRIENDLY, ENEMY };
const olc::Pixel defaultEntityColors[3] = { olc::WHITE, olc::GREEN, olc::RED };

class Entity : public GameObject {
public:
    // need this to work with pge
    Entity();
    Entity(vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType, bool affectedByGrav = true, bool tangible = true);
    ~Entity();

    void update(olc::PixelGameEngine* engine, float fElapsedTime, vec2D& mouse, vec2D& displayOffset);

    float hp; 
    float dmg;

    //test (use sprites in future)
    olc::Pixel color;

private:
    EntityType _type;

    vec2D _displayPos;
    vec2D _displayCenter;

    vec2D _displayOffset;


};

#endif