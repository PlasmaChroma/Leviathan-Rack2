#pragma once

#include "plugin.hpp"

#include <array>
#include <atomic>
#include <cmath>

constexpr int kWyrmPointCountDefault = 64;
constexpr int kWyrmPointCountMax = 256;
constexpr int kWyrmTableSize = 2048;
constexpr int kWyrmMaxChannels = 16;
constexpr int kWyrmMaxRocks = 6;
constexpr int kWyrmRockBoundarySamples = 64;

enum WyrmShapeId {
	SHAPE_SINE = 0,
	SHAPE_TRIANGLE,
	SHAPE_SAW,
	SHAPE_REV_SAW,
	SHAPE_SQUARE,
	SHAPE_SUPERSAW,
	SHAPE_SUPERSAW_DOWN,
	SHAPE_COUNT
};

enum WyrmRockMouseMode {
	ROCK_MOUSE_DRAGS = 0,
	ROCK_MOUSE_LIFTS,
};

extern const char* const kWyrmShapeLabels[SHAPE_COUNT];

constexpr float kWyrmAudioMinHz = 9.99f;
constexpr float kWyrmAudioMaxHz = 9999.f;
constexpr float kWyrmLfoMinHz = 0.01f;
constexpr float kWyrmLfoMaxHz = 100.f;
// Precomputed 1 / tanh(1) so this remains a valid C++11 constant on libc++.
constexpr float kWyrmFoldMakeupGain = 1.3130352f;
constexpr float kWyrmSlitherMaxOffset = 0.42f;
constexpr float kWyrmRockClearance = 0.012f;
constexpr float kWyrmRockValueScale = 0.5f;

inline float clamp01(float x) {
	return clamp(x, 0.f, 1.f);
}

inline float wrap01(float x) {
	return x - std::floor(x);
}

inline float wrap01Fast(float x) {
	if (x >= 1.f) {
		x -= 1.f;
	}
	else if (x < 0.f) {
		x += 1.f;
	}
	return x;
}

inline float smoother01(float x) {
	x = clamp01(x);
	return x * x * (3.f - 2.f * x);
}

inline float hashUnit(uint32_t seed) {
	seed ^= seed >> 16;
	seed *= 0x7feb352du;
	seed ^= seed >> 15;
	seed *= 0x846ca68bu;
	seed ^= seed >> 16;
	return float(seed & 0x00ffffffu) / float(0x01000000u);
}

inline float fastTanh(float x) {
	const float x2 = x * x;
	if (x2 < 9.f) {
		return x * (27.f + x2) / (27.f + 9.f * x2);
	}
	return (x > 0.f) ? 1.f : -1.f;
}

inline float softClip(float x) {
	return fastTanh(x);
}

inline float wyrmBaseFrequencyFromKnob(float knobNorm, bool lfoMode) {
	const float minFreq = lfoMode ? kWyrmLfoMinHz : kWyrmAudioMinHz;
	const float maxFreq = lfoMode ? kWyrmLfoMaxHz : kWyrmAudioMaxHz;
	return minFreq * std::pow(maxFreq / minFreq, clamp01(knobNorm));
}

inline float wyrmKnobValueForFrequency(float hz, bool lfoMode) {
	const float minFreq = lfoMode ? kWyrmLfoMinHz : kWyrmAudioMinHz;
	const float maxFreq = lfoMode ? kWyrmLfoMaxHz : kWyrmAudioMaxHz;
	hz = clamp(hz, minFreq, maxFreq);
	return std::log(hz / minFreq) / std::log(maxFreq / minFreq);
}

inline float foldWave(float x, float amount) {
	if (amount <= 1e-5f) {
		return x;
	}
	const float drive = 1.f + 5.5f * amount;
	const float d = x * drive;
	return std::sin(0.5f * float(M_PI) * d);
}

