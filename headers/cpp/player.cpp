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
    _updateParry(engine, fElapsedTime);

    // mouse updates
    _updateMouseMechanics(engine, env, fElapsedTime);

    // finally, display character
    // engine->DrawSprite({ (int)_displayPos.x, (int)_displayPos.y }, animHandler.spriteSheet.get());
    _handleAnimation(engine, fElapsedTime, playerContactPoint, playerContactNormal, playerT);

    // debug info
    _updatePlayerInfo(engine, playerContactNormal, playerContactPoint, playerT);

    _updatePlayerUI(engine, fElapsedTime);

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
            vel.x = (pcn.x * fElapsedTime * 50000.0f);
            _movementCtr += fElapsedTime;
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
        _currentWeapon = weaponData[VANTABLADE];
        std::cout << _currentWeapon.name << " equipped" << std::endl;
        std::cout << "cooldown time: " << _currentWeapon.cooldownTime << std::endl;

        //currentWeapon = VANTABLADE;
        //std::cout << "Vantablade equipped" << std::endl;
    }
    else if (engine->GetKey(olc::K2).bPressed) {
        _currentWeapon = weaponData[POCKETROCKET];
        std::cout << _currentWeapon.name << " equipped" << std::endl;
        std::cout << "cooldown time: " << _currentWeapon.cooldownTime << std::endl;

        //currentWeapon = POCKETROCKET;
        //std::cout << "Pocket Rocket equipped" << std::endl;
        // hehehehe
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
inline void Player::_updateParry(olc::PixelGameEngine* engine, float &fElapsedTime) {
    
    if (engine->GetKey(olc::F).bPressed && _parryCtr == 0.0f) {
        std::cout << "parry" << std::endl;
        _parryCtr += fElapsedTime;

        vec2D parryBoxSize = { _partialSpriteSize.x * 1.5f, _partialSpriteSize.x * 1.5f };
        vec2D lavScaled = vec2DMult(_lookAngleVector, _partialSpriteSize.x);
        // vec2D newParryPos = vec2DAdd(pos, lavScaled);
        vec2D oldParryPos = { _center.x - parryBoxSize.x / 2.0f, _center.y - parryBoxSize.y / 2.0f };
        vec2D newParryPos = vec2DAdd(oldParryPos, lavScaled);

        vec2D lavNormal = { _lookAngleVector.y, -_lookAngleVector.x };
        // newParryPos.x -= parryBoxSize.x / 2.0f; newParryPos.y -= parryBoxSize.y / 2.0f;
        _parryBox = new GameObject("playerParryBox", newParryPos, parryBoxSize, {0, 0}, {0, 0}, false, false);
        vec2D parryLinePos = vec2DAdd(_center, lavScaled);
        _parryLine = {
            {
                parryLinePos.x - std::min(_partialSpriteSize.x, lavNormal.x * _partialSpriteSize.x),
                parryLinePos.y - std::min(_partialSpriteSize.x, lavNormal.y * _partialSpriteSize.x) 
                    - (_partialSpriteSize.x / 2.0f)
            },
            {
                parryLinePos.x + std::min(_partialSpriteSize.x, lavNormal.x * _partialSpriteSize.x),
                parryLinePos.y + std::min(_partialSpriteSize.x, lavNormal.y * _partialSpriteSize.x) 
                    - (_partialSpriteSize.x / 2.0f)
            } 
        };

    }

    if (_parryCtr > 0.0f) {
        if (_parryCtr < _parryDuration) {
            vec2D lavScaled = vec2DMult(_lookAngleVector, _partialSpriteSize.x);
            // vec2D newParryPos = vec2DAdd(pos, lavScaled);
            vec2D oldParryPos = { _center.x - _parryBox->getSize().x / 2.0f, _center.y - _parryBox->getSize().y / 2.0f};
            vec2D newParryPos = vec2DAdd(oldParryPos, lavScaled);

            _parryCtr += fElapsedTime;

            _parryBox->pos = newParryPos;
            vec2D pbPos = _parryBox->pos;
            vec2D pbSize = _parryBox->getSize();
            engine->FillRect({ (int)(pbPos.x + _displayOffset.x), (int)(pbPos.y + _displayOffset.y) }, 
                { (int)pbSize.x, (int)pbSize.y }, olc::GREEN);

            vec2D lavNormal = { _lookAngleVector.y, -_lookAngleVector.x };
            vec2D parryLinePos = vec2DAdd(_center, lavScaled);
            _parryLine = {
                {
                    parryLinePos.x - std::min(_partialSpriteSize.x, lavNormal.x * _partialSpriteSize.x),
                    parryLinePos.y - std::min(_partialSpriteSize.x, lavNormal.y * _partialSpriteSize.x)
                        - (_partialSpriteSize.x / 2.0f)
                },
                {
                    parryLinePos.x + std::min(_partialSpriteSize.x, lavNormal.x * _partialSpriteSize.x),
                    parryLinePos.y + std::min(_partialSpriteSize.x, lavNormal.y * _partialSpriteSize.x)
                        - (_partialSpriteSize.x / 2.0f)
                }
            };

            pbPos.x += pbSize.x / 2.0f; pbPos.y += pbSize.y / 2.0f;
            engine->DrawLine(
                { (int)(_parryLine.start.x + _displayOffset.x), (int)(_parryLine.start.y + _displayOffset.y) },
                { (int)(_parryLine.stop.x + _displayOffset.x), (int)(_parryLine.stop.y + _displayOffset.y) },
                olc::BLUE
            );

        }
        else {
            delete _parryBox;
            _parryCtr = 0.0f;
        }
    }
}

inline void Player::_updateMouseMechanics(olc::PixelGameEngine* engine, Environment* env, float &fElapsedTime) {

    // bheld for automatic weapons, bpressed for semi-auto weapons (in-game)
    if (engine->GetMouse(0).bHeld && _weaponCDCtr == 0.0f) {
        std::cout << "attack" << std::endl;

        switch (_currentWeapon.id) {
            case VANTABLADE:
                break;
            case POCKETROCKET:

                vec2D projVel = vec2DMult(_lookAngleVector, 2000.0f);
                Projectile testProj = Projectile("playerProj", _center, 10, ProjShape::LINE, true, projVel, {0, 0}, olc::GREEN);
                testProj.bounces = 3;

                env->addProjectile(testProj);

                std::cout << projVel.x << " " << projVel.y << std::endl;
                break;
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
                // pierce does not decrease because we are not editing the actual projectile, just a copy
                p.pierce--;
                _iFramesCounter += 0.0001f;
            }
            break;
        }
        case ProjShape::CIRCLE: {
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
}

inline void Player::_updatePlayerUI(olc::PixelGameEngine* engine, float& fElapsedTime) {
    engine->DrawString({ 50, 50 }, "HP: " + std::to_string((int)hp), olc::GREEN, 2);
}

float Player::getLookAngleDeg() { return _lookAngleDeg; }
vec2D Player::getLookAngleVec() { return _lookAngleVector; }
vec2D Player::getDisplayOffset() { return _displayOffset; }