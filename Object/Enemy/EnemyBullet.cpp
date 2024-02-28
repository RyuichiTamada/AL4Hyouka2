#include "Enemy/EnemyBullet.h"
#include "MyMath.h"
#include "Collision/ColliderConfig.h"
#include <cmath>
#include <cassert>

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) { 
	assert(model);

	model_ = model;

	bulletTexture_ = TextureManager::Load("white1x1.png");

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);

	SetRadius(1.0f);

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	SettingScale();

	velocity_ = velocity;
}

void EnemyBullet::Update() {
	Vector3 toPlayer = Subtract(player_->GetWorldPosition(), worldTransform_.translation_);
	toPlayer = Normalize(toPlayer);

	velocity_ = Normalize(velocity_);

	velocity_ = Slerp(velocity_, toPlayer, 0.1f);
	velocity_.x *= 0.5f;
	velocity_.y *= 0.5f;
	velocity_.z *= 0.5f;

	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	worldTransform_.rotation_.x = atan2(-velocity_.y, velocityXZ);

	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	worldTransform_.UpdateMatrix();

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) { 
	if (!isDead_) {
		model_->Draw(worldTransform_, viewProjection, bulletTexture_);
	}
}

bool EnemyBullet::UnCollision() {
	return false;
}

bool EnemyBullet::OnCollision() { 
	isDead_ = true;

	return true;
}

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPosition{};
	
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];

	return worldPosition;
}

void EnemyBullet::SettingScale() { 
	worldTransform_.scale_.x = 1.0f;
	worldTransform_.scale_.y = 1.0f;
	worldTransform_.scale_.z = 1.0f;
}