#pragma once

#include "plugin.hpp"
#include "MathHelpers.hpp"

#include <array>
#include <atomic>
#include <cmath>
#include <limits>
#include <memory>

constexpr int kWyrmPointCountDefault = 128;
constexpr int kWyrmPointCountMax = 256;
constexpr int kWyrmTableSize = 2048;
constexpr int kWyrmTableMipLevels = 8;
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
	SHAPE_COUNT
};

enum WyrmRockMouseMode {
	ROCK_MOUSE_DRAGS = 0,
	ROCK_MOUSE_LIFTS,
};

enum WyrmSandBackend {
	WYRMSAND_NANOVG_CELLS = 0,
	WYRMSAND_NANOVG_IMAGE,
	WYRMSAND_OPENGL_TEXTURE,
	WYRMSAND_SHADER_FEEDBACK,
};

enum WyrmRenderMode {
	WYRM_RENDER_NANOVG = 0,
	WYRM_RENDER_OPENGL,
	WYRM_RENDER_OPENGL_SHDR,
};

enum WyrmSandDetail {
	WYRMSAND_DETAIL_LOW = 0,
	WYRMSAND_DETAIL_MEDIUM,
	WYRMSAND_DETAIL_HIGH,
	WYRMSAND_DETAIL_AUTO,
};

