#include "../h/entity.h"

Entity::Entity() {}
Entity::Entity(std::string name, vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, 
    EntityType entityType, AIType aiType, const std::string spriteSheet, float damage, 
    bool affectedByGrav, bool tangible)
{

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
    spriteSheetFilename = spriteSheet;
    animHandler = new AnimationHandler(spriteSheetFilename, AnimationState::IDLE, _size);
    animHandler->setFPS(15);
    animHandler->setAnimType(AnimationState::IDLE, 1);
}
Entity::~Entity() {}

// collisions and behavior will be handled by environment. Why? because i suck at coding
void Entity::update(float& fElapsedTime) {

    //vel.x += accel.x * fElapsedTime;
    //vel.y += accel.y * fElapsedTime;

    pos.x += vel.x * fElapsedTime;
    pos.y += vel.y * fElapsedTime;

    updateHitbox();

    (vel.x > 0) ? animHandler->flip = 0 : animHandler->flip = 1;
}

void Entity::draw(olc::PixelGameEngine* engine, vec2D& displayOffset, float& fElapsedTime) {
    if (spriteSheetFilename == "NULL") {
        engine->FillRectDecal({ (pos.x + displayOffset.x), (pos.y + displayOffset.y) },
            { _size.x, _size.y }, color);
    }
    else {
        animHandler->update(engine, pos, _size, displayOffset, fElapsedTime, false);
    }
    engine->DrawStringDecal({ pos.x + displayOffset.x, pos.y - 15 + displayOffset.y },
        "HP: " + std::to_string(hp), color);
}

// entity behavior update occurs in environment
//void Entity::updateEntityBehavior(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D& playerPos) {
//    return;
//}

// returns dummy, friendly, or enemy
EntityType Entity::getType() { return _type; }

// returns specific ai behavior type
AIType Entity::getAI() { return _ai; }

