#include "../h/environment.h"

Environment::Environment() {}
Environment::Environment(const std::string& worldDataFile) {
	std::ifstream worldData(worldDataFile);

	if (!worldData.is_open()) {
		std::cout << "cannot open file " << worldDataFile << "; environment has not been created" << std::endl;
		return;
	}
	std::cout << "successfully opened file" << worldDataFile << std::endl;

	while (!worldData.eof()) {
		char fileline[500];
		std::stringstream stream;

		worldData.getline(fileline, 500);

		// read current file line
		// note: it seems that the arrows indicate the direction of the passage of information 
		// during the stream
		stream << fileline;

		// used to store first letter of line
		char typeJunk;
		int rgb[3];
		std::string name;

		if (fileline[0] == 't') {

			vec2D initPos; vec2D size; vec2D initVel = { 0, 0 }; vec2D initAccel = { 0, 0 };
			std::string spriteSheetFilename = "NULL";
			olc::Pixel color = (0, 0, 0);
			bool affectedByGrav = false; bool tangible = true;

			stream >> typeJunk >> name >> initPos.x >> initPos.y >> size.x >> size.y >> initVel.x >> initVel.y
				>> initAccel.x >> initAccel.y >> spriteSheetFilename >> rgb[0] >> rgb[1] >> rgb[2] >> std::boolalpha >>
				affectedByGrav >> std::boolalpha >> tangible;

			Tile t = Tile(name, initPos, size, initVel, initAccel, spriteSheetFilename, 
				olc::Pixel(rgb[0], rgb[1], rgb[2]), affectedByGrav, tangible);

			addTile(t);

		}

		if (fileline[0] == 'e') {
		
			vec2D initPos; vec2D initVel; vec2D initAccel; vec2D size;
			int entityTypeNum; int aiTypeNum; float damage;
			bool affectedByGrav; bool tangible;

			// use std::map to create a conversion table for both
			std::string entityTypeStr; std::string aiTypeStr;

			stream >> typeJunk >> name >> initPos.x >> initPos.y >> initVel.x >> initVel.y >> initAccel.x >>
				initAccel.y >> size.x >> size.y >> entityTypeStr >> aiTypeStr >> damage >> std::boolalpha >>
				affectedByGrav >> std::boolalpha >> tangible;

			Entity e = Entity(name, initPos, initVel, initAccel, size, _entityTypeMap[entityTypeStr], _aiTypeMap[aiTypeStr],
				damage, affectedByGrav, tangible);
			e.moveSpeedVec = vec2DAbs(e.vel);
			e.moveSpeed = vec2DMag(e.moveSpeedVec);
			e.jumpSpeed = 400.0f; // set something later, maybe
			addEntity(e);

		}

		if (fileline[0] == 'p') {
			vec2D initPos;  float size; int shape; bool friendly; vec2D initVel;
			vec2D initAccel; olc::Pixel initColor; bool affectedByGrav;
			bool tangible; bool parriable;

			stream >> typeJunk >> name >> initPos.x >> initPos.y >> size >> shape >> friendly >> initVel.x
				>> initVel.y >> initAccel.x >> initAccel.y >> rgb[0] >> rgb[1] >> rgb[2] >> std::boolalpha >>
				affectedByGrav >> std::boolalpha >> tangible >> std::boolalpha >> parriable;

			addProjectile(name, initPos, size, ProjShape(shape), friendly, initVel, initAccel,
				olc::Pixel(rgb[0], rgb[1], rgb[2]), affectedByGrav, tangible, parriable);
		}

		if (fileline[0] == 's') {
			stream >> typeJunk >> _spawnLocation.x >> _spawnLocation.y;
		}

	}
}
Environment::~Environment() {}

void Environment::update(olc::PixelGameEngine* pge, olc::MiniAudio* ma,
	float& fElapsedTime, vec2D& displayOffset, vec2D& mouse, vec2D& playerPos, float& globalFreezeCtr)
{
	_notFrozen = globalFreezeCtr == 0.0f;
	drawTiles(pge, fElapsedTime, displayOffset);
	drawProjectiles(pge, fElapsedTime, mouse, displayOffset);
	drawEntities(pge, fElapsedTime, mouse, displayOffset, playerPos);
	if (!_notFrozen) pge->FillRectDecal({0, 0}, pge->GetScreenSize(), *_screenFlash);
}

void Environment::drawTiles(olc::PixelGameEngine* pge, float fElapsedTime,
	vec2D& displayOffset) {

	if (_tangibleTiles.size() == 0 && _intangibleTiles.size() == 0) return;

	for (Tile& t : _tangibleTiles) {
		if (_notFrozen) t.update(fElapsedTime);
		t.draw(pge, displayOffset, fElapsedTime);
	}
	for (Tile& t : _intangibleTiles) {
		if (_notFrozen) t.update(fElapsedTime);
		t.draw(pge, displayOffset, fElapsedTime);
	}

	if (pge->GetKey(olc::T).bPressed) std::cout << _tangibleTiles.size() << std::endl;
}

