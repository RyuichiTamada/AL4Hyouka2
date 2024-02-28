#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"

class PlayerBullet : public Collider {
public:
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	void Update();
	void Draw(const ViewProjection& viewProjection);

	bool UnCollision() override;
	bool OnCollision() override;
	Vector3 GetWorldPosition() override;

	bool IsDead() const { return isDead_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t bulletTexture_ = 0u;

	static const int32_t kLifeTime = 60 * 4;
	int32_t deathTimer = kLifeTime;
	bool isDead_ = false;

	Vector3 velocity_;
};