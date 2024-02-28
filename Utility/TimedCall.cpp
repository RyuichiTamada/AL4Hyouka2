#include "TimedCall.h"

TimedCall::TimedCall(std::function<void()> callBack, uint32_t time) { 
	callBack_ = callBack;
	time_ = time;
}

void TimedCall::Update() { 
	if (isFinish_) {
		return;
	}

	time_--;

	if (time_ <= 0) {
		isFinish_ = true;
		callBack_();
	}
}