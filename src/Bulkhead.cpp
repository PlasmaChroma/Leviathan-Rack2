#include "Bulkhead.hpp"

namespace {
constexpr float SPEED_OF_SOUND_MPS = 343.f;

inline float cvToUnit(float v) {
	return clamp(v / 5.f, -1.f, 1.f);
}

inline float mapUnit(float t, float lo, float hi) {
	const float n = 0.5f * (t + 1.f);
	return lo + n * (hi - lo);
}

inline float jsonFloatOr(json_t* rootJ, const char* key, float fallback) {
	if (!rootJ) {
		return fallback;
	}
	json_t* valueJ = json_object_get(rootJ, key);
	if (!valueJ || !json_is_number(valueJ)) {
		return fallback;
	}
	return float(json_number_value(valueJ));
}

inline int clampDelaySamples(int delaySamples, int maxDelaySamples) {
	return clamp(delaySamples, 1, maxDelaySamples);
}

inline float softClip(float x) {
	return x / (1.f + std::fabs(x) * 0.35f);
}

} // namespace

void Bulkhead::DelayLine::resize(int size) {
	const int safeSize = std::max(size, 2);
	buffer.assign(static_cast<size_t>(safeSize), 0.f);
	writeIndex = 0;
}

float Bulkhead::DelayLine::readDelay(int delaySamples) const {
	if (buffer.empty()) {
		return 0.f;
	}
	const int size = static_cast<int>(buffer.size());
	const int clampedDelay = clamp(delaySamples, 1, size - 1);
	int index = writeIndex - clampedDelay;
	while (index < 0) {
		index += size;
	}
	return buffer[static_cast<size_t>(index)];
}

void Bulkhead::DelayLine::writeSample(float v) {
	if (buffer.empty()) {
		return;
	}
	buffer[static_cast<size_t>(writeIndex)] = v;
	writeIndex++;
	if (writeIndex >= static_cast<int>(buffer.size())) {
		writeIndex = 0;
	}
}

float Bulkhead::CombFilter::process(float in, int delaySamples) {
	const float delayed = line.readDelay(delaySamples);
	filterStore = delayed + (filterStore - delayed) * damp;
	line.writeSample(in + filterStore * feedback);
	return delayed;
}

float Bulkhead::AllpassFilter::process(float in, int delaySamples) {
	const float delayed = line.readDelay(delaySamples);
	const float y = -in + delayed;
	line.writeSample(in + delayed * feedback);
	return y;
}

Bulkhead::Bulkhead() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	resetSceneDefaults();

	configParam(DECAY_PARAM, 0.1f, 30.f, 2.5f, "Decay", " s");
	configParam(DIFFUSE_PARAM, 0.f, 1.f, 0.55f, "Diffuse");
	configParam(MIX_PARAM, 0.f, 1.f, 0.35f, "Mix");
	configParam(ABSORB_PARAM, 0.f, 1.f, 0.35f, "Absorb");
	configParam(EARLY_LATE_PARAM, -1.f, 1.f, 0.f, "Early/Late");
	configParam(MOTION_PARAM, 0.f, 1.f, 0.15f, "Motion");

	configInput(LST_X_INPUT, "Listener X");
	configInput(LST_Y_INPUT, "Listener Y");
	configInput(WALL_LEFT_INPUT, "Left wall distance");
	configInput(WALL_RIGHT_INPUT, "Right wall distance");
	configInput(WALL_FRONT_INPUT, "Front wall distance");
	configInput(WALL_BACK_INPUT, "Back wall distance");
	configInput(IN_L_INPUT, "In L");
	configInput(IN_R_INPUT, "In R");

	configOutput(OUT_L_OUTPUT, "Out L");
	configOutput(OUT_R_OUTPUT, "Out R");

	sampleRate = APP->engine->getSampleRate();
	initDsp();
}

void Bulkhead::resetSceneDefaults() {
	room.left = -4.f;
	room.right = 4.f;
	room.bottom = -2.5f;
	room.top = 2.5f;
	speakerLeft.x = room.left + 1.2f;
	speakerLeft.y = room.top - 0.8f;
	speakerRight.x = room.right - 1.2f;
	speakerRight.y = room.top - 0.8f;
	listener.x = 0.5f * (room.left + room.right);
	listener.y = 0.5f * (room.bottom + room.top);
	listenerYawRadians = 0.5f * M_PI;
}

void Bulkhead::initDsp() {
	const int maxDelaySamples = std::max(4096, static_cast<int>(sampleRate * 2.5f));
	for (auto& c : combL) {
		c.line.resize(maxDelaySamples);
		c.feedback = 0.76f;
		c.damp = 0.22f;
		c.filterStore = 0.f;
	}
	for (auto& c : combR) {
		c.line.resize(maxDelaySamples);
		c.feedback = 0.79f;
		c.damp = 0.24f;
		c.filterStore = 0.f;
	}
	for (auto& ap : allpassL) {
		ap.line.resize(maxDelaySamples);
		ap.feedback = 0.5f;
	}
	for (auto& ap : allpassR) {
		ap.line.resize(maxDelaySamples);
		ap.feedback = 0.5f;
	}
	wetPostLpL = 0.f;
	wetPostLpR = 0.f;
}

