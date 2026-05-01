#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <array>
#include <atomic>
#include <cmath>

namespace {

constexpr int kWyrmPointCountDefault = 32;
constexpr int kWyrmPointCountMax = 128;
constexpr int kWyrmTableSize = 2048;
constexpr int kWyrmMaxChannels = 16;
constexpr int kWyrmMaxRocks = 6;

enum WyrmShapeId {
	SHAPE_SINE = 0,
	SHAPE_TRIANGLE,
	SHAPE_SAW,
	SHAPE_REV_SAW,
	SHAPE_SQUARE,
	SHAPE_COUNT
};

enum WyrmRockMouseMode {
	ROCK_MOUSE_DRAGS = 0,
	ROCK_MOUSE_LIFTS,
};

const char* const kWyrmShapeLabels[SHAPE_COUNT] = {
	"Sine",
	"Triangle",
	"Saw",
	"Reverse Saw",
	"Square"
};

constexpr float kWyrmAudioMinHz = 20.f;
constexpr float kWyrmAudioMaxHz = 20000.f;
constexpr float kWyrmLfoMinHz = 0.01f;
constexpr float kWyrmLfoMaxHz = 100.f;
constexpr float kWyrmFoldMakeupGain = 1.0f / std::tanh(1.f);
constexpr float kWyrmSlitherMaxOffset = 0.42f;
constexpr float kWyrmRockClearance = 0.055f;

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
	// Midpoint (0.5) preserves previous baseline speed. Left slows, right speeds up.
	return std::pow(2.f, (clamp01(speedKnob) - 0.5f) * 4.f);
}

} // namespace

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

struct Wyrm : Module {
	enum ParamId {
		FREQ_PARAM,
		FINE_PARAM,
		FM_ATTEN_PARAM,
		FOLD_PARAM,
		SLITHER_PARAM,
		SLITHER_SPEED_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VOCT_INPUT,
		FM_INPUT,
		SYNC_INPUT,
		FOLD_CV_INPUT,
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
	std::array<float, kWyrmMaxChannels> phase {};
	std::array<float, kWyrmMaxChannels> slitherPhase {};
	std::array<dsp::SchmittTrigger, kWyrmMaxChannels> syncTriggers;

	bool lfoMode = false;
	bool editorLocked = false;
	int selectedShape = SHAPE_SINE;
	int pointCount = kWyrmPointCountDefault;
	int rockCount = 0;
	int rockMouseMode = ROCK_MOUSE_DRAGS;
	int liftedRock = -1;
	std::array<WyrmRock, kWyrmMaxRocks> rocks {};

	Wyrm() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam<WyrmFreqQuantity>(FREQ_PARAM, 0.f, 1.f, 0.45f, "Frequency");
		configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
		configParam(FM_ATTEN_PARAM, -1.f, 1.f, 0.f, "FM attenuator");
		configParam(FOLD_PARAM, 0.f, 1.f, 0.f, "Fold amount");
		configParam(SLITHER_PARAM, 0.f, 1.f, 0.f, "Slither", "%", 0.f, 100.f);
		configParam(SLITHER_SPEED_PARAM, 0.f, 1.f, 0.5f, "Slither speed");
		configInput(VOCT_INPUT, "V/Oct");
		configInput(FM_INPUT, "FM");
		configInput(SYNC_INPUT, "Sync");
		configInput(FOLD_CV_INPUT, "Fold CV");
		configOutput(OUT_OUTPUT, "Out");
		configOutput(RAW_OUTPUT, "Raw");

		setFactoryShape(SHAPE_SINE);
		for (int i = 0; i < kWyrmMaxRocks; ++i) {
			placeRock(i);
		}
	}

	void placeRock(int index) {
		if (index < 0 || index >= kWyrmMaxRocks) return;
		const uint32_t seed = 0x9e3779b9u + uint32_t(index) * 0x85ebca6bu;
		WyrmRock& rock = rocks[index];
		rock.seed = seed;
		rock.phase = 0.08f + 0.84f * hashUnit(seed ^ 0x31524u);
		rock.value = -0.72f + 1.44f * hashUnit(seed ^ 0x9ab31u);
		rock.radiusPhase = 0.035f + 0.02f * hashUnit(seed ^ 0x4c2du);
		rock.radiusValue = 0.105f + 0.055f * hashUnit(seed ^ 0x732u);
	}

	void setRockCount(int count) {
		const int oldCount = rockCount;
		rockCount = clamp(count, 0, kWyrmMaxRocks);
		if (liftedRock >= rockCount) {
			liftedRock = -1;
		}
		for (int i = oldCount; i < rockCount; ++i) {
			pushWavePointsOutsideRock(i);
		}
	}

	void setWavePoint(int index, float value) {
		if (index < 0 || index >= pointCount) {
			return;
		}
		wavePoints[index].store(clamp(value, -1.f, 1.f), std::memory_order_relaxed);
		waveVersion.fetch_add(1u, std::memory_order_release);
	}

	float getWavePoint(int index) const {
		if (index < 0 || index >= pointCount) {
			return 0.f;
		}
		return wavePoints[index].load(std::memory_order_relaxed);
	}

	void setFactoryShape(int shapeId) {
		shapeId = clamp(shapeId, 0, SHAPE_COUNT - 1);
		selectedShape = shapeId;
		for (int i = 0; i < pointCount; ++i) {
			const float p = float(i) / float(pointCount);
			float v = 0.f;
			switch (shapeId) {
				case SHAPE_SINE: v = std::sin(2.f * float(M_PI) * p); break;
				case SHAPE_TRIANGLE: {
					const float x = 2.f * std::fabs(2.f * p - 1.f) - 1.f;
					v = -x;
				} break;
				case SHAPE_SAW: v = 2.f * p - 1.f; break;
				case SHAPE_REV_SAW: v = 1.f - 2.f * p; break;
				case SHAPE_SQUARE: v = (p < 0.5f) ? 1.f : -1.f; break;
				default: break;
			}
			wavePoints[i].store(clamp(v, -1.f, 1.f), std::memory_order_relaxed);
		}
		waveVersion.fetch_add(1u, std::memory_order_release);
	}

