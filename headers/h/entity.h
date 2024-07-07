#pragma once

#ifndef entity_h
#define entity_h

#include "iostream"
#include "../extern-lib/olcPixelGameEngine.h"
#include "gameObject.h"
#include "util.h"

// W.I.P.

// most implementations should be in dummy, friendly, and enemy, in that order
const enum EntityType { DUMMY, FRIENDLY, ENEMY };

//const enum DummyAI { DUMMY  };
//const enum FriendlyAI { STATIONARY, FOLLOWER };
//const enum EnemyAI { SENTRY, WALKER, DRONE };

const enum AIType { STATIONARY, FOLLOWER, SENTRY, WALKER, DRONE };


const olc::Pixel defaultEntityColors[3] = { olc::WHITE, olc::GREEN, olc::RED };


class Entity : public GameObject {
public:
    // need this to work with pge
    Entity();
    Entity(std::string name, vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType, AIType aiType,
        float damage = 1.0f, bool affectedByGrav = true, bool tangible = true);
    ~Entity();

    void update(olc::PixelGameEngine* engine, float fElapsedTime, vec2D& mouse, vec2D& displayOffset);
    EntityType getAIType();
    AIType getAI();
    
    template <typename T>
    T getAI();

    float hp = 1.0f; 
    float dmg = 0.0f;

    //test (use sprites in future)
    olc::Pixel color;

private:
    EntityType _type;
    AIType _ai;

    vec2D _displayPos;
    vec2D _displayCenter;

    vec2D _displayOffset;


};

#endif