#pragma once

#include "ViewProjection.h"
#include "WorldTransform.h"

class FollowCamera {
public:
	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);

	const ViewProjection& GetViewProjection() { return viewProjection_; }

	float GetDestinationAngleY() { return destinationAngleY_; }

	Vector3 TargetOffset() const;

	void Reset();

private:
	ViewProjection viewProjection_;
	const WorldTransform* target_ = nullptr;
	Vector3 interTarget_ = {};
	float destinationAngleY_ = 0.0f;
};