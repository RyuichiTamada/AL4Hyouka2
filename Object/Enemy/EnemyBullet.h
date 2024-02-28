#pragma once

#include "Collision/Collider.h"
#include "Model.h"
#include "Player.h"
#include "WorldTransform.h"

class EnemyBullet : public Collider {
public:
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);
	void Update();
	void Draw(const ViewProjection& viewProjection);

	bool UnCollision() override;
	bool OnCollision() override;

	Vector3 GetWorldPosition() override;

	void SetPlayer(Player* player) { player_ = player; }

	bool IsDead() const { return isDead_; }

	void SettingScale();

private:
	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	uint32_t bulletTexture_ = 0u;

	Player* player_ = nullptr;

	static const int32_t kLifeTime_ = 60 * 5;

	int32_t deathTimer_ = kLifeTime_;

	bool isDead_ = false;

	Vector3 velocity_;
};