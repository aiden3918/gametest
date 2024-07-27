#pragma once

#ifndef entity_h
#define entity_h

#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "gameObject.h"
#include "util.h"

// W.I.P.

// most implementations should be in dummy, friendly, and enemy, in that order
const enum class EntityType { DUMMY, FRIENDLY, ENEMY };
const enum class AIType { STATIONARY, SENTRY, WALKER, DRONE };

const olc::Pixel defaultEntityColors[3] = { olc::WHITE, olc::GREEN, olc::RED };

class Entity : public GameObject {
public:
    // need this to work with pge
    Entity();
    Entity(std::string name, vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType, AIType aiType,
        float damage = 1.0f, bool affectedByGrav = true, bool tangible = true);
    ~Entity();

    void update(float& fElapsedTime);
    void draw(olc::PixelGameEngine* engine, vec2D& displayOffset);

    EntityType getType();
    AIType getAI();

    float hp = 1.0f;
    float dmg = 0.0f;

    //test (use sprites in future)
    olc::Pixel color;

    float attackCtr = 0.0f;
    float attackInterval = 1.0f; // attack cooldown interval (in sec)

    float projSpeed = 500.0f;
    vec2D moveSpeedVec;
    float moveSpeed;

    std::vector<std::pair<GameObject, float>> possibleColTiles;
    vec2D pcp; vec2D pcn; float pt;
    float jumpSpeed;

private:
    EntityType _type;
    AIType _ai;

    vec2D _displayPos;
    vec2D _displayCenter;

    vec2D _displayOffset;


};

#endif