#pragma once

#include "ChronomawState.hpp"

namespace chronomaw {

struct FrameInputs {
	float sampleTime = 1.f / 44100.f;
	bool clkConnected = false;
	float clkVoltage = 0.f;
	bool runConnected = false;
	float runVoltage = 0.f;
	bool resetConnected = false;
	float resetVoltage = 0.f;
};

struct FrameOutputs {
	std::array<float, kNumOutputs> outVolts {};
};

class Engine {
public:
	void reset();
	void process(const FrameInputs& in, LiveState& live, FrameOutputs* out);

private:
	float phaseBeats_ = 0.f;
	float currentGate_ = 0.f;
};

} // namespace chronomaw