void Environment::addTile(std::string name, vec2D& initPos, vec2D& size, vec2D initVel, vec2D initAccel, 
	std::string spriteSheetFilename, olc::Pixel color, bool affectedByGrav, bool tangible) {

	Tile newTile = Tile(name, initPos, size, initVel, initAccel, spriteSheetFilename, color, 
		affectedByGrav, tangible);
	(newTile.isTangible) ? _tangibleTiles.push_back(newTile) : _intangibleTiles.push_back(newTile);

}

void Environment::addTile(Tile& newTile) { (newTile.isTangible) ? _tangibleTiles.push_back(newTile) : _intangibleTiles.push_back(newTile); }

std::vector<Tile> Environment::getTangibleTiles() { return _tangibleTiles; }
std::vector<Tile> Environment::getIntangibleTiles() { return _intangibleTiles; }

void Environment::addProjectile(std::string name, vec2D initPos, float size, ProjShape shape, bool friendly,
	vec2D initVel, vec2D initAccel, olc::Pixel initColor,
	bool affectedByGrav, bool tangible, bool parriable)
{
	Projectile proj = Projectile(name, initPos, size, shape, friendly, initVel, initAccel, initColor, affectedByGrav, tangible, parriable);
	_projectiles.push_back(proj);
}

void Environment::addProjectile(Projectile& projectile) {
	_projectiles.push_back(projectile);
}

void Environment::drawProjectiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D& mouse,
	vec2D& displayOffset)
{
	if (_projectiles.size() == 0) return;

	int screenWidth = pge->GetScreenSize().x;
	int screenHeight = pge->GetScreenSize().y;

	// check projectile collisions with tiles and entities (projectile reaction only)
	// but only if screen is not frozen
	if (_notFrozen) {
		for (int i = 0; i < _projectiles.size(); i++) {
			Projectile* p = &_projectiles[i];

			// filter from projectiles no longer "alive"
			if (p->bounces < 1 || p->pierce < 1 || p->lifespanCtr > p->lifespan) {
				_eraseProj(i);
				continue;
			}

			p->update(fElapsedTime);

			vec2D projCtPt; vec2D projCtN; float projCtT;
			vec2D projDir = vec2DMult(p->vel, fElapsedTime);
			AABB projHB = p->getHitbox();

			for (auto& t : _tangibleTiles) {
				AABB tileHB = t.getHitbox();

				// if not collided with tile, continue
				switch (p->getShape()) {
				case ProjShape::LINE: {
					if (!(checkRayCollision(p->pos, projDir, tileHB, projCtPt, projCtN, projCtT) &&
						projCtT >= 0.0f && projCtT <= 1.0f)) continue;
					break;
				}

				case ProjShape::CIRCLE:
				case ProjShape::RECT: {
					if (!(checkDynamicRectVsRectCollision(*p, t, fElapsedTime, projCtPt, projCtN, projCtT)
						&& projCtT >= 0.0f && projCtT <= 1.0f)) continue;
					break;
				}

				}

				p->bounces--;
				if (projCtN.x != 0) p->vel.x *= -1.0f;
				if (projCtN.y != 0) p->vel.y *= -1.0f;

			}
		}
	}

	for (auto& p : _projectiles) {
		vec2D projDisplayPos = { p.pos.x + displayOffset.x, p.pos.y + displayOffset.y };
		p.draw(pge, displayOffset);
	}
}

void Environment::_eraseProj(int& index) {
	_projectiles.erase(_projectiles.begin() + index);
	std::cout << "proj deleted" << std::endl;
}

std::vector<Projectile> Environment::getProjectiles() { return _projectiles; }
std::vector<Projectile>* Environment::getActualProjectilesVec() {
	return &_projectiles;
}

void Environment::drawEntities(olc::PixelGameEngine* pge, float fElapsedTime, vec2D& mouse,
	vec2D& displayOffset, vec2D& playerPos) {
	if (_entities.size() == 0) return;

	// delete dead entities
	for (int i = 0; i < _entities.size(); i++) {
		if (_entities[i].hp <= 0) {
			_deleteEntity(i);
			return;
		}
	}

	if (_notFrozen) {
		handleEntityTileCollisions(fElapsedTime);
		handleEntityProjCollisions(fElapsedTime);

		for (Entity& e : _entities) e.update(fElapsedTime);

		updateEntityBehaviors(pge, fElapsedTime, playerPos);
	}

	for (Entity& e : _entities) e.draw(pge, displayOffset);
}

void Environment::addEntity(Entity& entity) {
	_entities.push_back(entity);
}

void Environment::addEntity(std::string name, vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType,
	AIType aiType, float damage, bool affectedByGrav, bool tangible) {
	Entity entity = Entity(name, initPos, initVel, initAccel, size, entityType, aiType, damage, affectedByGrav,
		tangible);
	_entities.push_back(entity);
}

