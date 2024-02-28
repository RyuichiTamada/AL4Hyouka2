#pragma once

#include "ColliderConfig.h"
#include "Collider.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include <list>

class GameScene;

class CollisionManager {
public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetPlayerBullet(const std::list<PlayerBullet*>& playerBullet) { playerBullet_ = playerBullet; }

	void SetEnemy(const std::list<Enemy*>& enemy) { enemy_ = enemy; }
	void SetEnemyBullet(const std::list<EnemyBullet*>& enemyBullet) { enemyBullet_ = enemyBullet; }

	void SetGameObject(
	    Player* player, const std::list<PlayerBullet*>& playerBullet,
	    const std::list<Enemy*>& enemy, const std::list<EnemyBullet*>& enemyBullet);
	
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	void CheckAllCollisions(GameScene* gameScene);

private:
	std::list<Collider*> colliders_;

	Player* player_;
	std::list<PlayerBullet*> playerBullet_;

	std::list<Enemy*> enemy_;
	std::list<EnemyBullet*> enemyBullet_;
};