	void setPointCount(int newPointCount) {
		newPointCount = clamp(newPointCount, 32, kWyrmPointCountMax);
		if (newPointCount != 32 && newPointCount != 48 && newPointCount != 64 && newPointCount != 128) {
			newPointCount = kWyrmPointCountDefault;
		}
		if (newPointCount == pointCount) {
			return;
		}
		pointCount = newPointCount;
		setFactoryShape(selectedShape);
	}

	void rebuildWavetable() {
		std::array<float, kWyrmPointCountMax> local {};
		for (int i = 0; i < pointCount; ++i) {
			local[i] = wavePoints[i].load(std::memory_order_relaxed);
		}
		float maxAbs = 1e-6f;
		for (int i = 0; i < kWyrmTableSize; ++i) {
			const float ph = float(i) / float(kWyrmTableSize);
			const float y = catmullPeriodic(local, pointCount, ph);
			wavetable[i] = y;
			maxAbs = std::max(maxAbs, std::fabs(y));
		}
		const float inv = 1.f / maxAbs;
		for (int i = 0; i < kWyrmTableSize; ++i) {
			wavetable[i] = clamp(wavetable[i] * inv, -1.f, 1.f);
		}
	}

	float lookupWave(float ph) const {
		float p = ph;
		if (p >= 1.f) {
			p -= 1.f;
		}
		else if (p < 0.f) {
			p += 1.f;
		}
		const float x = p * float(kWyrmTableSize);
		const int i0 = int(x);
		const int i1 = (i0 + 1 < kWyrmTableSize) ? (i0 + 1) : 0;
		const float t = x - float(i0);
		return std::fma((wavetable[i1] - wavetable[i0]), t, wavetable[i0]);
	}

	float rockDx(float ph, const WyrmRock& rock) const {
		float dx = wrap01(ph) - wrap01(rock.phase);
		if (dx > 0.5f) {
			dx -= 1.f;
		}
		else if (dx < -0.5f) {
			dx += 1.f;
		}
		return dx;
	}

	float rockClearancePhase(const WyrmRock& rock) const {
		return kWyrmRockClearance * rock.radiusPhase / std::max(rock.radiusValue, 1e-4f);
	}

