#include "../h/environment.h"

Environment::Environment() {}
Environment::~Environment() {}

bool Environment::gameObjCollidedWithEnv(GameObject& gameObject, GameObject& collidedTile) {

	AABB objectHitbox = gameObject.getHitbox();

	for (Tile& i : _tangibleTiles) {
		AABB tileHitbox = i.getHitbox();
		if (checkAABBCollision(objectHitbox, tileHitbox)) {
			collidedTile = i;
			return true;
		}
	}

	return false;
}

void Environment::drawTiles(olc::PixelGameEngine* pge, float fElapsedTime) { 
	for (Tile &t : _tangibleTiles) t.update(pge, fElapsedTime); 
	for (Tile &t : _intangibleTiles) t.update(pge, fElapsedTime);

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
	bool affectedByGrav, bool tangible, bool parriable) {
	Projectile proj = Projectile(initPos, size, shape, friendly, initVel, initAccel, initColor, affectedByGrav, tangible, parriable);
	_projectiles.push_back(proj);
}

void Environment::addProjectile(Projectile& projectile) {
	_projectiles.push_back(projectile);
}

void Environment::drawProjectiles(olc::PixelGameEngine* pge, float fElapsedTime, vec2D &mouse) {
	if (_projectiles.size() == 0) return;


	int screenWidth = pge->GetScreenSize().x;
	int screenHeight = pge->GetScreenSize().y;

	for (int i = 0; i < _projectiles.size() - 1; i++) {
		Projectile* p = &_projectiles[i];

		p->update(pge, fElapsedTime, mouse);

		// if out of screen
		if (!checkPtCollision(p->pos, { {0, 0}, {(float)screenWidth, (float)screenHeight } })
			&& p->bounces > 0) {
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
					if (p->bounces == 0) _eraseProj(i);

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