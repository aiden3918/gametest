#include "../h/entity.h"

Entity::Entity() {}
Entity::Entity(std::string name, vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType, 
    AIType aiType, float damage, bool affectedByGrav, bool tangible) {

    _name = name;

    pos = initPos;
    vel = initVel;
    accel = initAccel;
    _size = size;

    _affectedByGrav = affectedByGrav;
    isTangible = tangible;

    if (affectedByGrav) accel.y = 1000.0f;

    updateHitbox();

    _type = entityType;
    _ai = aiType;

    int entityTypeNum = static_cast<int>(entityType);
    color = defaultEntityColors[entityTypeNum];

    switch (_type) {
    case EntityType::DUMMY: hp = 9999.0f; break;
    case EntityType::FRIENDLY: hp = 1.0f; break;
    case EntityType::ENEMY: hp = 10.0f; break;
    }

    dmg = damage;
}
Entity::~Entity() {}

// collisions and behavior will be handled by environment. Why? because i suck at coding
void Entity::update(olc::PixelGameEngine* engine, float fElapsedTime, vec2D& mouse, vec2D &displayOffset) {

    //vel.x += accel.x * fElapsedTime;
    //vel.y += accel.y * fElapsedTime;

    pos.x += vel.x * fElapsedTime;
    pos.y += vel.y * fElapsedTime;

    updateHitbox();

    engine->FillRect({ (int)(pos.x + displayOffset.x), (int)(pos.y + displayOffset.y) },
        { (int)_size.x, (int)_size.y }, color);
    engine->DrawString({ (int)(pos.x + displayOffset.x), (int)(pos.y - 15 + displayOffset.y) },
        "HP: " + std::to_string(hp), color);
}

// returns dummy, friendly, or enemy
EntityType Entity::getType() { return _type; }

// returns specific ai behavior type
AIType Entity::getAI() { return _ai; }


//void Entity::updateEntityBehavior(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D &playerPos) {
//    if (_ai == STATIONARY) return;
//
//    switch (_ai) {
//        case SENTRY: {
//            vec2D playerDirVec = vec2DSub(playerPos, pos);
//            playerDirVec = vec2DNormalise(playerDirVec);
//
//            if (attackCtr == 0.0f) {
//                attackCtr += fElapsedTime;
//            }
//            break;
//        }
//    }
//}