	float rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue = 0.f) const {
		const float radiusPhase = rock.radiusPhase + ((clearanceValue > 0.f) ? rockClearancePhase(rock) : 0.f);
		const float radiusValue = rock.radiusValue + clearanceValue;
		if (std::fabs(dx) >= radiusPhase) {
			return 0.f;
		}
		const float nx = dx / std::max(radiusPhase, 1e-4f);
		return radiusValue * std::sqrt(std::max(0.f, 1.f - nx * nx));
	}

	bool rockBoundsAtPhase(const WyrmRock& rock, float ph, float* lower, float* upper) const {
		const float edgeY = rockEdgeY(rock, rockDx(ph, rock), kWyrmRockClearance);
		if (edgeY <= 0.f) {
			return false;
		}
		if (lower) *lower = rock.value - edgeY;
		if (upper) *upper = rock.value + edgeY;
		return true;
	}

	bool pushPointOutsideRock(int pointIndex, const WyrmRock& rock, bool preferUpper, bool forceSide) {
		if (pointIndex < 0 || pointIndex >= pointCount) return false;
		const float ph = (float(pointIndex) + 0.5f) / float(pointCount);
		float lower = 0.f;
		float upper = 0.f;
		if (!rockBoundsAtPhase(rock, ph, &lower, &upper)) {
			return false;
		}
		const float base = wavePoints[pointIndex].load(std::memory_order_relaxed);
		if (!forceSide && (base <= lower || base >= upper)) {
			return false;
		}
		float pushed = 0.f;
		if (forceSide) {
			pushed = preferUpper ? upper : lower;
			if ((preferUpper && base >= pushed) || (!preferUpper && base <= pushed)) {
				return false;
			}
		}
		else {
			pushed = (std::fabs(base - lower) < std::fabs(upper - base)) ? lower : upper;
		}
		wavePoints[pointIndex].store(clamp(pushed, -1.f, 1.f), std::memory_order_relaxed);
		return true;
	}

	bool segmentIntersectsRockBounds(const WyrmRock& rock, float ph0, float y0, float ph1, float y1, bool* preferUpper) const {
		const float rx = rock.radiusPhase + rockClearancePhase(rock);
		const float ry = rock.radiusValue + kWyrmRockClearance;
		float x0 = rockDx(ph0, rock);
		float x1 = rockDx(ph1, rock);
		if (x1 - x0 > 0.5f) {
			x1 -= 1.f;
		}
		else if (x1 - x0 < -0.5f) {
			x1 += 1.f;
		}
		y0 -= rock.value;
		y1 -= rock.value;

		const float dx = x1 - x0;
		const float dy = y1 - y0;
		const float invRx = 1.f / std::max(rx, 1e-4f);
		const float invRy = 1.f / std::max(ry, 1e-4f);
		const float a = dx * dx * invRx * invRx + dy * dy * invRy * invRy;
		const float b = 2.f * (x0 * dx * invRx * invRx + y0 * dy * invRy * invRy);
		const float c = x0 * x0 * invRx * invRx + y0 * y0 * invRy * invRy - 1.f;
		const float t = (a > 1e-8f) ? clamp(-b / (2.f * a), 0.f, 1.f) : 0.f;
		const float closest = a * t * t + b * t + c;
		if (closest > 0.f) {
			return false;
		}
		if (preferUpper) {
			*preferUpper = (y0 + dy * t) >= 0.f;
		}
		return true;
	}

	void pushWavePointsOutsideRock(int rockIndex) {
		if (rockIndex < 0 || rockIndex >= rockCount) return;
		const WyrmRock& rock = rocks[rockIndex];
		bool changed = false;
		for (int i = 0; i < pointCount; ++i) {
			changed = pushPointOutsideRock(i, rock, false, false) || changed;
		}
		for (int i = 0; i < pointCount - 1; ++i) {
			const float ph0 = (float(i) + 0.5f) / float(pointCount);
			const float ph1 = (float(i + 1) + 0.5f) / float(pointCount);
			const float y0 = wavePoints[i].load(std::memory_order_relaxed);
			const float y1 = wavePoints[i + 1].load(std::memory_order_relaxed);
			bool preferUpper = false;
			if (segmentIntersectsRockBounds(rock, ph0, y0, ph1, y1, &preferUpper)) {
				changed = pushPointOutsideRock(i, rock, preferUpper, true) || changed;
				changed = pushPointOutsideRock(i + 1, rock, preferUpper, true) || changed;
			}
		}
		if (changed) {
			waveVersion.fetch_add(1u, std::memory_order_release);
		}
	}

	float applyRockClamp(float base, float ph, float offset) const {
		if (rockCount <= 0 || std::fabs(offset) <= 1e-6f) {
			return offset;
		}
		float clampedOffset = offset;
		for (int i = 0; i < rockCount; ++i) {
			if (i == liftedRock) continue;
			const WyrmRock& rock = rocks[i];
			const float dx = rockDx(ph, rock);
			const float dyCenter = rock.value - base;
			const float edgeY = rockEdgeY(rock, dx, kWyrmRockClearance);
			if (edgeY <= 0.f) continue;
			const float influence = smoother01(1.f - std::fabs(dx) / (rock.radiusPhase + rockClearancePhase(rock)));
			if (dyCenter > 0.f && clampedOffset > 0.f) {
				const float blocked = std::max(0.f, dyCenter - edgeY);
				clampedOffset = clampedOffset + influence * (std::min(clampedOffset, blocked) - clampedOffset);
			}
			else if (dyCenter < 0.f && clampedOffset < 0.f) {
				const float blocked = std::min(0.f, dyCenter + edgeY);
				clampedOffset = clampedOffset + influence * (std::max(clampedOffset, blocked) - clampedOffset);
			}
		}
		return clampedOffset;
	}

	json_t* dataToJson() override {
		json_t* root = json_object();
		json_object_set_new(root, "lfoMode", json_boolean(lfoMode));
		json_object_set_new(root, "editorLocked", json_boolean(editorLocked));
		json_object_set_new(root, "selectedShape", json_integer(selectedShape));
		json_object_set_new(root, "pointCount", json_integer(pointCount));
		json_object_set_new(root, "rockCount", json_integer(rockCount));
		json_object_set_new(root, "rockMouseMode", json_integer(rockMouseMode));
		json_t* pts = json_array();
		for (int i = 0; i < pointCount; ++i) {
			json_array_append_new(pts, json_real(getWavePoint(i)));
		}
		json_object_set_new(root, "wavePoints", pts);
		json_t* rockArray = json_array();
		for (int i = 0; i < kWyrmMaxRocks; ++i) {
			json_t* rockJ = json_object();
			json_object_set_new(rockJ, "phase", json_real(rocks[i].phase));
			json_object_set_new(rockJ, "value", json_real(rocks[i].value));
			json_object_set_new(rockJ, "radiusPhase", json_real(rocks[i].radiusPhase));
			json_object_set_new(rockJ, "radiusValue", json_real(rocks[i].radiusValue));
			json_object_set_new(rockJ, "seed", json_integer(rocks[i].seed));
			json_array_append_new(rockArray, rockJ);
		}
		json_object_set_new(root, "rocks", rockArray);
		return root;
	}

	void dataFromJson(json_t* root) override {
		json_t* lfoJ = json_object_get(root, "lfoMode");
		if (lfoJ) lfoMode = json_is_true(lfoJ);
		json_t* lockJ = json_object_get(root, "editorLocked");
		if (lockJ) editorLocked = json_is_true(lockJ);
		json_t* shapeJ = json_object_get(root, "selectedShape");
		if (shapeJ) selectedShape = clamp(int(json_integer_value(shapeJ)), 0, SHAPE_COUNT - 1);
		json_t* pointCountJ = json_object_get(root, "pointCount");
		if (pointCountJ) {
			int loadedPointCount = int(json_integer_value(pointCountJ));
			if (loadedPointCount == 32 || loadedPointCount == 48 || loadedPointCount == 64 || loadedPointCount == 128) {
				pointCount = loadedPointCount;
			}
		}
		json_t* rockCountJ = json_object_get(root, "rockCount");
		if (rockCountJ) setRockCount(int(json_integer_value(rockCountJ)));
		json_t* rockMouseModeJ = json_object_get(root, "rockMouseMode");
		if (rockMouseModeJ) rockMouseMode = clamp(int(json_integer_value(rockMouseModeJ)), ROCK_MOUSE_DRAGS, ROCK_MOUSE_LIFTS);
		json_t* pts = json_object_get(root, "wavePoints");
		if (pts && json_is_array(pts)) {
			const size_t n = json_array_size(pts);
			for (int i = 0; i < pointCount && i < int(n); ++i) {
				json_t* v = json_array_get(pts, i);
				if (v) {
					wavePoints[i].store(clamp(float(json_number_value(v)), -1.f, 1.f), std::memory_order_relaxed);
				}
			}
			waveVersion.fetch_add(1u, std::memory_order_release);
		}
		json_t* rocksJ = json_object_get(root, "rocks");
		if (rocksJ && json_is_array(rocksJ)) {
			const size_t n = json_array_size(rocksJ);
			for (int i = 0; i < kWyrmMaxRocks && i < int(n); ++i) {
				json_t* rockJ = json_array_get(rocksJ, i);
				if (!rockJ) continue;
				json_t* phaseJ = json_object_get(rockJ, "phase");
				json_t* valueJ = json_object_get(rockJ, "value");
				json_t* radiusPhaseJ = json_object_get(rockJ, "radiusPhase");
				json_t* radiusValueJ = json_object_get(rockJ, "radiusValue");
				json_t* seedJ = json_object_get(rockJ, "seed");
				if (phaseJ) rocks[i].phase = wrap01(float(json_number_value(phaseJ)));
				if (valueJ) rocks[i].value = clamp(float(json_number_value(valueJ)), -1.f, 1.f);
				if (radiusPhaseJ) rocks[i].radiusPhase = clamp(float(json_number_value(radiusPhaseJ)), 0.02f, 0.09f);
				if (radiusValueJ) rocks[i].radiusValue = clamp(float(json_number_value(radiusValueJ)), 0.06f, 0.24f);
				if (seedJ) rocks[i].seed = uint32_t(json_integer_value(seedJ));
			}
		}
	}

	void process(const ProcessArgs& args) override {
		const uint32_t v = waveVersion.load(std::memory_order_acquire);
		if (v != appliedWaveVersion) {
			rebuildWavetable();
			appliedWaveVersion = v;
		}

		const int channels = std::max(1, std::max({inputs[VOCT_INPUT].getChannels(), inputs[FM_INPUT].getChannels(), inputs[SYNC_INPUT].getChannels(), inputs[FOLD_CV_INPUT].getChannels()}));
		outputs[OUT_OUTPUT].setChannels(channels);
		outputs[RAW_OUTPUT].setChannels(channels);

		const float knobNorm = clamp01(params[FREQ_PARAM].getValue());
		const float baseFreq = wyrmBaseFrequencyFromKnob(knobNorm, lfoMode);
		const float fmAtten = params[FM_ATTEN_PARAM].getValue();
		const float fine = params[FINE_PARAM].getValue() / 1200.f;
		const float foldBase = params[FOLD_PARAM].getValue();
		const float slitherAmount = clamp01(params[SLITHER_PARAM].getValue());
		const float slitherSpeed = slitherSpeedFactor(params[SLITHER_SPEED_PARAM].getValue());

		for (int c = 0; c < channels; ++c) {
			if (inputs[SYNC_INPUT].isConnected()) {
				const float s = inputs[SYNC_INPUT].getPolyVoltage(c);
				if (syncTriggers[c].process(s)) {
					phase[c] = 0.f;
					slitherPhase[c] = 0.f;
				}
			}
			const float voct = inputs[VOCT_INPUT].isConnected() ? inputs[VOCT_INPUT].getPolyVoltage(c) : 0.f;
			const float fm = inputs[FM_INPUT].isConnected() ? inputs[FM_INPUT].getPolyVoltage(c) * fmAtten : 0.f;
			float hz = baseFreq * rack::dsp::exp2_taylor5(voct + fm + fine);
			hz = clamp(hz, 0.005f, 0.45f * args.sampleRate);
			phase[c] = wrap01Fast(phase[c] + hz * args.sampleTime);
			const float slitherBaseHz = lfoMode ? clamp(hz, 0.01f, 8.f) : clamp(0.125f * hz, 0.15f, 8.f);
			const float slitherHz = clamp(slitherBaseHz * slitherSpeed, 0.01f, 16.f);
			slitherPhase[c] = wrap01Fast(slitherPhase[c] + slitherHz * args.sampleTime);
			const float base = lookupWave(phase[c]);
			const float slither = applyRockClamp(base, phase[c], slitherOffset(phase[c], slitherPhase[c], slitherAmount));
			const float raw = clamp(base + slither, -1.f, 1.f);
			const float foldCv = inputs[FOLD_CV_INPUT].isConnected() ? clamp(inputs[FOLD_CV_INPUT].getPolyVoltage(c) / 10.f, -1.f, 1.f) : 0.f;
			const float foldAmt = clamp(foldBase + foldCv, 0.f, 2.f);
			float folded = raw;
			if (foldAmt > 1e-5f) {
				folded = clamp(softClip(foldWave(raw, foldAmt)) * kWyrmFoldMakeupGain, -1.f, 1.f);
			}
			outputs[RAW_OUTPUT].setVoltage(5.f * raw, c);
			outputs[OUT_OUTPUT].setVoltage(5.f * folded, c);
		}
	}
};

