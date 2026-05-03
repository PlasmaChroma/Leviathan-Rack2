#include "Wyrm.hpp"

const char* const kWyrmShapeLabels[SHAPE_COUNT] = {
	"Sine",
	"Triangle",
	"Saw Up",
	"Saw Down",
	"Square",
	"S.Saw Up",
	"S.Saw Down"
};

Wyrm::Wyrm() {
	createdUnixTimeSec = system::getUnixTime();
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	const float defaultFreqKnob = wyrmKnobValueForFrequency(261.63f, false);
	configParam<WyrmFreqQuantity>(FREQ_PARAM, 0.f, 1.f, defaultFreqKnob, "Frequency");
	configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
	configParam(FM_ATTEN_PARAM, -1.f, 1.f, 0.f, "FM attenuator");
	configParam(FOLD_PARAM, 0.f, 1.f, 0.f, "Fold amount");
	configParam(SLITHER_PARAM, 0.f, 1.f, 0.f, "Slither", "%", 0.f, 100.f);
	configParam(SLITHER_SPEED_PARAM, 0.f, 1.f, 0.5f, "Slither speed");
	configInput(VOCT_INPUT, "V/Oct");
	configInput(FM_INPUT, "FM");
	configInput(SYNC_INPUT, "Sync");
	configInput(FOLD_CV_INPUT, "Fold CV");
	configOutput(OUT_OUTPUT, "Fold");
	configOutput(RAW_OUTPUT, "Signal");

	setFactoryShape(SHAPE_SINE);
	for (int i = 0; i < kWyrmMaxRocks; ++i) {
		placeRock(i);
	}
}

void Wyrm::placeRock(int index) {
	if (index < 0 || index >= kWyrmMaxRocks) return;
	const uint32_t seed = 0x9e3779b9u + uint32_t(index) * 0x85ebca6bu;
	WyrmRock& rock = rocks[index];
	rock.seed = seed;
	rock.phase = 0.08f + 0.84f * hashUnit(seed ^ 0x31524u);
	rock.value = -0.72f + 1.44f * hashUnit(seed ^ 0x9ab31u);
	rock.radiusPhase = 0.035f + 0.02f * hashUnit(seed ^ 0x4c2du);
	rock.radiusValue = 0.105f + 0.055f * hashUnit(seed ^ 0x732u);
}

void Wyrm::setRockCount(int count) {
	const int oldCount = rockCount;
	rockCount = clamp(count, 0, kWyrmMaxRocks);
	if (liftedRock >= rockCount) {
		liftedRock = -1;
	}
	for (int i = oldCount; i < rockCount; ++i) {
		pushWavePointsOutsideRock(i);
	}
}

void Wyrm::setWavePoint(int index, float value) {
	if (index < 0 || index >= pointCount) {
		return;
	}
	wavePoints[index].store(clamp(value, -1.f, 1.f), std::memory_order_relaxed);
	waveCustomized = true;
	waveVersion.fetch_add(1u, std::memory_order_release);
}

float Wyrm::getWavePoint(int index) const {
	if (index < 0 || index >= pointCount) {
		return 0.f;
	}
	return wavePoints[index].load(std::memory_order_relaxed);
}

