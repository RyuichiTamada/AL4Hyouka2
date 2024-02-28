#include "Enemy.h"
#include "MyMath.h"
#include "Collision/ColliderConfig.h"
#include "GameScene.h"
#include <cassert>

Enemy::Enemy() { state_ = new EnemyStateApproach(); }

Enemy::~Enemy() {

}

EnemyState::~EnemyState() {

}

void Enemy::Initalize(Model* model, const Vector3& position) { 
	assert(model);

	model_ = model;
	enemyTexture_ = TextureManager::Load("enemy.png");

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);

	SetRadius(8.0f);

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {4, 4, 4};

	state_->Initialize(this);
}

void Enemy::Update() { 
	state_->Update(this);

	worldTransform_.UpdateMatrix();
	worldTransform_.TransferMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, enemyTexture_);
}

bool Enemy::UnCollision() { 
	return false; 
}

bool Enemy::OnCollision() { 
	isDead_ = true;
	return true;
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 worldPosition;

	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];

	return worldPosition;
}

void Enemy::ChangeState(EnemyState* enemyState) { 
	delete state_;
	state_ = enemyState;
}

void Enemy::Move(const Vector3& velocity) {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);
}

void Enemy::Fire() { 
	assert(player_);

	const float kBulletSpeed = -0.5f;
	Vector3 velocity{0, 0, kBulletSpeed};

	player_->GetWorldPosition();

	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, GetWorldPosition(), velocity);
	newBullet->SetPlayer(player_);

	gameScene_->AddEnemyBullet(newBullet);
}

EnemyStateApproach::~EnemyStateApproach() { 
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void EnemyStateApproach::FireAndResetTimer() { 
	enemy_->Fire();

	timedCalls_.push_back(new TimedCall(std::bind(&EnemyStateApproach::FireAndResetTimer, this), kFireInterval));
}

void EnemyStateApproach::Initialize(Enemy* enemy) { 
	enemy_ = enemy;

	timedCalls_.push_back(new TimedCall(std::bind(&EnemyStateApproach::FireAndResetTimer, this), kFireInterval));
}

void EnemyStateApproach::Update(Enemy* enemy) {
	const Vector3 kMoveSpeed = {0, 0, -0.01f};

	enemy->Move(kMoveSpeed);

	timedCalls_.remove_if([](TimedCall* timedCall) { 
		if (timedCall->IsFinish()) {
			delete timedCall;
			return true;
		}
		return false;
	});

	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	if (enemy->GetEnemyPosition().z < -15.0f) {
		timedCalls_.clear();
		enemy->ChangeState(new EnemyStateLeave());
	}
}

void EnemyStateLeave::Initialize(Enemy* enemy) { enemy_ = enemy; }

void EnemyStateLeave::Update(Enemy* enemy) { 
	const Vector3 kMoveSpeed = {-0.05f, 0.05f, -0.05f};

	enemy->Move(kMoveSpeed);
}