inline float catmullPeriodic(const std::array<float, kWyrmPointCountMax>& points, int pointCount, float phase) {
	const int count = clamp(pointCount, 2, kWyrmPointCountMax);
	const float p = wrap01(phase) * float(count);
	const int i1 = int(std::floor(p)) % count;
	const int i0 = (i1 + count - 1) % count;
	const int i2 = (i1 + 1) % count;
	const int i3 = (i1 + 2) % count;
	const float t = p - std::floor(p);
	const float p0 = points[i0];
	const float p1 = points[i1];
	const float p2 = points[i2];
	const float p3 = points[i3];
	const float t2 = t * t;
	const float t3 = t2 * t;
	return 0.5f * ((2.f * p1) + (-p0 + p2) * t + (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 + (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3);
}

inline float slitherOffset(float phase, float travelPhase, float amount) {
	const float shapedAmount = amount * amount;
	return kWyrmSlitherMaxOffset * shapedAmount * std::sin(2.f * float(M_PI) * (wrap01(phase) - wrap01(travelPhase)));
}

inline float slitherSpeedFactor(float speedKnob) {
	return std::pow(2.f, (clamp01(speedKnob) - 0.5f) * 4.f);
}

struct Wyrm;

struct WyrmFreqQuantity final : ParamQuantity {
	float getDisplayValue() override;
	void setDisplayValue(float displayValue) override;
	std::string getDisplayValueString() override;
};

struct WyrmRock {
	float phase = 0.f;
	float value = 0.f;
	float radiusPhase = 0.045f;
	float radiusValue = 0.13f;
	uint32_t seed = 1u;
};

struct WyrmRockBoundaryCache {
	bool valid = false;
	float phase = 0.f;
	float value = 0.f;
	float radiusPhase = 0.f;
	float radiusValue = 0.f;
	std::array<float, kWyrmRockBoundarySamples> lower {};
	std::array<float, kWyrmRockBoundarySamples> upper {};
};

struct Wyrm : Module {
	enum ParamId {
		FREQ_PARAM,
		FINE_PARAM,
		FM_ATTEN_PARAM,
		FOLD_PARAM,
		SLITHER_PARAM,
		SLITHER_SPEED_PARAM,
		WAVE_LEFT_PARAM,
		WAVE_RIGHT_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VOCT_INPUT,
		FM_INPUT,
		SYNC_INPUT,
		FOLD_CV_INPUT,
		SLITHER_CV_INPUT,
		SLITHER_SPEED_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		RAW_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	std::array<std::atomic<float>, kWyrmPointCountMax> wavePoints {};
	std::array<float, kWyrmTableSize> wavetable {};
	std::atomic<uint32_t> waveVersion {1};
	uint32_t appliedWaveVersion = 0;
	std::atomic<float> displayFrequencyHz {0.f};
	std::array<float, kWyrmMaxChannels> phase {};
	std::array<float, kWyrmMaxChannels> slitherPhase {};
	std::array<dsp::SchmittTrigger, kWyrmMaxChannels> syncTriggers;

	bool lfoMode = false;
	bool editorLocked = false;
	bool waveCustomized = false;
	int selectedShape = SHAPE_SINE;
	int pointCount = kWyrmPointCountDefault;
	int rockCount = 0;
	int rockMouseMode = ROCK_MOUSE_DRAGS;
	int liftedRock = -1;
	std::array<WyrmRock, kWyrmMaxRocks> rocks {};
	std::array<WyrmRockBoundaryCache, kWyrmMaxRocks> rockBoundaryCaches {};
	double createdUnixTimeSec = 0.0;

	Wyrm();

	void placeRock(int index);
	void setRockCount(int count);
	void setWavePoint(int index, float value);
	float getWavePoint(int index) const;
	void setFactoryShape(int shapeId);
	void setPointCount(int newPointCount);
	void rebuildWavetable();
	float lookupWave(float ph) const;
	float rockDx(float ph, const WyrmRock& rock) const;
	float rockClearancePhase(const WyrmRock& rock) const;
	float rockClearancePhase(const WyrmRock& rock, float clearanceValue) const;
	float rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue = 0.f) const;
	float rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue, float clearancePhase) const;
	void rebuildRockBoundaryCache(int rockIndex);
	void rebuildAllRockBoundaryCaches();
	bool cachedRockBoundsAtPhase(int rockIndex, float ph, float* lower, float* upper) const;
	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float* lower, float* upper) const;
	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float clearanceValue, float* lower, float* upper) const;
	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float clearanceValue, float clearancePhase, float* lower, float* upper) const;
	bool segmentIntersectsRockBounds(const WyrmRock& rock, float ph0, float y0, float ph1, float y1, bool* preferUpper) const;
	void sculptWaveAroundRock(int rockIndex, const WyrmRock* previousRock = nullptr);
	float resolveAgainstRocks(float anchorY, float desiredY, float ph, float clearanceValue = kWyrmRockClearance) const;
	float resolveAgainstRocks(float anchorY, float desiredY, float ph, float clearanceValue, float clearancePhase) const;
	float applyRockPush(float base, float ph) const;
	float applyRockClamp(float base, float ph, float offset) const;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void process(const ProcessArgs& args) override;
};

TransparentWidget* createWyrmWaveEditor(Wyrm* module);
