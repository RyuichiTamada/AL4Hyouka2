#include "PlayerBullet.h"
#include "MyMath.h"
#include "Collision/ColliderConfig.h"
#include "WorldTransform.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	assert(model);

	model_ = model;
	bulletTexture_ = TextureManager::Load("enemyBullet.png");

	SetRadius(1.0f);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;
}

void PlayerBullet::Update() { 
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	float velocisyXZ;
	velocisyXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, velocisyXZ);

	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	worldTransform_.UpdateMatrix();

	if (--deathTimer <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, bulletTexture_);
}

bool PlayerBullet::UnCollision() { 
	return false;
}

bool PlayerBullet::OnCollision() { 
	isDead_ = true;

	return true;
}

Vector3 PlayerBullet::GetWorldPosition() {
	Vector3 worldPosition;
	
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];

	return worldPosition;
}