#include "Ground.h"
#include <cassert>

void Ground::Initialize(Model* model, const Vector3& position) { 
	assert(model);
	model_ = model;

	worldTranform_.Initialize();
	worldTranform_.translation_ = position;
	worldTranform_.scale_ = {400.0f, 400.0f, 400.0f};
}

void Ground::Update() { 
	worldTranform_.UpdateMatrix();
	worldTranform_.TransferMatrix();
}

void Ground::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTranform_, viewProjection);
}