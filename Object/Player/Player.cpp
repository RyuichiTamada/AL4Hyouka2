#define _USE_MATH_DEFINES

#include "Player.h"
#include "MyMath.h"
#include <Input.h>
#include <Xinput.h>
#include <cassert>
#include "GameScene.h"
#include <cmath>

Player::Player() {

}

Player::~Player() { 
	delete sprite2DReticle_;
}

void Player::Initialize(Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm, Model* modelWeapon, Model* modelBullet) {
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);
	assert(modelWeapon);
	assert(modelBullet);

	modelBody_ = modelBody;
	modelHead_ = modelHead;
	modelL_arm_ = modelL_arm;
	modelR_arm_ = modelR_arm;
	modelWeapon_ = modelWeapon;
	modelBullet_ = modelBullet;

	reticleTexture_ = TextureManager::Load("black.png");
	uint32_t textureReticle = TextureManager::Load("Reticle.png");

	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 320}, {1, 1, 1, 1}, {0.5f, 0.5f});

	input_ = Input::GetInstance();

	worldTransformL_arm_.translation_.x = -1.5f;
	worldTransformR_arm_.translation_.x = 1.5f;
	worldTransformL_arm_.translation_.y = 5.0f;
	worldTransformR_arm_.translation_.y = 5.0f;
	worldTransformWeapon_.translation_.x = 1.5f;
	worldTransformWeapon_.translation_.y = 2.0f;
	worldTransformWeapon_.translation_.z = 1.5f;

	SetRadius(2.0f);

	SetParent(&GetWorldTransformBody());

	InitializeFloatingGimmick();

	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformWeapon_.Initialize();

	spritePosition_ = sprite2DReticle_->GetPosition();
	worldTransform3DReticle_.Initialize();
}

void Player::Update() { 
	XINPUT_STATE joyState;

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	Create3DReticle();
	Create2DReticle(*viewProjection_);

	worldTransform3DReticle_.UpdateMatrix();

	Attack();
	BehaviorRootUpdate();

	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();
}

void Player::DrawUI() { 
	sprite2DReticle_->Draw();
}

void Player::Draw(ViewProjection& viewProjection) {
	modelBody_->Draw(worldTransformBody_, viewProjection);
	modelHead_->Draw(worldTransformHead_, viewProjection);
	modelL_arm_->Draw(worldTransformL_arm_, viewProjection);
	modelR_arm_->Draw(worldTransformR_arm_, viewProjection);
	modelWeapon_->Draw(worldTransformWeapon_, viewProjection);
	modelBullet_->Draw(worldTransform3DReticle_, viewProjection);
}

bool Player::UnCollision() {
	return false;
}

bool Player::OnCollision() { 
	velocity_ = Multiply(-1.0f, velocity_);

	worldTransformBase_.translation_ = Add(velocity_, worldTransformBase_.translation_);
	worldTransformBase_.UpdateMatrix();

	isDead_ = true;

	return true;
}

Vector3 Player::GetWorldPosition() { 
	Vector3 worldPosition;

	worldPosition.x = worldTransformBody_.matWorld_.m[3][0];
	worldPosition.y = worldTransformBody_.matWorld_.m[3][1];
	worldPosition.z = worldTransformBody_.matWorld_.m[3][2];

	return worldPosition;
}

Vector3 Player::GetWorld3DReticlePosition() { 
	Vector3 worldPosition;

	worldPosition.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPosition.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPosition.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPosition;
}

void Player::SetWorldPosition(Vector3 prePosition) {
	worldTransformBase_.matWorld_.m[3][0] = prePosition.x;
	worldTransformBase_.matWorld_.m[3][1] = prePosition.y;
	worldTransformBase_.matWorld_.m[3][2] = prePosition.z;
	worldTransformBody_.matWorld_.m[3][0] = prePosition.x;
	worldTransformBody_.matWorld_.m[3][1] = prePosition.y;
	worldTransformBody_.matWorld_.m[3][2] = prePosition.z;
}

void Player::SetParent(const WorldTransform* parent) { 
	worldTransformBase_.parent_ = parent;
	worldTransformHead_.parent_ = parent;
	worldTransformL_arm_.parent_ = parent;
	worldTransformR_arm_.parent_ = parent;
	worldTransformWeapon_.parent_ = parent;
}

void Player::InitializeFloatingGimmick() { 
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() { 
	const uint16_t T = 120;
	const float step = 2.0f * (float)M_PI / T;
	
	floatingParameter_ += step;
	floatingParameter_ = (float)std::fmod(floatingParameter_, 2.0f * M_PI);

	const float floatingAmplitude = 0.2f;

	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude;

	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_) * 0.75f;
	worldTransformR_arm_.rotation_.x = std::sin(floatingParameter_) * 0.75f;
}