void Bulkhead::onSampleRateChange() {
	sampleRate = APP->engine->getSampleRate();
	initDsp();
}

void Bulkhead::onReset() {
	resetSceneDefaults();
}

void Bulkhead::process(const ProcessArgs&) {
	listenerYawRadians = 0.5f * M_PI;
	const float inL = inputs[IN_L_INPUT].getVoltage();
	const float inR = inputs[IN_R_INPUT].isConnected() ? inputs[IN_R_INPUT].getVoltage() : inL;

	// Geometry/CV scaffold: keep listener + room state responsive while DSP is still minimal.
	if (inputs[LST_X_INPUT].isConnected()) {
		listener.x = mapUnit(cvToUnit(inputs[LST_X_INPUT].getVoltage()), room.left, room.right);
	}
	if (inputs[LST_Y_INPUT].isConnected()) {
		listener.y = mapUnit(cvToUnit(inputs[LST_Y_INPUT].getVoltage()), room.bottom, room.top);
	}

	const float minWallGap = 0.5f;
	const float leftMod = inputs[WALL_LEFT_INPUT].isConnected() ? inputs[WALL_LEFT_INPUT].getVoltage() * 0.1f : 0.f;
	const float rightMod = inputs[WALL_RIGHT_INPUT].isConnected() ? inputs[WALL_RIGHT_INPUT].getVoltage() * 0.1f : 0.f;
	const float frontMod = inputs[WALL_FRONT_INPUT].isConnected() ? inputs[WALL_FRONT_INPUT].getVoltage() * 0.1f : 0.f;
	const float backMod = inputs[WALL_BACK_INPUT].isConnected() ? inputs[WALL_BACK_INPUT].getVoltage() * 0.1f : 0.f;

	room.left = std::min(room.left + leftMod, listener.x - minWallGap);
	room.right = std::max(room.right + rightMod, listener.x + minWallGap);
	room.top = std::max(room.top + frontMod, listener.y + minWallGap);
	room.bottom = std::min(room.bottom + backMod, listener.y - minWallGap);

	const float decaySec = params[DECAY_PARAM].getValue();
	const float diffuse = params[DIFFUSE_PARAM].getValue();
	const float mix = params[MIX_PARAM].getValue();
	const float absorb = params[ABSORB_PARAM].getValue();
	const float earlyLate = params[EARLY_LATE_PARAM].getValue();
	const float motion = params[MOTION_PARAM].getValue();

	const float monoIn = 0.5f * (inL + inR);
	const auto reflL = bulkhead::geometry::firstOrderReflectionDistances(room, speakerLeft, listener);
	const auto reflR = bulkhead::geometry::firstOrderReflectionDistances(room, speakerRight, listener);
	const float roomExtentX = std::max(1.f, room.right - room.left);
	const float roomExtentY = std::max(1.f, room.top - room.bottom);
	const float roomScale = std::sqrt(roomExtentX * roomExtentY) * 0.25f;
	const float absorbGain = 1.f - 0.75f * absorb;

	float earlyL = 0.f;
	float earlyR = 0.f;
	for (int i = 0; i < bulkhead::geometry::WALL_COUNT; ++i) {
		const float distL = std::max(0.1f, reflL[static_cast<size_t>(i)] * roomScale);
		const float distR = std::max(0.1f, reflR[static_cast<size_t>(i)] * roomScale);
		const float delaySecL = distL / SPEED_OF_SOUND_MPS;
		const float delaySecR = distR / SPEED_OF_SOUND_MPS;
		const int dL = clampDelaySamples(static_cast<int>(delaySecL * sampleRate), static_cast<int>(combL[0].line.buffer.size()) - 1);
		const int dR = clampDelaySamples(static_cast<int>(delaySecR * sampleRate), static_cast<int>(combR[0].line.buffer.size()) - 1);
		const float gL = absorbGain / (1.f + 0.22f * distL);
		const float gR = absorbGain / (1.f + 0.22f * distR);
		earlyL += combL[0].line.readDelay(dL) * gL;
		earlyR += combR[0].line.readDelay(dR) * gR;
	}
	earlyL *= 0.25f;
	earlyR *= 0.25f;

	combL[0].line.writeSample(monoIn);
	combR[0].line.writeSample(monoIn);

	const float decayNorm = clamp((decaySec - 0.1f) / 29.9f, 0.f, 1.f);
	const float feedbackBase = 0.58f + 0.30f * decayNorm;
	const float dampBase = 0.12f + 0.55f * absorb;
	const float motionDepth = 1.f + motion * 0.08f;

	static const int combBaseL[COMB_COUNT] = { 1116, 1188, 1277, 1356 };
	static const int combBaseR[COMB_COUNT] = { 1139, 1211, 1300, 1379 };

	float lateL = 0.f;
	float lateR = 0.f;
	for (int i = 0; i < COMB_COUNT; ++i) {
		combL[static_cast<size_t>(i)].feedback = feedbackBase;
		combR[static_cast<size_t>(i)].feedback = feedbackBase * 0.995f;
		combL[static_cast<size_t>(i)].damp = dampBase;
		combR[static_cast<size_t>(i)].damp = dampBase;
		const int delayL = clampDelaySamples(static_cast<int>(combBaseL[i] * motionDepth * (1.f + 0.04f * diffuse)), static_cast<int>(combL[i].line.buffer.size()) - 1);
		const int delayR = clampDelaySamples(static_cast<int>(combBaseR[i] * motionDepth * (1.f + 0.04f * diffuse)), static_cast<int>(combR[i].line.buffer.size()) - 1);
		lateL += combL[static_cast<size_t>(i)].process(monoIn + 0.15f * earlyL, delayL);
		lateR += combR[static_cast<size_t>(i)].process(monoIn + 0.15f * earlyR, delayR);
	}
	lateL *= 0.24f;
	lateR *= 0.24f;

	const int ap0 = clampDelaySamples(static_cast<int>(225 * (1.f + 0.10f * diffuse)), static_cast<int>(allpassL[0].line.buffer.size()) - 1);
	const int ap1 = clampDelaySamples(static_cast<int>(556 * (1.f + 0.08f * diffuse)), static_cast<int>(allpassL[1].line.buffer.size()) - 1);
	lateL = allpassL[0].process(lateL, ap0);
	lateL = allpassL[1].process(lateL, ap1);
	lateR = allpassR[0].process(lateR, ap0 + 17);
	lateR = allpassR[1].process(lateR, ap1 + 19);

	const float earlyWeight = clamp(0.5f - 0.5f * earlyLate, 0.f, 1.f);
	const float lateWeight = clamp(0.5f + 0.5f * earlyLate, 0.f, 1.f);
	float wetL = earlyL * earlyWeight + lateL * lateWeight;
	float wetR = earlyR * earlyWeight + lateR * lateWeight;

	// Post-wet tone smoothing to reduce harsh comb resonances at high mix/decay.
	const float toneAmount = 0.10f + 0.35f * absorb + 0.10f * diffuse;
	wetPostLpL += (wetL - wetPostLpL) * toneAmount;
	wetPostLpR += (wetR - wetPostLpR) * toneAmount;
	const float toneBlend = 0.45f + 0.40f * absorb;
	wetL = crossfade(wetL, wetPostLpL, toneBlend);
	wetR = crossfade(wetR, wetPostLpR, toneBlend);

	// Keep wet energy controlled when decay and diffusion are high.
	const float wetTrim = 0.72f - 0.16f * decayNorm - 0.08f * diffuse;
	wetL *= wetTrim;
	wetR *= wetTrim;

	// Equal-power dry/wet law to avoid level spikes near full-wet settings.
	const float mixAngle = mix * 0.5f * M_PI;
	const float dryGain = std::cos(mixAngle);
	const float wetGain = std::sin(mixAngle);
	float outL = dryGain * inL + wetGain * wetL;
	float outR = dryGain * inR + wetGain * wetR;

	outL = softClip(outL);
	outR = softClip(outR);
	outputs[OUT_L_OUTPUT].setVoltage(clamp(outL, -10.f, 10.f));
	outputs[OUT_R_OUTPUT].setVoltage(clamp(outR, -10.f, 10.f));
}

