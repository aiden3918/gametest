#include "../h/player.h"

Player::Player() {}
Player::Player(vec2D initPos, vec2D initVel, vec2D initAccel, std::string filename, vec2D screenSize, 
    bool affectedByGrav, bool tangible) 
{
    pos = initPos;
    vel = initVel;
    accel = initAccel;

    _affectedByGrav = affectedByGrav;
    isTangible = tangible;

    if (affectedByGrav) accel.y = 1000.0f;

    vec2D spriteSheetSize = get_png_image_dimensions(filename);
    animHandler.init(filename, IDLE, _partialSpriteSize);
    animHandler.setFPS(15);

    //_sprite = std::make_unique<olc::Sprite>(filename);
    //_size = get_png_image_dimensions(filename);
    _size = _partialSpriteSize;

    // _canMove = true;

    _displayPos = { (screenSize.x - _size.x) / 2.0f, 1.33f * (screenSize.y - _size.y) / 2.0f };

    updateHitbox(); 
}
Player::~Player() {}

void Player::update(olc::PixelGameEngine* engine, float fElapsedTime, Environment* env, vec2D& mouse) {

    // movement and collisions

    vec2D playerContactPoint, playerContactNormal;
    float playerT;
    std::vector<std::pair<GameObject, float>> possibleCollidingTiles;

    _updateHorizontalMovement(engine);

    vel.x += accel.x * fElapsedTime;
    vel.y += accel.y * fElapsedTime;

    _updateTileCollisions(fElapsedTime, env, possibleCollidingTiles, playerContactPoint, playerContactNormal,
        playerT);

    if (_iFramesCounter == 0.0f) {
        _updateEnemyCollisions(engine, env, fElapsedTime);
   
        _updateProjCollisions(engine, env, fElapsedTime);
    }
    else (_iFramesCounter > _iFramesInterval) ? _iFramesCounter = 0.0f : _iFramesCounter += fElapsedTime;

    _updateJumpMechanics(engine, fElapsedTime, playerContactNormal, playerT);

    // technically an approximation, but is ok 
    pos.x += vel.x * fElapsedTime;
    pos.y += vel.y * fElapsedTime;
    updateHitbox();

    // calculate display positions for scrolling effect
    // _displayPos = { (engine->ScreenWidth() - _size.x) / 2.0f, 1.5f * (engine->ScreenHeight() - _size.y) / 2.0f };
    _displayCenter = { _displayPos.x + (_size.x / 2.0f), _displayPos.y + (_size.y / 2.0f) };
    _displayOffset = vec2DSub(_displayPos, pos);

    // mouse info
    _updateMouseInfo(engine, mouse);

    // weapon selection
    _updateWeapons(engine);

    // ability updates
    if (_parryCooldownCtr == 0.0f) _updateParry(engine, env, fElapsedTime);
    else {
        (_parryCooldownCtr < _parryCooldownDuration) ? _parryCooldownCtr += fElapsedTime : _parryCooldownCtr = 0.0f;
    }


    // mouse updates
    _updateMouseMechanics(engine, env, fElapsedTime);

    // finally, display character
    // engine->DrawSprite({ (int)_displayPos.x, (int)_displayPos.y }, animHandler.spriteSheet.get());
    _handleAnimation(engine, fElapsedTime, playerContactPoint, playerContactNormal, playerT);

    // debug info
    _updatePlayerInfo(engine, playerContactNormal, playerContactPoint, playerT);

    _updatePlayerUI(engine, fElapsedTime);

    _updateResetMechanics(engine, env, fElapsedTime);

}

