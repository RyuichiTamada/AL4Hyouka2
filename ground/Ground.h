#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

#include <memory>

class Ground {
public:
	void Initialize(Model* mode, const Vector3& positionl);

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:
	WorldTransform worldTranform_;
	Model* model_ = nullptr;
};