void Environment::_deleteEntity(int& index) {
	_entities.erase(_entities.begin() + index);
}

// entity reaction to tiles
void Environment::handleEntityTileCollisions(float& fElapsedTime) {

	for (auto& e : _entities) {
		e.vel.x += e.accel.x * fElapsedTime;
		e.vel.y += e.accel.y * fElapsedTime;

		for (auto& i : getTangibleTiles()) {
			if (checkDynamicRectVsRectCollision(e, i, fElapsedTime, e.pcp, e.pcn, e.pt)) {

				//std::cout << "Player contact with test box" << std::endl;
				//std::cout << pcn.x << " " << pcn.y << std::endl;

				e.possibleColTiles.push_back({ i, e.pt });
			}
		}

		// sort tiles by proximity by checking tNear;
		std::sort(e.possibleColTiles.begin(), e.possibleColTiles.end(),
			[](const std::pair<GameObject, float>& a, const std::pair<GameObject, float>& b) {
				return a.second < b.second;
			}
		);

		for (auto& k : e.possibleColTiles) {
			if (checkDynamicRectVsRectCollision(e, k.first, fElapsedTime, e.pcp, e.pcn, e.pt)) {
				e.vel.x += e.pcn.x * std::abs(e.vel.x) * (1 - e.pt);
				e.vel.y += e.pcn.y * std::abs(e.vel.y) * (1 - e.pt);
			}
		}

		e.possibleColTiles.clear();
	}
}

// entity reaction to projectiles
void Environment::handleEntityProjCollisions(float& fElapsedTime) {

	vec2D projCtPt; vec2D projCtNorm; float projT;

	for (auto& e : _entities) {

		AABB entityHB = e.getHitbox();

		// check ALL projectiles for collision with current entity;
		for (auto& p : _projectiles) {

			// trying to avoid nested hell 
			// lines
			// projT can be less than 0.0f because that would mean its inside
			// DEBUG:	if projectile spawned inside, then it doesnt register. however, if you use regular
			//			AABB collision, then local iframe implementation is needed
			//			quick fix: make sure that all projectiles are able to completely pass through an
			//			entity to trigger colllision
			//			UPDATE: limited success with that one
			if (p.getShape() == ProjShape::LINE) {
				vec2D projDir = vec2DMult(p.vel, fElapsedTime);
				if (!(checkRayCollision(p.pos, projDir, entityHB, projCtPt, projCtNorm, projT) &&
					projT < 1.0f && projT > 0.0f)) continue;
			}
			// circles and rects
			else {
				AABB projHB = p.getHitbox();
				if (!checkDynamicRectVsRectCollision(p, e, fElapsedTime, projCtPt, projCtNorm, projT)) continue;
			}

			if ((p.isFriendly && e.getType() != EntityType::FRIENDLY) ||
				(!p.isFriendly && e.getType() != EntityType::ENEMY)) {
				std::cout << "something got hit" << std::endl;
				e.hp -= p.dmg;
				p.pierce--;
			}
		}
	}
}

void Environment::updateEntityBehaviors(olc::PixelGameEngine* engine, float& fElapsedTime, vec2D& playerPos) {

	// cannot move this to entity.cpp because it spawns in projectiles (cyclical linker error)
	for (auto& e : _entities) {

		if (e.getAI() == AIType::STATIONARY) continue;

		vec2D entityCenter = e.getCenter();
		vec2D playerDirVec = vec2DSub(playerPos, entityCenter);
		playerDirVec = vec2DNormalise(playerDirVec);

		switch (e.getAI()) {
		case AIType::SENTRY: {
			if (e.attackCtr == 0.0f) {
				vec2D projVel = vec2DMult(playerDirVec, e.projSpeed);

				Projectile entityProj = Projectile("entityProj", entityCenter, 10, ProjShape::LINE, false, projVel, { 0, 0 }, olc::RED);
				addProjectile(entityProj);

				e.attackCtr += 0.0001f;
			}
			else {
				(e.attackCtr > e.attackInterval) ? e.attackCtr = 0.0f : e.attackCtr += fElapsedTime;
			}
			break;
		}
		case AIType::WALKER: {
			// move to player's x
			(playerDirVec.x > 0) ? e.vel.x = e.moveSpeedVec.x : e.vel.x = -e.moveSpeedVec.x;
			// attempt to jump if player is above
			vec2D onGround = { 0, -1 };
			if (playerDirVec.y < -0.3f && e.pcn == onGround && e.pt == 0.0f) {
				e.vel.y = -e.jumpSpeed;
			}
			break;
		}
		case AIType::DRONE: {
			// simply just fly towards player
			e.vel = vec2DMult(playerDirVec, e.moveSpeed);
			// should i set all drones' affectedByGrav to false by default?
			break;
		}
		}
	}

}

std::vector<Entity> Environment::getEntities() { return _entities; }
vec2D Environment::getSpawnPoint() { return _spawnLocation; }