void Player::_updateTileCollisions(float& fElapsedTime, Environment* env,
    std::vector<std::pair<GameObject, float>>& possibleColTiles, vec2D& pct, vec2D& pcn, float& pt) 
{
    for (auto& i : env->getTangibleTiles()) {
        _isColliding = checkDynamicRectVsRectCollision(*this, i, fElapsedTime, pct, pcn, pt);
        if (_isColliding) {

            //std::cout << "Player contact with test box" << std::endl;
            //std::cout << pcn.x << " " << pcn.y << std::endl;

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

inline void Player::_updateJumpMechanics(olc::PixelGameEngine* engine, float fElapsedTime, vec2D pcn, float playerT) {
    // walljump mechanics
    if (_movementCtr > 0.0f) {
        if (_movementCtr < _movementDuration) _movementCtr += fElapsedTime;
        else _movementCtr = 0.0f;
    }

    if (_canMove && (engine->GetKey(olc::W).bPressed || engine->GetKey(olc::SPACE).bPressed) &&
        pcn.y != 1 && playerT == 0.0f && _movementCtr == 0.0f)
    {
        std::cout << "pcn: " << pcn.x << ", " << pcn.y << std::endl;
        vel.y = -jumpspeed;
        // walljump
        if (pcn.x != 0.0f) {
            vel.x = (pcn.x * 400.0f);
            _movementCtr += 0.0001f;
        }
    }
}

inline void Player::_updateHorizontalMovement(olc::PixelGameEngine* engine) {
    if (_movementCtr == 0.0f) {
        vel.x = 0.0f;
        if (engine->GetKey(olc::A).bHeld) vel.x = -movespeed;
        if (engine->GetKey(olc::D).bHeld) vel.x = movespeed;
    }
    // if (engine->GetKey(olc::S).bHeld) vel.y = 100.0f;
}

inline void Player::_updateWeapons(olc::PixelGameEngine* engine) {
    // any weapon is currently on cooldown, do not allow switch
    if (_weaponCDCtr != 0.0f) return;

    if (engine->GetKey(olc::K1).bPressed) {
        int swordInt = static_cast<int>(Weapons::SWORD);
        _currentWeapon = weaponData[swordInt];
        std::cout << _currentWeapon.name << " equipped" << std::endl;
        std::cout << "cooldown time: " << _currentWeapon.cooldownTime << std::endl;
    }
    else if (engine->GetKey(olc::K2).bPressed) {
        int gunInt = static_cast<int>(Weapons::GUN);
        _currentWeapon = weaponData[gunInt];
        std::cout << _currentWeapon.name << " equipped" << std::endl;
        std::cout << "cooldown time: " << _currentWeapon.cooldownTime << std::endl;
    }
}

inline void Player::_updateMouseInfo(olc::PixelGameEngine* engine, vec2D& mouse) {
    engine->DrawLine({ (int)_displayCenter.x, (int)_displayCenter.y }, { (int)mouse.x, (int)mouse.y }, olc::YELLOW);
    vec2D mouseDist = { mouse.x - _displayCenter.x, mouse.y - _displayCenter.y };
    _lookAngleVector = vec2DNormalise(mouseDist);
    engine->DrawString({ 50, 630 }, "look vector: (" + std::to_string(_lookAngleVector.x) + " " + std::to_string(_lookAngleVector.y) + ")");
    //_lookAngleDeg = atan2f(mouseDist.y, mouseDist.x);
    //_lookAngleDeg = radToDeg(_lookAngleDeg);
    // engine->DrawString({ 50, 660 }, "look angle: " + std::to_string(_lookAngleDeg) + " deg");
}


// probably should organize these; so messy
inline void Player::_updateParry(olc::PixelGameEngine* engine, Environment* env, float &fElapsedTime) {
    
    if (engine->GetKey(olc::F).bPressed && _parryCtr == 0.0f) {

        vec2D parryHBSize = vec2DMult(_partialSpriteSize, _parryBoxScale);
        _parryBox = new GameObject("parryBox", pos, parryHBSize);
        _parryBox->color = olc::GREEN;

        _parryCtr += 0.0001f;

    }

    if (_parryCtr > 0.0f) {
        if (_parryCtr < _parryDuration) {

            _parryBox->pos = pos;
            _parryBox->pos.y -= _partialSpriteSize.y * (_parryBoxScale - 1.0f);
            (_lookAngleVector.x < 0.0f) ? _parryBox->pos.x -=
                (_partialSpriteSize.x * (_parryBoxScale - 1.0f) + 10.0f) : _parryBox->pos.x += 10.0f;
            _parryBox->updateHitbox();

            _parryBox->fillBasicRect(engine, _displayOffset);
            _parryCtr += fElapsedTime;

            // memory leak, perhaps?
            std::vector<Projectile>* envProjsPtr = env->getActualProjectilesVec();
            AABB parryBoxHB = _parryBox->getHitbox();

            for (auto& p : *envProjsPtr) {

                AABB projHB = p.getHitbox();

                if (!p.isParriable()) continue;
                if (p.getShape() == ProjShape::LINE && !checkPtCollision(p.pos, parryBoxHB)) continue;
                // once again, only heuristic
                if (p.getShape() == ProjShape::CIRCLE && !checkAABBCollision(projHB, parryBoxHB)) continue;

                // redirect projectile
                float projSpeed = vec2DMag(p.vel);
                p.vel = vec2DMult(_lookAngleVector, projSpeed * 2.0f);
                p.isFriendly = true;

                // extra incentive to parry
                p.dmg *= 2.0f;

                // on successful parry, there is no cooldown
                _parryCtr = 0.0f;
            }

        }
        else {
            delete _parryBox;
            _parryCtr = 0.0f;
            _parryCooldownCtr += 0.0001f;
        }
    }
}

inline void Player::_updateMouseMechanics(olc::PixelGameEngine* engine, Environment* env, float &fElapsedTime) {

    // bheld for automatic weapons, bpressed for semi-auto weapons (in-game)
    if (engine->GetMouse(0).bHeld && _weaponCDCtr == 0.0f) {
        std::cout << "attack" << std::endl;

        switch (_currentWeapon.id) {
            case Weapons::SWORD: {
                vec2D meleeSize = { _partialSpriteSize.y, _partialSpriteSize.y };
                vec2D meleePosOffset = vec2DDiv(meleeSize, 2.0f);
                vec2D meleePos = vec2DSub(_center, meleePosOffset);
                vec2D meleeVel = vec2DMult(_lookAngleVector, 400.0f);
                vec2D meleeAccel = vec2DMult(meleeVel, -2.0f);
                Projectile meleeProj = Projectile("meleeProj", meleePos, meleeSize, true, meleeVel, 
                    meleeAccel, olc::DARK_YELLOW, false, true, false);
                meleeProj.lifespan = 0.5f;
                meleeProj.pierce = 3;
                meleeProj.dmg = _currentWeapon.dmg;

                env->addProjectile(meleeProj);
                break;
            }

            case Weapons::GUN: {
                vec2D projVel = vec2DMult(_lookAngleVector, 2000.0f);
                Projectile gunProj = Projectile("gunProj", _center, 10, ProjShape::LINE, true, projVel,
                    { 0, 0 }, olc::GREEN);
                gunProj.bounces = 3;
                gunProj.pierce = 2;

                env->addProjectile(gunProj);

                std::cout << projVel.x << " " << projVel.y << std::endl;
                break;
            }
        }

        _weaponCDCtr += 0.001f;
    }

    if (_weaponCDCtr > 0.0f) {
        (_weaponCDCtr > _currentWeapon.cooldownTime) ? _weaponCDCtr = 0.0f : _weaponCDCtr += fElapsedTime;
    }

}

inline void Player::_updatePlayerInfo(olc::PixelGameEngine* engine, vec2D& pcn, vec2D pcp, float& pT) {
    if (engine->GetKey(olc::I).bPressed) {
        std::cout << "pos: (" << pos.x << ", " << pos.y << ")" << std::endl;
        std::cout << "vel: (" << vel.x << ", " << vel.y << ")" << std::endl;
        std::cout << "accel: (" << accel.x << ", " << accel.y << ")" << std::endl;
        std::cout << "normal: (" << pcn.x << ", " << pcn.y << ")" << std::endl;
        std::cout << "contact pt: (" << pcp.x << ", " << pcp.y << ")" << std::endl;
        std::cout << "pct: " << pT << std::endl;
    }
}

inline void Player::_updateEnemyCollisions(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime) {
    vec2D pct; vec2D pcn; float pt;
    
    for (auto& e : env->getEntities()) {
        if (checkDynamicRectVsRectCollision(*this, e, fElapsedTime, pct, pcn, pt) && e.getType() == EntityType::ENEMY) {
            std::cout << "collided with enemy" << std::endl;
            hp -= e.dmg;
            _iFramesCounter += 0.0001f;
        }
    }
}

inline void Player::_updateProjCollisions(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime) {
    std::vector<Projectile>* envProjectilesPtr = env->getActualProjectilesVec();

    for (auto &p : *envProjectilesPtr) {
        if (p.isFriendly) continue;

        switch (p.getShape()) {
            case ProjShape::LINE: {
                if (checkPtCollision(p.pos, _hitbox)) {
                    hp -= p.dmg;
                    p.pierce--;
                    _iFramesCounter += 0.0001f;
                }
                break;
            }
            case ProjShape::CIRCLE: {
                AABB projHB = p.getHitbox();
                // does not account for corners (tbd)
                if (checkAABBCollision(projHB, _hitbox)) {
                    hp -= p.dmg;
                    p.pierce--;
                    _iFramesCounter += 0.0001f;
                }
                break;
            }
        }
    }
}

inline void Player::_handleAnimation(olc::PixelGameEngine* engine, float& fElapsedTime, 
    vec2D &pct, vec2D &pcn, float &pt) 
{
    if (vel.x == 0) {
        if (animHandler.currentAnimState != IDLE) animHandler.setAnimType(IDLE, 1);
    }
    else if (vel.x < 0) {
        if (animHandler.currentAnimState != RUN) animHandler.setAnimType(RUN, 4);
        animHandler.flip = 1;
    }
    else {
        if (animHandler.currentAnimState != RUN) animHandler.setAnimType(RUN, 4);
        animHandler.flip = 0;
    }
    if (!_isColliding && (pt > 1.0f || pt < 0.0f)) {
        if (animHandler.currentAnimState != JUMP) animHandler.setAnimType(JUMP, 1);
    }
    animHandler.update(engine, _displayPos, fElapsedTime);

    vec2D displayPos = vec2DAdd(pos, _displayOffset);
    engine->DrawRect({(int)displayPos.x, (int)displayPos.y}, {(int)_partialSpriteSize.x, (int)_partialSpriteSize.y}, olc::WHITE);
}

inline void Player::_updatePlayerUI(olc::PixelGameEngine* engine, float& fElapsedTime) {
    engine->DrawString({ 50, 50 }, "HP: " + std::to_string((int)hp), olc::GREEN, 2);
    engine->DrawString({ 50, 100 }, "Current Weapon: " + _currentWeapon.name, olc::GREEN, 2);
}

inline void Player::_updateResetMechanics(olc::PixelGameEngine* engine, Environment* env, float& fElapsedTime) {
    if (engine->GetKey(olc::R).bPressed || pos.y > 2000.0f || hp < 0.0f) {
        if (hp < 0.0f) hp = 100.0f;
        pos = env->getSpawnPoint();
    }
}

float Player::getLookAngleDeg() { return _lookAngleDeg; }
vec2D Player::getLookAngleVec() { return _lookAngleVector; }
vec2D Player::getDisplayOffset() { return _displayOffset; }