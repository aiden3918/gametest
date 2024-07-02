#include "../h/entity.h"

Entity::Entity() {}
Entity::Entity(vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType, 
	bool affectedByGrav, bool tangible) {

    pos = initPos;
    vel = initVel;
    accel = initAccel;
    _size = size;

    _affectedByGrav = affectedByGrav;
    isTangible = tangible;

    if (affectedByGrav) accel.y = 1000.0f;

    updateHitbox();

    _type = entityType;
    color = defaultEntityColors[entityType];
}
Entity::~Entity() {}

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

EntityType Entity::getType() { return _type; }