enum WyrmSandPersistence {
	WYRMSAND_PERSISTENCE_SHORT = 0,
	WYRMSAND_PERSISTENCE_MEDIUM,
	WYRMSAND_PERSISTENCE_LONG,
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

inline float hashUnit(uint32_t seed) {
	seed ^= seed >> 16;
	seed *= 0x7feb352du;
	seed ^= seed >> 15;
	seed *= 0x846ca68bu;
	seed ^= seed >> 16;
	return float(seed & 0x00ffffffu) / float(0x01000000u);
}

inline float wyrmBaseFrequencyFromKnob(float knobNorm, bool lfoMode) {
	const float minFreq = lfoMode ? kWyrmLfoMinHz : kWyrmAudioMinHz;
	const float maxFreq = lfoMode ? kWyrmLfoMaxHz : kWyrmAudioMaxHz;
	return minFreq * std::pow(maxFreq / minFreq, levi_math::clamp01(knobNorm));
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
	const float p = levi_math::wrap01(phase) * float(count);
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
	const float cubic = 0.5f * ((2.f * p1) + (-p0 + p2) * t + (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 + (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3);

	// Catmull-Rom can ring/overshoot on steep adjacent steps, creating a
	// pre-spike in the opposite direction. Clamp risky segments to the local
	// endpoint range so jumps stay clean while keeping cubic shape elsewhere.
	const float d01 = p1 - p0;
	const float d12 = p2 - p1;
	const float d23 = p3 - p2;
	const bool hasTurnOrStep = (d01 * d12 <= 0.f) || (d12 * d23 <= 0.f);
	if (hasTurnOrStep) {
		return clamp(cubic, std::min(p1, p2), std::max(p1, p2));
	}
	return cubic;
}

inline float slitherOffset(float phase, float travelPhase, float amount) {
	const float shapedAmount = amount * amount;
	return kWyrmSlitherMaxOffset * shapedAmount * std::sin(2.f * float(M_PI) * (levi_math::wrap01(phase) - levi_math::wrap01(travelPhase)));
}

inline float slitherSpeedFactor(float speedKnob) {
	static std::array<float, 512> lut = []() {
		std::array<float, 512> t {};
		for (size_t i = 0; i < t.size(); ++i) {
			const float knob = float(i) / float(t.size() - 1u);
			const float expArg = (knob - 0.5f) * 4.f;
			t[i] = std::pow(2.f, expArg);
		}
		return t;
	}();
	const float k = levi_math::clamp01(speedKnob) * float(lut.size() - 1u);
	const int i0 = clamp(int(std::floor(k)), 0, int(lut.size() - 1u));
	const int i1 = std::min(i0 + 1, int(lut.size() - 1u));
	const float t = k - float(i0);
	return std::fma((lut[size_t(i1)] - lut[size_t(i0)]), t, lut[size_t(i0)]);
}

struct Wyrm;
struct WyrmSand;

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

struct WyrmRockStateSnapshot {
	int rockCount = 0;
	int liftedRock = -1;
	std::array<WyrmRock, kWyrmMaxRocks> rocks {};
	std::array<WyrmRockBoundaryCache, kWyrmMaxRocks> rockBoundaryCaches {};
	std::array<float, kWyrmMaxRocks> wrappedPhase {};
	std::array<float, kWyrmMaxRocks> defaultClearancePhase {};
	std::array<float, kWyrmMaxRocks> defaultRx {};
	std::array<float, kWyrmMaxRocks> defaultInvRx {};
	std::array<float, kWyrmMaxRocks> defaultRadiusValue {};
};

struct Wyrm : Module {
	ModuleTeardownTimer teardownTimer {"Wyrm"};
	enum ParamId {
		FREQ_PARAM,
		FINE_PARAM,
		FM_ATTEN_PARAM,
		FOLD_PARAM,
		SLITHER_PARAM,
		SLITHER_SPEED_PARAM,
		WAVE_LEFT_PARAM,
		WAVE_RIGHT_PARAM,
		LFO_MODE_PARAM,
		SYNC_MODE_PARAM,
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
		LFO_MODE_LIGHT,
		SYNC_MODE_LIGHT,
		LIGHTS_LEN
	};

	std::array<std::atomic<float>, kWyrmPointCountMax> wavePoints {};
	std::array<std::array<float, kWyrmTableSize>, kWyrmTableMipLevels> wavetableMip {};
	std::atomic<uint32_t> waveVersion {1};
	uint32_t appliedWaveVersion = 0;
	std::atomic<float> displayFrequencyHz {0.f};
	// Rate-limited channel-one state for the low-frequency waveform tracer.
	std::atomic<float> displayPhase {0.f};
	std::atomic<float> displayPhaseFrequencyHz {0.f};
	std::atomic<float> displaySlitherPhase {0.f};
	std::atomic<float> uiSlitherPhase {0.f};
	std::atomic<float> displaySlitherAmount {0.f};
	std::atomic<float> displaySlitherSpeedFactor {1.f};
	std::array<float, kWyrmMaxChannels> phase {};
	std::array<float, kWyrmMaxChannels> slitherPhase {};
	std::array<float, kWyrmMaxChannels> phaseDir {};
	std::array<dsp::SchmittTrigger, kWyrmMaxChannels> syncTriggers;

	std::atomic<bool> lfoMode {false};
	std::atomic<bool> editorLocked {false};
	std::atomic<bool> sandViewEnabled {false};
	std::atomic<int> renderMode {WYRM_RENDER_NANOVG};
	std::atomic<int> sandBackend {WYRMSAND_NANOVG_IMAGE};
	std::atomic<int> sandDetail {WYRMSAND_DETAIL_AUTO};
	std::atomic<int> sandPersistence {WYRMSAND_PERSISTENCE_MEDIUM};
	bool waveCustomized = false;
	int selectedShape = SHAPE_SINE;
	int pointCount = kWyrmPointCountDefault;
	int rockCount = 0;
	int rockMouseMode = ROCK_MOUSE_DRAGS;
	int liftedRock = -1;
	std::array<WyrmRock, kWyrmMaxRocks> rocks {};
	std::array<WyrmRockBoundaryCache, kWyrmMaxRocks> rockBoundaryCaches {};
	std::array<WyrmRockStateSnapshot, 2> activeRockState {};
	std::atomic<int> activeRockStateIndex {0};
	dsp::ClockDivider perfMeasureDivider;
	std::atomic<uint64_t> perfAudioSampledCount {0};
	std::atomic<uint64_t> perfAudioProcessNs {0};
	std::atomic<uint64_t> perfAudioProcessMinNs {std::numeric_limits<uint64_t>::max()};
	std::atomic<uint64_t> perfAudioProcessMaxNs {0};
	std::atomic<int> perfChannels {1};
	std::atomic<bool> perfFmConnected {false};
	std::atomic<bool> perfFoldActive {false};
	std::atomic<bool> perfSlitherActive {false};
	std::atomic<bool> perfLfoMode {false};
	std::atomic<bool> perfWavetableRebuilt {false};
	std::atomic<float> perfSandGlUs {0.f};
	std::atomic<uint64_t> perfBodySampleCacheHits {0};
	std::atomic<uint64_t> perfBodySampleCacheMisses {0};
	float phaseTracerPublishTimer = 0.f;
	uint32_t debugInstanceId = 0;
	double createdUnixTimeSec = 0.0;

	Wyrm();
	~Wyrm() override;

	void placeRock(int index);
	void setRockCount(int count);
	void setWavePoint(int index, float value);
	float getWavePoint(int index) const;
	void setFactoryShape(int shapeId);
	void setPointCount(int newPointCount);
	void rebuildWavetable();
	float lookupWave(float ph, float phaseStep = 0.f) const;
	float rockDx(float ph, const WyrmRock& rock) const;
	float rockClearancePhase(const WyrmRock& rock) const;
	float rockClearancePhase(const WyrmRock& rock, float clearanceValue) const;
	float rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue = 0.f) const;
	float rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue, float clearancePhase) const;
	void rebuildRockBoundaryCache(int rockIndex);
	void rebuildAllRockBoundaryCaches();
	void publishRockState();
	const WyrmRockStateSnapshot& getActiveRockState() const;
	bool cachedRockBoundsAtPhase(int rockIndex, float ph, float* lower, float* upper) const;
	static bool cachedRockBoundsAtPhase(const WyrmRockStateSnapshot& state, int rockIndex, float ph, float* lower, float* upper);
	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float* lower, float* upper) const;
	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float clearanceValue, float* lower, float* upper) const;
	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float clearanceValue, float clearancePhase, float* lower, float* upper) const;
	bool segmentIntersectsRockBounds(const WyrmRock& rock, float ph0, float y0, float ph1, float y1, bool* preferUpper) const;
	void sculptWaveAroundRock(int rockIndex, const WyrmRock* previousRock = nullptr);
	float resolveAgainstRocks(float anchorY, float desiredY, float ph, float clearanceValue = kWyrmRockClearance) const;
	float resolveAgainstRocks(float anchorY, float desiredY, float ph, float clearanceValue, float clearancePhase) const;
	static float resolveAgainstRocks(const WyrmRockStateSnapshot& state, float anchorY, float desiredY, float ph, float clearanceValue, float clearancePhase = -1.f);
	float applyRockPush(float base, float ph) const;
	float applyRockClamp(float base, float ph, float offset) const;
	static float applyRockPush(const WyrmRockStateSnapshot& state, float base, float ph);
	static float applyRockClamp(const WyrmRockStateSnapshot& state, float base, float ph, float offset);
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void process(const ProcessArgs& args) override;
};

TransparentWidget* createWyrmWaveEditor(Wyrm* module);
TransparentWidget* createWyrmWaveEditor(Wyrm* module, std::shared_ptr<WyrmSand> sandState);
Widget* createWyrmSandGlWidget(Wyrm* module, std::shared_ptr<WyrmSand> sandState);