void Player::BehaviorRootInitialize() {
	worldTransformBase_.rotation_.y = bulletImpactPoint_; 
}

void Player::BehaviorRootUpdate() { 
	Matrix4x4 rotateMatrix{};
	Vector3 move{};
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float speed = 0.7f;
		const float threshold = 0.7f;

		bool isAimMove = false;

		move = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};

		if (Length(move) > threshold) {
			isAimMove = true;
		}

		if (isAimMove) {
			move = Multiply(speed, Normalize(move));

			rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
			move = TransformNormal(move, rotateMatrix);
			move.y = 0;

			velocity_ = move;

			worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, move);
			worldTransformBody_.translation_ = worldTransformBase_.translation_;

			bulletImpactPoint_ = std::atan2(move.x, move.z);
		}
	}

	worldTransformBase_.rotation_.y =
	    Alerp(worldTransformBase_.rotation_.y, bulletImpactPoint_, 0.1f);

	worldTransformBody_.rotation_.y =
	    Alerp(worldTransformBody_.rotation_.y, bulletImpactPoint_, 0.1f);

	UpdateFloatingGimmick();
}

void Player::Attack() { 
	XINPUT_STATE joyState;

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		worldTransformBase_.rotation_ = viewProjection_->rotation_;
		worldTransformBase_.rotation_.x = 0;
		worldTransformBody_.rotation_ = viewProjection_->rotation_;
		worldTransformBody_.rotation_.x = 0;

		const float kBulletSpeed = 6.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		Vector3 worldPosition;
		worldPosition.x = worldTransformWeapon_.matWorld_.m[3][0];
		worldPosition.y = worldTransformWeapon_.matWorld_.m[3][1];
		worldPosition.z = worldTransformWeapon_.matWorld_.m[3][2];

		velocity = TransformNormal(velocity, worldTransformBase_.matWorld_);

		Vector3 worldReticlePosition = {
		    worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		    worldTransform3DReticle_.matWorld_.m[3][2]};

		velocity = Subtract(worldReticlePosition, worldPosition);
		velocity = Normalize(velocity);

		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, worldPosition, velocity);

		gameScene_->AddPlayerBullet(newBullet);
	}
}

void Player::Create3DReticle() {
	
	const float kDistancePlayerTo3DReticle = 200.0f;
	
	Vector3 offset{0, 0, 1.0f};
	
	offset = TransformNormal(offset, worldTransformBase_.matWorld_);
	offset = Normalize(offset);
	
	offset.x *= kDistancePlayerTo3DReticle;
	offset.y *= kDistancePlayerTo3DReticle;
	offset.z *= kDistancePlayerTo3DReticle;

	worldTransform3DReticle_.translation_.x = GetWorldPosition().x + offset.x;
	worldTransform3DReticle_.translation_.y = GetWorldPosition().y + offset.y;
	worldTransform3DReticle_.translation_.z = GetWorldPosition().z + offset.z;

	worldTransform3DReticle_.UpdateMatrix();
}

void Player::Create2DReticle(const ViewProjection& viewProjection) {
	Vector3 positionReticle = GetWorld3DReticlePosition();
	
	matViewPort_ = MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
	    Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewPort_));
	
	positionReticle = Transform(positionReticle, matViewProjectionViewport);
	
	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	XINPUT_STATE joyState;
	Vector2 joyRange{};
	
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		joyRange.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 0.02f;
		joyRange.y += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 0.04f;
		spritePosition_.x += joyRange.x;
		spritePosition_.y -= joyRange.y;
	
		sprite2DReticle_->SetPosition(spritePosition_);
	}

	Matrix4x4 matViewport =
	    MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	
	Matrix4x4 matVPV =
	    Multiply(viewProjection_->matView, Multiply(viewProjection_->matProjection, matViewport));
	
	Matrix4x4 matInverseVPV = Inverse(matVPV);
	
	Matrix4x4 checkInverse = Multiply(matInverseVPV, matVPV);

	Vector3 posNear = Vector3(
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 0);
	Vector3 posFar = Vector3(
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 1);

	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);
	
	Vector3 mouseDirection = Subtract(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	
	const float kDistanceTestObject = 200.0f;
	
	worldTransform3DReticle_.translation_.x = posNear.x + mouseDirection.x * kDistanceTestObject;
	worldTransform3DReticle_.translation_.y = posNear.y + mouseDirection.y * kDistanceTestObject;
	worldTransform3DReticle_.translation_.z = posNear.z + mouseDirection.z * kDistanceTestObject;
}