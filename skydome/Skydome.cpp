#include "Skydome.h"
#include <cassert>

void SkyDome::Initialize(Model* model, const Vector3& position) { 
	assert(model);
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {300.0f, 300.0f, 300.0f};
}

void SkyDome::Update() { 
	worldTransform_.UpdateMatrix();
	worldTransform_.TransferMatrix();
}

void SkyDome::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}