#pragma once

#ifndef player_h
#define player_h

#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "environment.h"
#include "collision.h"
#include "util.h"
#include "gameObject.h"
#include "entity.h"
#include "animation.h"
#include <algorithm>
#include <fstream>
#include "../extern-lib/rapidjson/document.h"
#include "../extern-lib/rapidjson/writer.h"
#include "../extern-lib/rapidjson/stringbuffer.h"

const enum Weapons {VANTABLADE, POCKETROCKET};

struct GeneralWeapon {
    std::string name;
    Weapons id;

    float dmg;
    float firerate; // in use/second
    float cooldownTime; // 1 / firerate

    // firerate is in uses/second
    GeneralWeapon() {}
    ~GeneralWeapon() {}

    GeneralWeapon(std::string weaponName, Weapons weaponID, float weaponDmg, float weaponFirerate) {
        name = weaponName;
        id = weaponID;

        dmg = weaponDmg;
        firerate = weaponFirerate;
        cooldownTime = 1.0f / firerate;
    }
};

const GeneralWeapon weaponData[2] = {
    GeneralWeapon("Vantablade", VANTABLADE, 3.0f, 5.0f),
    GeneralWeapon("Pocket Rocket", POCKETROCKET, 1.0f, 3.0f)
};

class Player : public GameObject {
public:
    // need this to work with pge
    Player();
    Player(vec2D initPos, vec2D initVel, vec2D initAccel, std::string filename, vec2D screenSize, bool affectedByGrav = true, bool tangible = true);
    ~Player();
    // init(vec2D initPos, vec2D initVel, vec2D initAccel, std::string filename, bool affectedByGrav = true, bool canCollide = true);
    
    void update(olc::PixelGameEngine* engine, float fElapsedTime, Environment* env, vec2D& mouse);
    float getLookAngleDeg();
    vec2D getLookAngleVec();

    vec2D getDisplayOffset();

    float hp = 100.0f;
    float movespeed = 200.0f;
    float jumpspeed = 500.0f;

    // weapon data
    Weapons currentWeapon;
    float currentWeaponDmg;
    float currentWeaponInterval;
private:

    void _updateTileCollisions(float& fElapsedTime, Environment* env, std::vector<std::pair<GameObject, 
        float>>& possibleColTiles, vec2D& pct, vec2D& pcn, float& pt);

    inline void _updateJumpMechanics(olc::PixelGameEngine* engine, float fElapsedTime, vec2D pcn, float playerT);
    inline void _updateHorizontalMovement(olc::PixelGameEngine* engine);
    inline void _updateMouseInfo(olc::PixelGameEngine* engine, vec2D& mouse);
    inline void _updateWeapons(olc::PixelGameEngine* engine);
    inline void _updateParry(olc::PixelGameEngine* engine, float &fElapsedTime);
    inline void _updateMouseMechanics(olc::PixelGameEngine* engine, Environment* env, float &fElapsedTime);
    inline void _updatePlayerInfo(olc::PixelGameEngine* engine, vec2D& pcn, vec2D pcp, float& pT);
    inline void _updateEnemyCollisions(olc::PixelGameEngine* engine, Environment* env, float &fElapsedTime);
    inline void _handleAnimation(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D& pct, vec2D& pcn, float& pt);
    inline void _updatePlayerUI(olc::PixelGameEngine* engine, float& fElapsedTime);

    // "scroll effect" data
    vec2D _displayPos;
    vec2D _displayCenter;

    vec2D _displayOffset;

    // sprite data
    std::unique_ptr<olc::Sprite> _sprite;
    unsigned int _uiSize[2];
    float _secCtr; // ????

    // mouse data
    bool _allowingMouse = true;
    vec2D _lookAngleVector;
    float _lookAngleDeg;

    // parry ability data
    float _parryCtr = 0.0f;
    float _parryDuration = 0.2f;
    bool _parrying;
    GameObject* _parryBox;
    line _parryLine;

    // movement data
    float _movementCtr = 0.0f;
    float _movementDuration = 0.1f;
    bool _canMove = true;

    //iframes data
    float _iFramesCounter = 0.0f;
    float _iFramesInterval = 0.5f; // cannot get hit within 0.5s of each other
    bool _invincible = false;