float WyrmFreqQuantity::getDisplayValue() {
	const auto* wyrm = static_cast<const Wyrm*>(module);
	return wyrmBaseFrequencyFromKnob(getValue(), wyrm ? wyrm->lfoMode : false);
}

void WyrmFreqQuantity::setDisplayValue(float displayValue) {
	const auto* wyrm = static_cast<const Wyrm*>(module);
	setImmediateValue(wyrmKnobValueForFrequency(displayValue, wyrm ? wyrm->lfoMode : false));
}

std::string WyrmFreqQuantity::getDisplayValueString() {
	const float hz = getDisplayValue();
	if (hz >= 1000.f) {
		return string::f("%.2f kHz", hz / 1000.f);
	}
	if (hz < 0.1f) {
		return string::f("%.3f Hz", hz);
	}
	if (hz < 10.f) {
		return string::f("%.2f Hz", hz);
	}
	return string::f("%.1f Hz", hz);
}

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;
	int hoveredRock = -1;
	int draggingRock = -1;
	float visualSlitherPhase = 0.f;
	double lastVisualUpdateSec = -1.0;
	Vec rockDragOffset;

	explicit WyrmWaveEditor(Wyrm* m) {
		module = m;
	}

	int indexFromX(float x) const {
		if (box.size.x <= 1.f) return 0;
		const float n = clamp(x / box.size.x, 0.f, 1.f);
		const int count = module ? module->pointCount : kWyrmPointCountDefault;
		return clamp(int(std::floor(n * float(count))), 0, count - 1);
	}

	float valueFromY(float y) const {
		if (box.size.y <= 1.f) return 0.f;
		const float n = clamp(y / box.size.y, 0.f, 1.f);
		return clamp(1.f - 2.f * n, -1.f, 1.f);
	}

	float phaseFromX(float x) const {
		if (box.size.x <= 1.f) return 0.f;
		return wrap01(clamp(x / box.size.x, 0.f, 0.9999f));
	}

	float yFromValue(float value) const {
		return (0.5f - 0.5f * clamp(value, -1.f, 1.f)) * box.size.y;
	}

	Vec rockCenter(const WyrmRock& rock) const {
		return Vec(rock.phase * box.size.x, yFromValue(rock.value));
	}

	Vec rockPixelRadius(const WyrmRock& rock) const {
		return Vec(std::max(5.f, rock.radiusPhase * box.size.x), std::max(5.f, 0.5f * rock.radiusValue * box.size.y));
	}

	void advanceVisualSlitherPhase(double nowSec) {
		if (!std::isfinite(nowSec)) return;
		if (lastVisualUpdateSec < 0.0 || !std::isfinite(lastVisualUpdateSec)) {
			lastVisualUpdateSec = nowSec;
			return;
		}
		const float elapsed = clamp(float(nowSec - lastVisualUpdateSec), 0.f, 0.25f);
		lastVisualUpdateSec = nowSec;
		if (!module) return;
		const float speedFactor = slitherSpeedFactor(module->params[Wyrm::SLITHER_SPEED_PARAM].getValue());
		visualSlitherPhase = wrap01(visualSlitherPhase + 0.65f * speedFactor * elapsed);
	}

	float slitherOffsetForIndex(int index) const {
		if (!module || module->pointCount <= 0) return 0.f;
		const float amount = clamp01(module->params[Wyrm::SLITHER_PARAM].getValue());
		if (amount <= 1e-5f) return 0.f;
		const float phase = (float(index) + 0.5f) / float(module->pointCount);
		return slitherOffset(phase, visualSlitherPhase, amount);
	}

	float displayWavePoint(int index) const {
		if (!module) return 0.f;
		const float phase = (float(index) + 0.5f) / float(module->pointCount);
		const float base = module->getWavePoint(index);
		const float slither = module->applyRockClamp(base, phase, slitherOffsetForIndex(index));
		return clamp(base + slither, -1.f, 1.f);
	}

	int rockIndexAt(Vec pos) const {
		if (!module) return -1;
		for (int i = module->rockCount - 1; i >= 0; --i) {
			const Vec center = rockCenter(module->rocks[i]);
			const Vec radius = rockPixelRadius(module->rocks[i]);
			const float dx = (pos.x - center.x) / radius.x;
			const float dy = (pos.y - center.y) / radius.y;
			if (dx * dx + dy * dy <= 1.25f) {
				return i;
			}
		}
		return -1;
	}

	void moveRockFromMouse(int rockIndex, Vec pos) {
		if (!module || rockIndex < 0 || rockIndex >= module->rockCount) return;
		const Vec adjusted = pos.minus(rockDragOffset);
		WyrmRock& rock = module->rocks[rockIndex];
		rock.phase = phaseFromX(adjusted.x);
		rock.value = valueFromY(adjusted.y);
		if (module->rockMouseMode == ROCK_MOUSE_DRAGS) {
			module->pushWavePointsOutsideRock(rockIndex);
		}
	}

	Vec currentLocalMousePos() const {
		if (!parent || !APP || !APP->scene || !APP->scene->rack) {
			return Vec();
		}
		return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
	}

	void applyPointFromPos(Vec pos) {
		if (!module || module->editorLocked) return;
		const int idx = indexFromX(pos.x);
		const float targetDisplayValue = valueFromY(pos.y);
		const double nowSec = system::getTime();
		advanceVisualSlitherPhase(nowSec);
		auto writeDisplayValue = [&](int pointIndex) {
			module->setWavePoint(pointIndex, targetDisplayValue - slitherOffsetForIndex(pointIndex));
		};
		if (lastIndex >= 0 && lastIndex != idx) {
			const int lo = std::min(lastIndex, idx);
			const int hi = std::max(lastIndex, idx);
			for (int i = lo; i <= hi; ++i) {
				// Operator-style paint gesture: each crossed segment is set directly.
				writeDisplayValue(i);
			}
		}
		else {
			writeDisplayValue(idx);
		}
		lastIndex = idx;
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onButton(e);
			return;
		}
		if (e.action == GLFW_PRESS) {
			lastIndex = -1;
			const int rockIndex = rockIndexAt(e.pos);
			if (rockIndex >= 0) {
				draggingRock = rockIndex;
				hoveredRock = rockIndex;
				if (module->rockMouseMode == ROCK_MOUSE_LIFTS) {
					module->liftedRock = rockIndex;
				}
				rockDragOffset = e.pos.minus(rockCenter(module->rocks[rockIndex]));
				e.consume(this);
				return;
			}
			applyPointFromPos(e.pos);
			e.consume(this);
			return;
		}
		if (e.action == GLFW_RELEASE) {
			lastIndex = -1;
			if (module->liftedRock == draggingRock) {
				module->liftedRock = -1;
			}
			draggingRock = -1;
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (module && draggingRock >= 0 && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			moveRockFromMouse(draggingRock, currentLocalMousePos());
			e.consume(this);
			return;
		}
		if (!module || module->editorLocked || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onDragMove(e);
			return;
		}
		applyPointFromPos(currentLocalMousePos());
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		if (!args.vg) return;
		advanceVisualSlitherPhase(system::getTime());

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(14, 14, 14, 205));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0.f, 0.5f * box.size.y);
		nvgLineTo(args.vg, box.size.x, 0.5f * box.size.y);
		nvgStrokeWidth(args.vg, 1.f);
		nvgStrokeColor(args.vg, nvgRGBA(240, 180, 42, 120));
		nvgStroke(args.vg);

		if (!module) return;

		Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= box.size.x && mouseLocal.y >= 0.f && mouseLocal.y <= box.size.y);
		hoveredRock = (draggingRock >= 0) ? draggingRock : (mouseInside ? rockIndexAt(mouseLocal) : -1);
		if (mouseInside) {
			const float guideY = clamp(mouseLocal.y, 0.f, box.size.y);
			const int hoverIdx = indexFromX(mouseLocal.x);
			const int count = module->pointCount;
			const float dxHover = box.size.x / float(count);
			const float x0 = hoverIdx * dxHover;
			const float x1 = x0 + dxHover;

			// Hover segment lane preview.
			nvgBeginPath(args.vg);
			nvgRect(args.vg, x0, 0.f, x1 - x0, box.size.y);
			nvgFillColor(args.vg, nvgRGBA(255, 230, 120, 28));
			nvgFill(args.vg);

			// Hover target-height guide.
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, 0.f, guideY);
			nvgLineTo(args.vg, box.size.x, guideY);
			nvgStrokeWidth(args.vg, 1.4f);
			nvgStrokeColor(args.vg, nvgRGBA(255, 232, 140, 180));
			nvgStroke(args.vg);
		}

		// Draw as discrete segments so each point reads as an editable bar.
		const float midY = 0.5f * box.size.y;
		const int count = module->pointCount;
		const float edgeInset = 2.2f;
		const float drawWidth = std::max(1.f, box.size.x - 2.f * edgeInset);
		const float dx = drawWidth / float(count);
		auto xAt = [&](int i) {
			return edgeInset + (float(i) + 0.5f) * dx;
		};
		for (int i = 0; i < count; ++i) {
			const float x = xAt(i);
			const float y = (0.5f - 0.5f * displayWavePoint(i)) * box.size.y;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x, midY);
			nvgLineTo(args.vg, x, y);
			nvgStrokeWidth(args.vg, 2.f);
			nvgStrokeColor(args.vg, nvgRGBA(158, 132, 78, 170));
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, 2.1f);
			nvgFillColor(args.vg, nvgRGBA(235, 204, 128, 245));
			nvgFill(args.vg);
		}

		// Constrain stylized body/texture rendering to the waveform editor bounds.
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);

		// Ouroboros body under-stroke.
		auto emitRoundedBodyPath = [&]() {
			const float roundCosThreshold = -0.25f; // smooth most corners, preserve very sharp reversals
			if (count <= 0) {
				return;
			}
			if (count == 1) {
				const float x = xAt(0);
				const float y = (0.5f - 0.5f * displayWavePoint(0)) * box.size.y;
				nvgMoveTo(args.vg, x, y);
				return;
			}

			auto pointAt = [&](int i) {
				return Vec(xAt(i), (0.5f - 0.5f * displayWavePoint(i)) * box.size.y);
			};

			const Vec pStart = pointAt(0);
			nvgMoveTo(args.vg, pStart.x, pStart.y);

			for (int i = 1; i < count - 1; ++i) {
				const Vec p0 = pointAt(i - 1);
				const Vec p1 = pointAt(i);
				const Vec p2 = pointAt(i + 1);
				Vec vIn = p1.minus(p0);
				Vec vOut = p2.minus(p1);
				const float inLen = std::sqrt(vIn.x * vIn.x + vIn.y * vIn.y);
				const float outLen = std::sqrt(vOut.x * vOut.x + vOut.y * vOut.y);
				if (inLen < 1e-4f || outLen < 1e-4f) {
					nvgLineTo(args.vg, p1.x, p1.y);
					continue;
				}
				vIn = vIn.div(inLen);
				vOut = vOut.div(outLen);
				const float cornerCos = vIn.x * vOut.x + vIn.y * vOut.y;
				if (cornerCos >= roundCosThreshold) {
					const Vec midOut = p1.plus(p2).mult(0.5f);
					nvgQuadTo(args.vg, p1.x, p1.y, midOut.x, midOut.y);
				}
				else {
					nvgLineTo(args.vg, p1.x, p1.y);
				}
			}

			const Vec pEnd = pointAt(count - 1);
			nvgLineTo(args.vg, pEnd.x, pEnd.y);
		};

		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 4.0f);
		nvgStrokeColor(args.vg, nvgRGBA(74, 54, 24, 205));
		nvgStroke(args.vg);

		// Mid-tone bronze body.
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 2.6f);
		nvgStrokeColor(args.vg, nvgRGBA(167, 132, 72, 230));
		nvgStroke(args.vg);

		// Verdigris/gold highlight along the top of the body.
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 1.15f);
		nvgStrokeColor(args.vg, nvgRGBA(246, 215, 136, 225));
		nvgStroke(args.vg);

		// Scale plates along the body.
		for (int i = 0; i < count; ++i) {
			const float x = xAt(i);
			const float y = (0.5f - 0.5f * displayWavePoint(i)) * box.size.y;
			const float plateW = clamp(0.31f * dx, 1.0f, 3.4f);
			const float plateH = 0.95f + 0.35f * std::sin(0.33f * float(i));
			for (int lane = 0; lane < 2; ++lane) {
				const float laneOffset = (lane == 0) ? -1.1f : 1.1f;
				const float laneShift = (lane == 0) ? -0.18f * dx : 0.18f * dx;
				nvgBeginPath(args.vg);
				nvgEllipse(args.vg, x + laneShift, y + laneOffset, plateW, plateH);
				nvgFillColor(args.vg,
					((i + lane) % 3) == 0
						? nvgRGBA(202, 168, 102, 185)
						: nvgRGBA(150, 110, 56, 150));
				nvgFill(args.vg);
			}
		}

		for (int i = 0; i < module->rockCount; ++i) {
			const WyrmRock& rock = module->rocks[i];
			const Vec center = rockCenter(rock);
			const Vec radius = rockPixelRadius(rock);
			const bool hot = (i == hoveredRock || i == draggingRock);
			nvgBeginPath(args.vg);
			nvgEllipse(args.vg, center.x, center.y, radius.x, radius.y);
			const int shade = 102 + int(44.f * hashUnit(rock.seed ^ 0x7a13u));
			nvgFillColor(args.vg, nvgRGBA(shade, shade, shade + 4, hot ? 245 : 215));
			nvgFill(args.vg);
			nvgStrokeWidth(args.vg, hot ? 2.2f : 1.1f);
			nvgStrokeColor(args.vg, hot ? nvgRGBA(236, 226, 190, 235) : nvgRGBA(42, 42, 44, 190));
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgEllipse(args.vg, center.x - 0.22f * radius.x, center.y - 0.24f * radius.y, 0.24f * radius.x, 0.16f * radius.y);
			nvgFillColor(args.vg, nvgRGBA(205, 205, 202, hot ? 100 : 72));
			nvgFill(args.vg);
		}

		if (draggingRock >= 0 && draggingRock < module->rockCount) {
			const Vec center = rockCenter(module->rocks[draggingRock]);
			const Vec radius = rockPixelRadius(module->rocks[draggingRock]);
			auto drawArrow = [&](Vec dir, Vec normal) {
				const Vec start = center.plus(Vec(dir.x * (radius.x + 3.f), dir.y * (radius.y + 3.f)));
				const Vec end = center.plus(Vec(dir.x * (radius.x + 18.f), dir.y * (radius.y + 18.f)));
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, start.x, start.y);
				nvgLineTo(args.vg, end.x, end.y);
				nvgStrokeWidth(args.vg, 1.5f);
				nvgStrokeColor(args.vg, nvgRGBA(236, 226, 190, 225));
				nvgStroke(args.vg);

				const Vec headA = end.minus(Vec(dir.x * 5.f, dir.y * 5.f)).plus(normal.mult(3.5f));
				const Vec headB = end.minus(Vec(dir.x * 5.f, dir.y * 5.f)).minus(normal.mult(3.5f));
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, end.x, end.y);
				nvgLineTo(args.vg, headA.x, headA.y);
				nvgMoveTo(args.vg, end.x, end.y);
				nvgLineTo(args.vg, headB.x, headB.y);
				nvgStrokeWidth(args.vg, 1.5f);
				nvgStrokeColor(args.vg, nvgRGBA(236, 226, 190, 225));
				nvgStroke(args.vg);
			};
			drawArrow(Vec(1.f, 0.f), Vec(0.f, 1.f));
			drawArrow(Vec(-1.f, 0.f), Vec(0.f, 1.f));
			drawArrow(Vec(0.f, 1.f), Vec(1.f, 0.f));
			drawArrow(Vec(0.f, -1.f), Vec(1.f, 0.f));
		}
		nvgResetScissor(args.vg);
		nvgRestore(args.vg);

	}
};

