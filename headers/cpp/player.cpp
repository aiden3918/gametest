#include "../h/player.h"

Player::Player() {}
Player::Player(vec2D initPos, vec2D initVel, vec2D initAccel, std::string filename, bool affectedByGrav,
    bool tangible) 
{
    pos = initPos;
    vel = initVel;
    accel = initAccel;

    _affectedByGrav = affectedByGrav;
    isTangible = tangible;

    if (affectedByGrav) accel.y = 1000.0f;
    _sprite = std::make_unique<olc::Sprite>(filename);

    _size = get_png_image_dimensions(filename);

    _allowingUserInput = true;
    updateHitbox(); 
}
Player::~Player() {}

void Player::update(olc::PixelGameEngine* engine, float fElapsedTime, Environment* env, vec2D& mouse) {

    // movement and collisions
    vel.x = 0.0f;
    vec2D playerContactPoint, playerContactNormal;
    float playerT;
    std::vector<std::pair<GameObject, float>> possibleCollidingTiles;

    if (_allowingUserInput) {
        if (engine->GetKey(olc::A).bHeld) vel.x = -200.0f;
        if (engine->GetKey(olc::D).bHeld) vel.x = 200.0f;
    }
    // if (engine->GetKey(olc::S).bHeld) vel.y = 100.0f;

    vel.x += accel.x * fElapsedTime;
    vel.y += accel.y * fElapsedTime;

    updateCollisions(fElapsedTime, env, possibleCollidingTiles, playerContactPoint, playerContactNormal,
        playerT);

    // walljump mechanics (unfinished, prevent movement input for a split second to allow vel.x change)
    if (_allowingUserInput && (engine->GetKey(olc::W).bPressed || engine->GetKey(olc::SPACE).bPressed) &&
        playerContactNormal.y != 1 && playerT == 0.0f)
    {
        vel.y = -500.0f;
        vel.x = (playerContactNormal.x * fElapsedTime * 1500.0f);
    }

    //pos.x += getDisp1(vel.x, accel.x, fElapsedTime);
    //pos.y += getDisp1(vel.y, accel.y, fElapsedTime);
    // technically an approximation, but is ok 
    pos.x += vel.x * fElapsedTime;
    pos.y += vel.y * fElapsedTime;

    updateHitbox();

    // mouse info
    engine->DrawLine({ (int)_center.x, (int)_center.y }, { (int)mouse.x, (int)mouse.y }, olc::YELLOW);
    vec2D mouseDist = { mouse.x - _center.x, mouse.y - _center.y };
    _lookAngleVector = vec2DNormalise(mouseDist);
    _lookAngleDeg = atan2f(mouseDist.y, mouseDist.x);
    _lookAngleDeg = radToDeg(_lookAngleDeg);
    engine->DrawString({ 50, 630 }, "look vector: (" + std::to_string(_lookAngleVector.x) + " " + std::to_string(_lookAngleVector.y) + ")");
    engine->DrawString({ 50, 660 }, "look angle: " + std::to_string(_lookAngleDeg) + " deg");

    // ability updates
    if (engine->GetKey(olc::F).bPressed && _parryCtr == 0.0f) {
        std::cout << "parry" << std::endl;
        _parryCtr += fElapsedTime;

        vec2D parryBoxSize = { 100.0f, 100.0f };
        vec2D lavScaled = vec2DMult(_lookAngleVector, 100.0f);
        // vec2D newParryPos = vec2DAdd(pos, lavScaled);
        vec2D dummy = { _center.x - parryBoxSize.x / 2.0f, _center.y - parryBoxSize.y / 2.0f };
        vec2D newParryPos = vec2DAdd(dummy, lavScaled);

        // newParryPos.x -= parryBoxSize.x / 2.0f; newParryPos.y -= parryBoxSize.y / 2.0f;
        _parryBox = new GameObject(newParryPos, parryBoxSize, { 0, 0 }, { 0, 0 }, false, false);

    }

    if (_parryCtr > 0.0f) {
        if (_parryCtr < _parryDuration) {
            _parryCtr += fElapsedTime;

            vec2D pbPos = _parryBox->pos;
            vec2D pbSize = _parryBox->getSize();
            engine->FillRect({ (int)pbPos.x, (int)pbPos.y }, { (int)pbSize.x, (int)pbSize.y }, olc::GREEN);

            pbPos.x += pbSize.x / 2.0f; pbPos.y += pbSize.y / 2.0f;
            vec2D lavNormal = { _lookAngleVector.y, -_lookAngleVector.x };
            engine->DrawLine({  (int)(pbPos.x - std::min(70.0f, lavNormal.x * 70.0f)), 
                                (int)(pbPos.y - std::min(70.0f, lavNormal.y * 70.0f)) },
                            {   (int)(pbPos.x + std::min(70.0f, lavNormal.x * 70.0f)), 
                                (int)(pbPos.y + std::min(70.0f, lavNormal.y * 70.0f)) },
                olc::BLUE);

        }
        else {
            _parryCtr = 0.0f;
        }
    }

    // mouse updates

    // why is there a delay to the projectile?
    // projetile's trajectory is delayed by one click
    // i think it has something to do with the mouse event click delay and how it
    // delays the variable updating
    // sometimes, the init vel.y is opposite, too
    if (engine->GetMouse(0).bPressed) {
        vec2D mouseDist = { engine->GetMouseX() - _center.x, engine->GetMouseY() - _center.y};
        _lookAngleVector = vec2DNormalise(mouseDist);
        //_lookAngleDeg = atan2f(mouseDist.y, mouseDist.x);
        //_lookAngleDeg = radToDeg(_lookAngleDeg);
        engine->DrawString({ 50, 630 }, "look vector: (" + std::to_string(_lookAngleVector.x) + " " + std::to_string(_lookAngleVector.y) + ")");

        std::cout << "m1 clicked; shooting" << std::endl;
        vec2D projVel = vec2DMult(_lookAngleVector, fElapsedTime * 400000.0f);
        std::cout << projVel.x << " " << projVel.y << std::endl;
        Projectile testProj = Projectile(_center, 10, LINE, true, projVel, { 0, 0 }, olc::RED);
        testProj.bounces = 3;
        env->addProjectile(testProj);
    }
    engine->DrawSprite({ (int)pos.x, (int)pos.y }, _sprite.get());

    // debug info
    if (engine->GetKey(olc::I).bPressed) {
        std::cout << "pos: (" << pos.x << ", " << pos.y << ")" << std::endl;
        std::cout << "vel: (" << vel.x << ", " << vel.y << ")" << std::endl;
        std::cout << "accel: (" << accel.x << ", " << accel.y << ")" << std::endl;
        std::cout << "normal: (" << playerContactNormal.x << ", " << playerContactNormal.y << ")" << std::endl;
        std::cout << "contact pt: (" << playerContactPoint.x << ", " << playerContactPoint.y << ")" << std::endl;
        std::cout << "pct: " << playerT << std::endl;
    }

}

void Player::updateCollisions(float& fElapsedTime, Environment* env, 
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

float Player::getLookAngleDeg() { return _lookAngleDeg; }
vec2D Player::getLookAngleVec() { return _lookAngleVector; }