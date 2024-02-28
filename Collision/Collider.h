#pragma once

#include "Vector3.h"
#include <cstdint>

class Collider {
public:
	uint32_t GetCollisionAttribute() { return collisionAttribute_; }
	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; }

	uint32_t GetCollisionMask() { return collisionMask_; }
	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; }

	float GetRadius() { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }

	virtual bool OnCollision() = 0;
	virtual bool UnCollision() = 0;

	virtual Vector3 GetWorldPosition() = 0;

private:
	float radius_ = 1.0f;

	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t collisionMask_ = 0xffffffff;
};