#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

#include <memory>

class SkyDome {
public:
	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
};