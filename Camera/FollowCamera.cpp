#include "FollowCamera.h"
#include "MyMath.h"
#include <Input.h>
#include <Xinput.h>

void FollowCamera::Initialize() { 
	viewProjection_.Initialize(); 
}

void FollowCamera::Update() { 
	if (target_) {
		Vector3 offset = TargetOffset();
		interTarget_ = Lerp(interTarget_, target_->translation_, 0.1f);
		viewProjection_.translation_ = Add(interTarget_, offset);
	}

	XINPUT_STATE joyState;

if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float kRadian = 0.04f;
		if (viewProjection_.rotation_.x >= -0.26f && viewProjection_.rotation_.x <= 0.26f) {
			viewProjection_.rotation_.x -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 0.02f;
		}

		destinationAngleY_ += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kRadian;
	}

	viewProjection_.rotation_.y =
	    Alerp(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);

	if (viewProjection_.rotation_.x <= -0.26f) {
		viewProjection_.rotation_.x = -0.2599999f;
	} 
	else if (viewProjection_.rotation_.x >= 0.26f) {
		viewProjection_.rotation_.x = 0.2599999f;
	}

	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();
}

void FollowCamera::SetTarget(const WorldTransform* target) { 
	target_ = target;
	Reset();
}

Vector3 FollowCamera::TargetOffset() const {
	Vector3 offset = {0, 8, -30};
	Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::Reset() { 
	if (target_) {
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}

	destinationAngleY_ = viewProjection_.rotation_.y;

	Vector3 offset = TargetOffset();
	viewProjection_.translation_ = Add(interTarget_, offset);
}