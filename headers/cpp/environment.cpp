#include "../h/environment.h"

Environment::Environment() {}
Environment::~Environment() {}

void Environment::drawTiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D &displayOffset) {

	if (_tangibleTiles.size() == 0 && _intangibleTiles.size() == 0) return;

	for (Tile &t : _tangibleTiles) t.update(pge, fElapsedTime, displayOffset);
	for (Tile &t : _intangibleTiles) t.update(pge, fElapsedTime, displayOffset);

	if (pge->GetKey(olc::T).bPressed) std::cout << _tangibleTiles.size() << std::endl;
}

void Environment::addTile(vec2D& initPos, vec2D& size, vec2D initVel, vec2D initAccel, olc::Pixel color, bool affectedByGrav, bool tangible) {

	Tile newTile = Tile(initPos, size, initVel, initAccel, color, affectedByGrav, tangible);
	(newTile.isTangible) ? _tangibleTiles.push_back(newTile) : _intangibleTiles.push_back(newTile);

}

void Environment::addTile(Tile& newTile) { (newTile.isTangible) ? _tangibleTiles.push_back(newTile) : _intangibleTiles.push_back(newTile); }

std::vector<Tile> Environment::getTangibleTiles() { return _tangibleTiles; }
std::vector<Tile> Environment::getIntangibleTiles() { return _intangibleTiles; }

void Environment::addProjectile(vec2D initPos, float size, ProjShape shape, bool friendly,
	vec2D initVel, vec2D initAccel, olc::Pixel initColor,
	bool affectedByGrav, bool tangible, bool parriable) 
{
	Projectile proj = Projectile(initPos, size, shape, friendly, initVel, initAccel, initColor, affectedByGrav, tangible, parriable);
	_projectiles.push_back(proj);
}

void Environment::addProjectile(Projectile& projectile) {
	_projectiles.push_back(projectile);
}

void Environment::drawProjectiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D &mouse, 
	vec2D &displayOffset) 
{
	if (_projectiles.size() == 0) return;

	int screenWidth = pge->GetScreenSize().x;
	int screenHeight = pge->GetScreenSize().y;

	for (int i = 0; i < _projectiles.size(); i++) {
		Projectile* p = &_projectiles[i];

		p->update(pge, fElapsedTime, mouse, displayOffset);

		// if out of screen
		vec2D projDisplayPos = { p->pos.x + displayOffset.x, p->pos.y + displayOffset.y };

		if (!checkPtCollision(projDisplayPos, { {0, 0}, {(float)screenWidth, (float)screenHeight} })
			|| p->bounces < 1 || p->pierce < 1) {

			_eraseProj(i);
			return;

		}
		
		if (p->getShape() == LINE) {

			for (auto& t : _tangibleTiles) {
				collisionDirectionState colDir;
				AABB tHb = t.getHitbox();
				vec2D projCtPt; vec2D projCtN; float projCtT;

				vec2D projDir = vec2DMult(p->vel, fElapsedTime);
				// projectiles deleting early
				// likely ray collisions are checking too far ahead
				if (checkRayCollision(p->pos, projDir, tHb, colDir, projCtPt, projCtN, projCtT) &&
					projCtT >= 0.0f && projCtT <= 1.0f) {

					p->bounces--;

					/*(projCtN.x != 0) ? p->vel.x *= -1.0f : p->vel.y *= -1.0f;*/

					// reverse dir (-> + 2(<-) = <-)
					//p->vel.x += 2.0f * (projCtN.x * std::abs(p->vel.x));
					//p->vel.y += 2.0f * (projCtN.y * std::abs(p->vel.y));

					if (projCtN.x != 0) p->vel.x *= -1.0f;
					if (projCtN.y != 0) p->vel.y *= -1.0f;

				}
			}

		} else {
			return;
		}

	}
	//for (auto& p : _projectiles) {
	//	if (p.getShape() == LINE && (p.pos.x > screenWidth || p.pos.x < 0 || p.pos.y > screenHeight || p.pos.y < 0)) {
	//		_projectiles.erase(std::remove(_projectiles.begin(), _projectiles.end(), p));
	//	}
	//	p.update(pge, fElapsedTime, mouse);
	//}
}

void Environment::_eraseProj(int& index) {
	_projectiles.erase(_projectiles.begin() + index);
	std::cout << "proj deleted" << std::endl;
}

void Environment::drawEntities(olc::PixelGameEngine* pge, float fElapsedTime, vec2D& mouse, vec2D& displayOffset) {
	if (_entities.size() == 0) return;
	
	for (int i = 0; i < _entities.size(); i++) {
		if (_entities[i].hp <= 0) {
			_deleteEntity(i);
			return;
		}
	}

	handleEntityTileCollisions(fElapsedTime);
	handleEntityProjCollisions(fElapsedTime);
	for (auto &e: _entities) e.update(pge, fElapsedTime, mouse, displayOffset);
}

void Environment::addEntity(Entity& entity) {
	_entities.push_back(entity);
}

void Environment::addEntity(vec2D initPos, vec2D initVel, vec2D initAccel, vec2D size, EntityType entityType,
	bool affectedByGrav, bool tangible) {
	Entity entity = Entity(initPos, initVel, initAccel, size, entityType, affectedByGrav, tangible);
	_entities.push_back(entity);
}

void Environment::_deleteEntity(int& index) {
	_entities.erase(_entities.begin() + index);
}

void Environment::handleEntityTileCollisions(float &fElapsedTime) {

	for (auto& e : _entities) {
		e.vel.x += e.accel.x * fElapsedTime;
		e.vel.y += e.accel.y * fElapsedTime;

		std::vector<std::pair<GameObject, float>> possibleColTiles;
		vec2D pct; vec2D pcn; float pt;

		for (auto& i : getTangibleTiles()) {
			if (checkDynamicRectVsRectCollision(e, i, fElapsedTime, pct, pcn, pt)) {

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
			if (checkDynamicRectVsRectCollision(e, k.first, fElapsedTime, pct, pcn, pt)) {
				e.vel.x += pcn.x * std::abs(e.vel.x) * (1 - pt);
				e.vel.y += pcn.y * std::abs(e.vel.y) * (1 - pt);
			}
		}
	}
}

void Environment::handleEntityProjCollisions(float& fElapsedTime) {
	for (auto& e : _entities) {
		AABB entityHB = e.getHitbox();
		for (auto& p : _projectiles) {
			if (p.getShape() == LINE) {
				if (checkPtCollision(p.pos, entityHB) && (e.getType() == DUMMY || e.getType() == ENEMY)) {
					std::cout << "something got hit" << std::endl;
					e.hp -= p.dmg;
					p.pierce--;
				}
			}
			// for circle
			else { return; }
		}
	}
}

std::vector<Entity> Environment::getEntities() { return _entities; }

//bool Environment::gameObjCollidedWithEnv(GameObject& gameObject, GameObject& collidedTile) {
//
//	AABB objectHitbox = gameObject.getHitbox();
//
//	for (Tile& i : _tangibleTiles) {
//		AABB tileHitbox = i.getHitbox();
//		if (checkAABBCollision(objectHitbox, tileHitbox)) {
//			collidedTile = i;
//			return true;
//		}
//	}
//
//	return false;
//}