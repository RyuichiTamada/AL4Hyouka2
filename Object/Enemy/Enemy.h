#pragma once

#include "Collision/Collider.h"
#include "EnemyBullet.h"
#include "Model.h"
#include "Player.h"
#include "WorldTransform.h"
#include "TimedCall.h"
#include <functional>

class Enemy;
class GameScene;

class EnemyState {
public:
	virtual ~EnemyState();
	virtual void Initialize(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy) = 0;
};

class EnemyStateApproach : public EnemyState {
public:
	~EnemyStateApproach();

	void Initialize(Enemy* enemy);
	void Update(Enemy* enemy);

	void FireAndResetTimer();

public:
	Enemy* enemy_;
	static const int kFireInterval = 60;
	std::list<TimedCall*> timedCalls_;
};

class EnemyStateLeave : public EnemyState {
public:
	void Initialize(Enemy* enemy);
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
};

class Enemy : public Collider {
public:
	Enemy();
	~Enemy();

	void Initalize(Model* model, const Vector3& position);
	void Update();
	void Draw(ViewProjection& viewProjection);

	bool UnCollision() override;
	bool OnCollision() override;

	Vector3 GetWorldPosition() override;
	Vector3 GetEnemyPosition() { return this->worldTransform_.translation_; }

	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }

	void ChangeState(EnemyState* enemyState);

	void Move(const Vector3& velocity);

	void Fire();

private:
	EnemyState* state_;

	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	uint32_t enemyTexture_ = 0u;

	Player* player_ = nullptr;

	GameScene* gameScene_ = nullptr;

	bool isDead_ = false;
};