void Wyrm::setFactoryShape(int shapeId) {
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
			case SHAPE_SUPERSAW: {
				// Static unison-like supersaw with nested smaller saw layers for richer shape detail.
				static constexpr float phaseOffsets[] = {-0.032f, -0.016f, 0.f, 0.016f, 0.032f};
				float baseSum = 0.f;
				for (float offset : phaseOffsets) {
					const float ph = wrap01(p + offset);
					baseSum += 2.f * ph - 1.f;
				}
				const float base = baseSum / float(sizeof(phaseOffsets) / sizeof(phaseOffsets[0]));
				const float inner1 = 2.f * wrap01(p * 2.f + 0.13f) - 1.f;
				const float inner2 = 2.f * wrap01(p * 3.f - 0.21f) - 1.f;
				v = base + 0.22f * inner1 + 0.11f * inner2;
				v = clamp(v * 1.05f, -1.f, 1.f);
			} break;
			case SHAPE_SUPERSAW_DOWN: {
				// Downward counterpart of supersaw: reverse all saw slopes.
				static constexpr float phaseOffsets[] = {-0.032f, -0.016f, 0.f, 0.016f, 0.032f};
				float baseSum = 0.f;
				for (float offset : phaseOffsets) {
					const float ph = wrap01(p + offset);
					baseSum += 1.f - 2.f * ph;
				}
				const float base = baseSum / float(sizeof(phaseOffsets) / sizeof(phaseOffsets[0]));
				const float inner1 = 1.f - 2.f * wrap01(p * 2.f + 0.13f);
				const float inner2 = 1.f - 2.f * wrap01(p * 3.f - 0.21f);
				v = base + 0.22f * inner1 + 0.11f * inner2;
				v = clamp(v * 1.05f, -1.f, 1.f);
			} break;
			default: break;
		}
		wavePoints[i].store(clamp(v, -1.f, 1.f), std::memory_order_relaxed);
	}
	waveCustomized = false;
	waveVersion.fetch_add(1u, std::memory_order_release);
}

void Wyrm::setPointCount(int newPointCount) {
	newPointCount = clamp(newPointCount, 32, kWyrmPointCountMax);
	if (newPointCount != 32 && newPointCount != 48 && newPointCount != 64 && newPointCount != 128 && newPointCount != 256) {
		newPointCount = kWyrmPointCountDefault;
	}
	if (newPointCount == pointCount) {
		return;
	}
	pointCount = newPointCount;
	setFactoryShape(selectedShape);
}