json_t* Bulkhead::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "roomLeft", json_real(room.left));
	json_object_set_new(rootJ, "roomRight", json_real(room.right));
	json_object_set_new(rootJ, "roomBottom", json_real(room.bottom));
	json_object_set_new(rootJ, "roomTop", json_real(room.top));
	json_object_set_new(rootJ, "listenerX", json_real(listener.x));
	json_object_set_new(rootJ, "listenerY", json_real(listener.y));
	json_object_set_new(rootJ, "speakerLeftX", json_real(speakerLeft.x));
	json_object_set_new(rootJ, "speakerLeftY", json_real(speakerLeft.y));
	json_object_set_new(rootJ, "speakerRightX", json_real(speakerRight.x));
	json_object_set_new(rootJ, "speakerRightY", json_real(speakerRight.y));
	json_object_set_new(rootJ, "listenerYawRadians", json_real(listenerYawRadians));
	return rootJ;
}

void Bulkhead::dataFromJson(json_t* rootJ) {
	room.left = jsonFloatOr(rootJ, "roomLeft", room.left);
	room.right = jsonFloatOr(rootJ, "roomRight", room.right);
	room.bottom = jsonFloatOr(rootJ, "roomBottom", room.bottom);
	room.top = jsonFloatOr(rootJ, "roomTop", room.top);
	listener.x = jsonFloatOr(rootJ, "listenerX", listener.x);
	listener.y = jsonFloatOr(rootJ, "listenerY", listener.y);
	speakerLeft.x = jsonFloatOr(rootJ, "speakerLeftX", speakerLeft.x);
	speakerLeft.y = jsonFloatOr(rootJ, "speakerLeftY", speakerLeft.y);
	speakerRight.x = jsonFloatOr(rootJ, "speakerRightX", speakerRight.x);
	speakerRight.y = jsonFloatOr(rootJ, "speakerRightY", speakerRight.y);
	listenerYawRadians = 0.5f * M_PI;
}