struct WyrmShapeMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int shape = SHAPE_SINE;

	void onAction(const event::Action& e) override {
		if (module) module->setFactoryShape(shape);
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->selectedShape == shape) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmPointCountMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int count = kWyrmPointCountDefault;

	void onAction(const event::Action& e) override {
		if (module) {
			module->setPointCount(count);
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->pointCount == count) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmEditorIconButton : TransparentWidget {
	enum Kind {
		LOCK,
		RESET
	};

	Wyrm* module = nullptr;
	Kind kind = LOCK;
	bool hovered = false;

	WyrmEditorIconButton(Wyrm* module, Kind kind) {
		this->module = module;
		this->kind = kind;
	}

	void step() override {
		hovered = false;
		if (parent && APP && APP->scene && APP->scene->rack) {
			const Vec local = APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
			hovered = (local.x >= 0.f && local.x <= box.size.x && local.y >= 0.f && local.y <= box.size.y);
		}
		TransparentWidget::step();
	}

	void onHover(const event::Hover& e) override {
		hovered = true;
		TransparentWidget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		hovered = false;
		TransparentWidget::onLeave(e);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			TransparentWidget::onButton(e);
			return;
		}
		if (kind == LOCK) {
			module->editorLocked = !module->editorLocked;
		}
		else {
			module->setFactoryShape(module->selectedShape);
		}
		e.consume(this);
	}

	void drawLockIcon(const DrawArgs& args, NVGcolor color) {
		const float w = box.size.x;
		const float h = box.size.y;
		const float cx = 0.5f * w;
		const float bodyW = 0.54f * w;
		const float bodyH = 0.32f * h;
		const float bodyX = cx - 0.5f * bodyW;
		const float bodyY = 0.51f * h;
		const float bodyR = 0.06f * w;
		const float shackleW = 0.34f * w;
		const float shackleTop = 0.19f * h;
		const float shackleY = bodyY + 0.03f * h;
		const bool locked = module && module->editorLocked;

		nvgStrokeWidth(args.vg, 2.3f);
		nvgStrokeColor(args.vg, color);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);

		nvgBeginPath(args.vg);
		if (locked) {
			nvgMoveTo(args.vg, cx - 0.5f * shackleW, shackleY);
			nvgLineTo(args.vg, cx - 0.5f * shackleW, shackleTop + 0.18f * h);
			nvgQuadTo(args.vg, cx, shackleTop, cx + 0.5f * shackleW, shackleTop + 0.18f * h);
			nvgLineTo(args.vg, cx + 0.5f * shackleW, shackleY);
		}
		else {
			const float detachedY = shackleY - 0.22f * h;
			nvgMoveTo(args.vg, cx - 0.5f * shackleW, shackleY);
			nvgLineTo(args.vg, cx - 0.5f * shackleW, shackleTop + 0.02f * h);
			nvgQuadTo(args.vg, cx, shackleTop - 0.16f * h, cx + 0.5f * shackleW, shackleTop + 0.02f * h);
			nvgLineTo(args.vg, cx + 0.5f * shackleW, detachedY);
		}
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, bodyX, bodyY, bodyW, bodyH, bodyR);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);

		NVGcolor cutout = nvgRGBA(14, 14, 14, 255);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cx, bodyY + 0.40f * bodyH, 0.054f * w);
		nvgFillColor(args.vg, cutout);
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, cx - 0.024f * w, bodyY + 0.40f * bodyH, 0.048f * w, 0.22f * bodyH, 0.9f);
		nvgFillColor(args.vg, cutout);
		nvgFill(args.vg);
	}

	void drawResetIcon(const DrawArgs& args, NVGcolor color) {
		const float w = box.size.x;
		const float h = box.size.y;
		const float cx = 0.56f * w;
		const float cy = 0.56f * h;
		const float r = 0.27f * std::min(w, h);
		const float shaftAngle = 0.90f * float(M_PI);
		const float endAngle = -0.30f * float(M_PI);
		const Vec shaftBottom(cx + std::cos(shaftAngle) * r, cy + std::sin(shaftAngle) * r);
		const Vec shaftTop(shaftBottom.x, cy - 0.98f * r);
		const float tipX = shaftTop.x - 0.20f * w;
		const float baseX = shaftTop.x + 0.01f * w;
		const float halfH = 0.105f * h;

		nvgStrokeWidth(args.vg, 1.95f);
		nvgStrokeColor(args.vg, color);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		nvgArc(args.vg, cx, cy, r, shaftAngle, endAngle, NVG_CCW);
		nvgMoveTo(args.vg, shaftBottom.x, shaftBottom.y);
		nvgLineTo(args.vg, shaftTop.x, shaftTop.y);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, tipX, shaftTop.y);
		nvgLineTo(args.vg, baseX, shaftTop.y - halfH);
		nvgLineTo(args.vg, baseX, shaftTop.y + halfH);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	}

	void draw(const DrawArgs& args) override {
		const int level = hovered ? 218 : 118;
		NVGcolor color = nvgRGBA(level, level, level, 255);
		if (kind == LOCK) {
			drawLockIcon(args, color);
		}
		else {
			drawResetIcon(args, color);
		}
	}
};