void Wyrm::rebuildWavetable() {
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

float Wyrm::lookupWave(float ph) const {
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

float Wyrm::rockDx(float ph, const WyrmRock& rock) const {
	float dx = wrap01(ph) - wrap01(rock.phase);
	if (dx > 0.5f) {
		dx -= 1.f;
	}
	else if (dx < -0.5f) {
		dx += 1.f;
	}
	return dx;
}

float Wyrm::rockClearancePhase(const WyrmRock& rock) const {
	return kWyrmRockClearance * rock.radiusPhase / std::max(kWyrmRockValueScale * rock.radiusValue, 1e-4f);
}

float Wyrm::rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue) const {
	const float radiusPhase = rock.radiusPhase + ((clearanceValue > 0.f) ? rockClearancePhase(rock) : 0.f);
	const float radiusValue = kWyrmRockValueScale * (rock.radiusValue + clearanceValue);
	if (std::fabs(dx) >= radiusPhase) {
		return 0.f;
	}
	const float nx = dx / std::max(radiusPhase, 1e-4f);
	return radiusValue * std::sqrt(std::max(0.f, 1.f - nx * nx));
}

bool Wyrm::rockBoundsAtPhase(const WyrmRock& rock, float ph, float* lower, float* upper) const {
	const float edgeY = rockEdgeY(rock, rockDx(ph, rock), kWyrmRockClearance);
	if (edgeY <= 0.f) {
		return false;
	}
	if (lower) *lower = rock.value - edgeY;
	if (upper) *upper = rock.value + edgeY;
	return true;
}

bool Wyrm::pushPointOutsideRock(int pointIndex, const WyrmRock& rock, bool preferUpper, bool forceSide) {
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

bool Wyrm::segmentIntersectsRockBounds(const WyrmRock& rock, float ph0, float y0, float ph1, float y1, bool* preferUpper) const {
	const float rx = rock.radiusPhase + rockClearancePhase(rock);
	const float ry = kWyrmRockValueScale * (rock.radiusValue + kWyrmRockClearance);
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

void Wyrm::pushWavePointsOutsideRock(int rockIndex) {
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
		waveCustomized = true;
		waveVersion.fetch_add(1u, std::memory_order_release);
	}
}

float Wyrm::applyRockPush(float base, float ph) const {
	if (rockCount <= 0) {
		return base;
	}
	float pushed = base;
	for (int i = 0; i < rockCount; ++i) {
		if (i == liftedRock) continue;
		float lower = 0.f;
		float upper = 0.f;
		if (!rockBoundsAtPhase(rocks[i], ph, &lower, &upper)) {
			continue;
		}
		if (pushed > lower && pushed < upper) {
			pushed = (std::fabs(pushed - lower) < std::fabs(upper - pushed)) ? lower : upper;
		}
	}
	return clamp(pushed, -1.f, 1.f);
}

float Wyrm::applyRockClamp(float base, float ph, float offset) const {
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

json_t* Wyrm::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "lfoMode", json_boolean(lfoMode));
	json_object_set_new(root, "editorLocked", json_boolean(editorLocked));
	json_object_set_new(root, "waveCustomized", json_boolean(waveCustomized));
	json_object_set_new(root, "selectedShape", json_integer(selectedShape));
	json_object_set_new(root, "pointCount", json_integer(pointCount));
	json_object_set_new(root, "rockCount", json_integer(rockCount));
	json_object_set_new(root, "rockMouseMode", json_integer(rockMouseMode));
	json_object_set_new(root, "createdUnixTimeSec", json_real(createdUnixTimeSec));
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

void Wyrm::dataFromJson(json_t* root) {
	json_t* lfoJ = json_object_get(root, "lfoMode");
	if (lfoJ) lfoMode = json_is_true(lfoJ);
	json_t* lockJ = json_object_get(root, "editorLocked");
	if (lockJ) editorLocked = json_is_true(lockJ);
	json_t* customizedJ = json_object_get(root, "waveCustomized");
	if (customizedJ) waveCustomized = json_is_true(customizedJ);
	json_t* shapeJ = json_object_get(root, "selectedShape");
	if (shapeJ) selectedShape = clamp(int(json_integer_value(shapeJ)), 0, SHAPE_COUNT - 1);
	json_t* pointCountJ = json_object_get(root, "pointCount");
	if (pointCountJ) {
		int loadedPointCount = int(json_integer_value(pointCountJ));
		if (loadedPointCount == 32 || loadedPointCount == 48 || loadedPointCount == 64 || loadedPointCount == 128 || loadedPointCount == 256) {
			pointCount = loadedPointCount;
		}
	}
	json_t* rockCountJ = json_object_get(root, "rockCount");
	if (rockCountJ) setRockCount(int(json_integer_value(rockCountJ)));
	json_t* rockMouseModeJ = json_object_get(root, "rockMouseMode");
	if (rockMouseModeJ) rockMouseMode = clamp(int(json_integer_value(rockMouseModeJ)), ROCK_MOUSE_DRAGS, ROCK_MOUSE_LIFTS);
	json_t* createdUnixTimeSecJ = json_object_get(root, "createdUnixTimeSec");
	if (createdUnixTimeSecJ && json_is_number(createdUnixTimeSecJ)) {
		const double loadedCreatedUnixTimeSec = json_number_value(createdUnixTimeSecJ);
		if (std::isfinite(loadedCreatedUnixTimeSec) && loadedCreatedUnixTimeSec > 0.0) {
			createdUnixTimeSec = loadedCreatedUnixTimeSec;
		}
	}
	json_t* pts = json_object_get(root, "wavePoints");
	if (pts && json_is_array(pts)) {
		const size_t n = json_array_size(pts);
		for (int i = 0; i < pointCount && i < int(n); ++i) {
			json_t* v = json_array_get(pts, i);
			if (v) {
				wavePoints[i].store(clamp(float(json_number_value(v)), -1.f, 1.f), std::memory_order_relaxed);
			}
		}
		if (!customizedJ && n > 0) {
			waveCustomized = true;
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

void Wyrm::process(const ProcessArgs& args) {
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
		const float displayHzNoFm = clamp(baseFreq * rack::dsp::exp2_taylor5(voct + fine), 0.005f, 0.45f * args.sampleRate);
		float hz = baseFreq * rack::dsp::exp2_taylor5(voct + fm + fine);
		hz = clamp(hz, 0.005f, 0.45f * args.sampleRate);
		if (c == 0) {
			displayFrequencyHz.store(displayHzNoFm, std::memory_order_relaxed);
		}
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