    // animation data
    vec2D _partialSpriteSize = { 50.0f, 100.0f };
    animationHandler animHandler;
    
    // weapon data
    GeneralWeapon _currentWeapon;
    float _weaponCDCtr = 0.0f;

};

#endif // !player_h

/*
#include <iostream>
#include "../extern-lib/olcPixelGameEngine.h"
#include "definitions.h"
#include "environment.h"
#include "gameObject.h"
#include <algorithm>

class Player : public GameObject {
public:
    // need this to work with pge
    Player() {}

    Player(vec2D initPos, vec2D initVel, vec2D initAccel, std::string filename, bool affectedByGrav = true, bool canCollide = true) {
        pos = initPos;
        vel = initVel;
        accel = initAccel;

        _affectedByGrav = affectedByGrav;
        _canCollide = canCollide;

        if (affectedByGrav) accel.y = 1000.0f;
        _sprite = std::make_unique<olc::Sprite>(filename);

        _size = get_png_image_dimensions(filename);
        updateHitbox();
    }

    // maybe handle collisions in gameV2.cpp
    // pros: would be easier to work with/around
    // cons: might be more jumbled during expansion
    void update(olc::PixelGameEngine* engine, float fElapsedTime, Environment* env) {

        vel.x = 0.0f;
        vec2D playerContactPoint, playerContactNormal;
        float playerT;
        std::vector<std::pair<GameObject, float>> possibleCollidingTiles;

        if (engine->GetKey(olc::A).bHeld) vel.x = -200.0f;
        if (engine->GetKey(olc::D).bHeld) vel.x = 200.0f;
        // if (engine->GetKey(olc::S).bHeld) vel.y = 100.0f;

        vel.x += accel.x * fElapsedTime;
        vel.y += accel.y * fElapsedTime;

        updateCollisions(fElapsedTime, env, possibleCollidingTiles, playerContactPoint, playerContactNormal,
            playerT);

        if ((engine->GetKey(olc::W).bPressed || engine->GetKey(olc::SPACE).bPressed) &&
            playerContactNormal.y == -1 && _isColliding) vel.y = -500.0f;

        //pos.x += getDisp1(vel.x, accel.x, fElapsedTime);
        //pos.y += getDisp1(vel.y, accel.y, fElapsedTime);
        pos.x += vel.x * fElapsedTime;
        pos.y += vel.y * fElapsedTime;

        updateHitbox();
        engine->DrawSprite({ (int)pos.x, (int)pos.y }, _sprite.get());

        if (engine->GetKey(olc::I).bPressed) {
            std::cout << "pos: (" << pos.x << ", " << pos.y << ")" << std::endl;
            std::cout << "vel: (" << vel.x << ", " << vel.y << ")" << std::endl;
            std::cout << "accel: (" << accel.x << ", " << accel.y << ")" << std::endl;
            std::cout << "normal: (" << playerContactNormal.x << ", " << playerContactNormal.y << ")" << std::endl;
        }

    }
private:

    void updateCollisions(float& fElapsedTime, Environment* env, std::vector<std::pair<GameObject, float>> &possibleColTiles,
        vec2D &pct, vec2D &pcn, float &pt) {
        for (auto& i : env->getTiles()) {
            _isColliding = checkDynamicRectVsRectCollision(*this, i, fElapsedTime, pct, pcn, pt);
            if (_isColliding) {

                std::cout << "Player contact with test box" << std::endl;
                std::cout << pcn.x << " " << pcn.y << std::endl;

                possibleColTiles.push_back({ i, pt });
            }
        }

        // sort tiles by proximity by checking tNear;
        std::sort(possibleColTiles.begin(), possibleColTiles.end(),
            [](const std::pair<GameObject, float>& a, const std::pair<GameObject, float>& b) {
                return a.second < b.second;
            }
        );

        for (auto& k : possibleColTiles) {
            if (checkDynamicRectVsRectCollision(*this, k.first, fElapsedTime, pct, pcn, pt)) {
                vel.x += pcn.x * std::abs(vel.x) * (1 - pt);
                vel.y += pcn.y * std::abs(vel.y) * (1 - pt);
            }
        }
    }

    std::unique_ptr<olc::Sprite> _sprite;
    unsigned int _uiSize[2];

};
*/