struct WyrmWidget : ModuleWidget {
	explicit WyrmWidget(Wyrm* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/wyrm.svg");
		setPanel(createPanel(panelPath));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto applyPt = [&](const char* id, Vec* pos) {
			Vec p;
			if (panel_svg::loadPointFromSvgMm(panelPath, id, &p)) {
				*pos = p;
			}
		};

		math::Rect editorRectMm(Vec(6.0f, 16.0f), Vec(59.12f, 52.0f));
		panel_svg::loadRectFromSvgMm(panelPath, "WYRm_WAVE_EDITOR", &editorRectMm);
		Vec freqPos(17.5f, 80.0f);
		Vec finePos(35.56f, 80.0f);
		Vec fmAttenPos(53.62f, 80.0f);
		Vec foldPos(35.56f, 98.0f);
		Vec lockPos(10.50f, 75.50f);
		Vec resetPos(17.25f, 75.50f);
		Vec slitherPos(17.50f, 112.80f);
		Vec slitherSpeedPos(26.50f, 112.80f);
		Vec voctPos(14.0f, 111.0f);
		Vec fmPos(28.0f, 111.0f);
		Vec syncPos(43.0f, 111.0f);
		Vec foldCvPos(57.0f, 111.0f);
		Vec rawOutPos(24.0f, 122.0f);
		Vec outPos(47.0f, 122.0f);
		applyPt("WYRM_FREQ_PARAM", &freqPos);
		applyPt("WYRM_FINE_PARAM", &finePos);
		applyPt("WYRM_FM_ATTEN_PARAM", &fmAttenPos);
		applyPt("WYRM_FOLD_PARAM", &foldPos);
		applyPt("WYRM_LOCK_BUTTON", &lockPos);
		applyPt("WYRM_RESET_BUTTON", &resetPos);
		applyPt("WYRM_SLITHER_PARAM", &slitherPos);
		applyPt("WYRM_SLITHER_SPEED_PARAM", &slitherSpeedPos);
		applyPt("WYRM_VOCT_INPUT", &voctPos);
		applyPt("WYRM_FM_INPUT", &fmPos);
		applyPt("WYRM_SYNC_INPUT", &syncPos);
		applyPt("WYRM_FOLD_CV_INPUT", &foldCvPos);
		applyPt("WYRM_RAW_OUTPUT", &rawOutPos);
		applyPt("WYRM_OUT_OUTPUT", &outPos);

		auto* editor = new WyrmWaveEditor(module);
		editor->box.pos = mm2px(editorRectMm.pos);
		editor->box.size = mm2px(editorRectMm.size);
		addChild(editor);

		auto addEditorIconButton = [&](WyrmEditorIconButton::Kind kind, Vec posMm) {
			auto* button = new WyrmEditorIconButton(module, kind);
			button->box.size = mm2px(Vec(5.2f, 5.2f));
			button->box.pos = mm2px(posMm).minus(button->box.size.mult(0.5f));
			addChild(button);
		};
		addEditorIconButton(WyrmEditorIconButton::LOCK, lockPos);
		addEditorIconButton(WyrmEditorIconButton::RESET, resetPos);

		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(freqPos), module, Wyrm::FREQ_PARAM));
		addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(finePos), module, Wyrm::FINE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(fmAttenPos), module, Wyrm::FM_ATTEN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(foldPos), module, Wyrm::FOLD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(slitherPos), module, Wyrm::SLITHER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(slitherSpeedPos), module, Wyrm::SLITHER_SPEED_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(voctPos), module, Wyrm::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(fmPos), module, Wyrm::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(syncPos), module, Wyrm::SYNC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(foldCvPos), module, Wyrm::FOLD_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(rawOutPos), module, Wyrm::RAW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(outPos), module, Wyrm::OUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* module = dynamic_cast<Wyrm*>(this->module);
		if (!module) return;

		menu->addChild(new MenuSeparator());
		menu->addChild(createBoolPtrMenuItem("LFO Mode", "", &module->lfoMode));
		menu->addChild(createBoolPtrMenuItem("Lock Wave Editor", "", &module->editorLocked));
		menu->addChild(createSubmenuItem("Rocks", string::f("%d", module->rockCount), [=](Menu* submenu) {
			submenu->addChild(createCheckMenuItem(
				"Mouse Drags Rocks", "",
				[=]() {
					return module->rockMouseMode == ROCK_MOUSE_DRAGS;
				},
				[=]() {
					module->rockMouseMode = ROCK_MOUSE_DRAGS;
					module->liftedRock = -1;
				}));
			submenu->addChild(createCheckMenuItem(
				"Mouse Lifts Rocks", "",
				[=]() {
					return module->rockMouseMode == ROCK_MOUSE_LIFTS;
				},
				[=]() {
					module->rockMouseMode = ROCK_MOUSE_LIFTS;
				}));
			submenu->addChild(new MenuSeparator());
			for (int count = 0; count <= kWyrmMaxRocks; ++count) {
				submenu->addChild(createCheckMenuItem(
					string::f("%d", count), "",
					[=]() {
						return module->rockCount == count;
					},
					[=]() {
						module->setRockCount(count);
					}));
			}
		}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Point Count"));
		for (int count : {32, 48, 64, 128}) {
			auto* item = new WyrmPointCountMenuItem();
			item->text = string::f("%d", count);
			item->module = module;
			item->count = count;
			menu->addChild(item);
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Factory Shape"));
		for (int i = 0; i < SHAPE_COUNT; ++i) {
			auto* item = new WyrmShapeMenuItem();
			item->text = kWyrmShapeLabels[i];
			item->module = module;
			item->shape = i;
			menu->addChild(item);
		}
	}
};

Model* modelWyrm = createModel<Wyrm, WyrmWidget>("Wyrm");
