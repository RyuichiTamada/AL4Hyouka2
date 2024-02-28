#include "ColliderManager.h"
#include "GameScene.h"

void CollisionManager::SetGameObject(
    Player* player, const std::list<PlayerBullet*>& playerBullet, const std::list<Enemy*>& enemy,
    const std::list<EnemyBullet*>& enemyBullet) {
	SetPlayer(player);
	SetPlayerBullet(playerBullet);
	SetEnemy(enemy);
	SetEnemyBullet(enemyBullet);
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
	    (colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {
		return;
	}

	colliderA->GetWorldPosition();
	colliderB->GetWorldPosition();

	Vector3 a2b = {
	    colliderA->GetWorldPosition().x - colliderB->GetWorldPosition().x,
	    colliderA->GetWorldPosition().y - colliderB->GetWorldPosition().y,
	    colliderA->GetWorldPosition().z - colliderB->GetWorldPosition().z};

	float a2bR = colliderA->GetRadius() + colliderB->GetRadius();

	if ((a2b.x * a2b.x) + (a2b.y * a2b.y) + (a2b.z * a2b.z) <= (a2bR * a2bR)) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	} else {
		colliderA->UnCollision();
		colliderB->UnCollision();
	}
}

void CollisionManager::CheckAllCollisions(GameScene* gameScene) {
	colliders_.clear();

	const std::list<PlayerBullet*>& playerBullets = gameScene->GetPlayerBullets();
	
	const std::list<EnemyBullet*>& enemyBullets = gameScene->GetEnemyBullets();

	colliders_.push_back(player_);
	
	for (PlayerBullet* bullet : playerBullets) {
		colliders_.push_back(bullet);
	}

	for (Enemy* enemy : enemy_) {
		colliders_.push_back(enemy);
	}
	
	for (EnemyBullet* bullet : enemyBullets) {
		colliders_.push_back(bullet);
	}
	
	std::list<Collider*>::iterator itrA = colliders_.begin();

	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}