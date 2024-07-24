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
//#include "../extern-lib/rapidjson/document.h"
//#include "../extern-lib/rapidjson/writer.h"
//#include "../extern-lib/rapidjson/stringbuffer.h"

const enum class Weapons { SWORD, GUN };

struct GeneralWeapon {
    std::string name;
    Weapons id;

    float dmg;
    float firerate; // in use/second
    float cooldownTime; // 1 / firerate

    // firerate is in uses/second
    GeneralWeapon() {}
    ~GeneralWeapon() {}

    // name, weaponID (enum), damage, firerate
    GeneralWeapon(std::string weaponName, Weapons weaponID, float weaponDmg, float weaponFirerate) {
        name = weaponName;
        id = weaponID;

        dmg = weaponDmg;
        firerate = weaponFirerate;
        cooldownTime = 1.0f / firerate;
    }
};

const GeneralWeapon weaponData[2] = {
    GeneralWeapon("Sword", Weapons::SWORD, 3.0f, 3.0f),
    GeneralWeapon("Gun", Weapons::GUN, 1.0f, 3.0f)
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

    // global "freeze" effect
    float freezeCtr = 0.0f;
    float freezeDuration = 0.5f;

private:

    void _updateTileCollisions(float& fElapsedTime, Environment* env, std::vector<std::pair<GameObject,
        float>>&possibleColTiles, vec2D& pct, vec2D& pcn, float& pt);

    inline void _updateJumpMechanics(olc::PixelGameEngine* engine, float fElapsedTime, vec2D pcn, float playerT);
    inline void _updateHorizontalMovement(olc::PixelGameEngine* engine);
    inline void _updateMouseInfo(olc::PixelGameEngine* engine, vec2D& mouse);
    inline void _updateWeapons(olc::PixelGameEngine* engine);
    inline void _updateParry(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime);
    inline void _updateMouseMechanics(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime);
    inline void _updatePlayerInfo(olc::PixelGameEngine* engine, vec2D& pcn, vec2D pcp, float& pT);
    inline void _updateEnemyCollisions(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime);
    inline void _updateProjCollisions(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime);
    inline void _handleAnimation(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D& pct, vec2D& pcn, float& pt);
    inline void _updatePlayerUI(olc::PixelGameEngine* engine, float& fElapsedTime);
    inline void _updateResetMechanics(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime);

    // "scroll effect" data
    vec2D _displayPos;
    vec2D _displayCenter;

    vec2D _displayOffset;

    // sprite data
    std::unique_ptr<olc::Sprite> _sprite;
    // std::unique_ptr<olc::Decal> _spriteDecal;
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
    float _parryBoxScale = 1.25f;
    float _parryCooldownCtr = 0.0f;
    float _parryCooldownDuration = 0.5f;

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
    AnimationHandler* animHandler;

    // weapon data
    GeneralWeapon _currentWeapon = weaponData[static_cast<int>(Weapons::SWORD)];
    float _weaponCDCtr = 0.0f;

};

#endif // !player_h
