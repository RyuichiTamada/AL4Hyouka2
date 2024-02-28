#pragma once

#include "Collision/Collider.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include <optional>

class GameScene;

class Player : public Collider {
public:
	Player();
	~Player();

	void Initialize(
	    Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm,
	    Model* modelWeapon, Model* modelBullet);

	void Update();

	void DrawUI();
	void Draw(ViewProjection& viewProjection);

	bool UnCollision() override;
	bool OnCollision() override;

	Vector3 GetWorldPosition() override;

	void Attack();

	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

	void BehaviorRootUpdate();
	void BehaviorRootInitialize();

	void Create3DReticle();
	void Create2DReticle(const ViewProjection& viewProjection);

	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	const WorldTransform& GetWorldTransformBase() { return worldTransformBase_; }
	Vector3 GetWorld3DReticlePosition();

	void SetWorldPosition(Vector3 prePosition);
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetParent(const WorldTransform* parent);

private:
	enum class Behavior { kRoot };
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformWeapon_;

	WorldTransform worldTransform3DReticle_;

	const ViewProjection* viewProjection_ = nullptr;

	Model* modelBody_;
	Model* modelHead_;
	Model* modelL_arm_;
	Model* modelR_arm_;
	Model* modelWeapon_;
	Model* modelBullet_;

	float bulletImpactPoint_;
	float floatingParameter_ = 0.0f;

	Sprite* sprite2DReticle_ = nullptr;
	uint32_t reticleTexture_ = 0u;
	Matrix4x4 matViewPort_;

	GameScene* gameScene_;

	bool isDead_ = true;

	Vector2 spritePosition_;

	bool isBullet_;
	
	Input* input_;

	Vector3 velocity_;
};