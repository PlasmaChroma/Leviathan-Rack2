#include "Wyrm.hpp"
#include "DebugTerminalTransport.hpp"

#include <chrono>

namespace {
static std::atomic<uint32_t> gWyrmDebugInstanceCounter {1u};
constexpr int kWyrmPerfMeasureDivision = 17;
constexpr float kWyrmRockFoldReturn = 0.78f;

inline float finiteOr(float x, float fallback = 0.f) {
	return std::isfinite(x) ? x : fallback;
}

inline float foldedRockPenetration(float penetration, float rockWidth) {
	const float width = std::max(rockWidth, 1e-5f);
	const float period = 2.f * width;
	float folded = std::fmod(std::max(0.f, penetration), period);
	if (folded > width) {
		folded = period - folded;
	}
	return folded * kWyrmRockFoldReturn;
}

inline float foldAwayFromRock(float anchorY, float desiredY, float lower, float upper) {
	const float width = upper - lower;
	if (width <= 1e-6f) {
		return clamp(desiredY, -1.f, 1.f);
	}
	if (anchorY <= lower) {
		return clamp(lower - foldedRockPenetration(desiredY - lower, width), -1.f, 1.f);
	}
	if (anchorY >= upper) {
		return clamp(upper + foldedRockPenetration(upper - desiredY, width), -1.f, 1.f);
	}
	const float center = 0.5f * (lower + upper);
	if (desiredY < center) {
		return clamp(lower - foldedRockPenetration(desiredY - lower, width), -1.f, 1.f);
	}
	return clamp(upper + foldedRockPenetration(upper - desiredY, width), -1.f, 1.f);
}
}

const char* const kWyrmShapeLabels[SHAPE_COUNT] = {
	"Sine",
	"Triangle",
	"Saw Up",
	"Saw Down",
	"Square",
	"S.Saw"
};

Wyrm::Wyrm() {
	debugInstanceId = gWyrmDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
	createdUnixTimeSec = system::getUnixTime();
	perfMeasureDivider.setDivision(kWyrmPerfMeasureDivision);
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	const float defaultFreqKnob = wyrmKnobValueForFrequency(261.63f, false);
	configParam<WyrmFreqQuantity>(FREQ_PARAM, 0.f, 1.f, defaultFreqKnob, "Frequency");
	configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
	configParam(FM_ATTEN_PARAM, -1.f, 1.f, 0.f, "FM attenuator");
	configParam(FOLD_PARAM, 0.f, 1.f, 0.f, "Fold amount");
	configParam(SLITHER_PARAM, 0.f, 1.f, 0.f, "Slither", "%", 0.f, 100.f);
	configParam(SLITHER_SPEED_PARAM, 0.f, 1.f, 0.5f, "Slither speed");
	configButton(WAVE_LEFT_PARAM, "Waveform previous");
	configButton(WAVE_RIGHT_PARAM, "Waveform next");
	configSwitch(LFO_MODE_PARAM, 0.f, 1.f, 0.f, "LFO mode", {"Audio", "LFO"});
	configSwitch(SYNC_MODE_PARAM, 0.f, 1.f, 0.f, "Sync mode", {"Hard", "Soft"});
	configSwitch(ENV_MODE_PARAM, 0.f, 1.f, 0.f, "Envelope mode", {"Oscillator", "Envelope"});
	configSwitch(COARSE_STEP_MODE_PARAM, 0.f, 1.f, 0.f, "Octave stepped", {"Continuous", "Octave stepped"});
	configInput(VOCT_INPUT, "V/Oct / Envelope trigger");
	configInput(FM_INPUT, "FM");
	configInput(SYNC_INPUT, "Sync");
	configInput(FOLD_CV_INPUT, "Fold CV");
	configInput(SLITHER_CV_INPUT, "Slither CV");
	configInput(SLITHER_SPEED_CV_INPUT, "Slither speed CV");
	configOutput(OUT_OUTPUT, "Fold");
	configOutput(RAW_OUTPUT, "Signal");

	setFactoryShape(SHAPE_SINE);
	for (int i = 0; i < kWyrmMaxRocks; ++i) {
		placeRock(i);
	}
	for (int c = 0; c < kWyrmMaxChannels; ++c) {
		phaseDir[c] = 1.f;
	}
	publishRockState();
}

Wyrm::~Wyrm() {
	teardownTimer.begin(id);
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
	rebuildRockBoundaryCache(index);
	publishRockState();
}

void Wyrm::setRockCount(int count) {
	const int oldCount = rockCount;
	rockCount = clamp(count, 0, kWyrmMaxRocks);
	if (liftedRock >= rockCount) {
		liftedRock = -1;
	}
	for (int i = oldCount; i < rockCount; ++i) {
		rebuildRockBoundaryCache(i);
	}
	for (int i = rockCount; i < kWyrmMaxRocks; ++i) {
		rockBoundaryCaches[i].valid = false;
	}
	publishRockState();
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
				// Phase-align triangle with sine: zero crossings at p=0.0 and p=0.5.
				if (p < 0.25f) {
					v = 4.f * p;
				}
				else if (p < 0.75f) {
					v = 2.f - 4.f * p;
				}
				else {
					v = -4.f + 4.f * p;
				}
			} break;
			case SHAPE_SAW: v = 2.f * p - 1.f; break;
			case SHAPE_REV_SAW: v = 1.f - 2.f * p; break;
			case SHAPE_SQUARE: v = (p < 0.5f) ? 1.f : -1.f; break;
			case SHAPE_SUPERSAW: {
				// Static unison-like supersaw with nested smaller saw layers for richer shape detail.
				static constexpr float phaseOffsets[] = {-0.032f, -0.016f, 0.f, 0.016f, 0.032f};
				float baseSum = 0.f;
				for (float offset : phaseOffsets) {
					const float ph = levi_math::wrap01(p + offset);
					baseSum += 2.f * ph - 1.f;
				}
				const float base = baseSum / float(sizeof(phaseOffsets) / sizeof(phaseOffsets[0]));
				const float inner1 = 2.f * levi_math::wrap01(p * 2.f + 0.13f) - 1.f;
				const float inner2 = 2.f * levi_math::wrap01(p * 3.f - 0.21f) - 1.f;
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

void Wyrm::setEnvelopeArShape() {
	const int lastIndex = std::max(1, pointCount - 1);
	const int attackEnd = clamp(int(std::lround(0.15f * float(lastIndex))), 1, lastIndex - 1);
	const int releaseLength = std::max(1, lastIndex - attackEnd);
	for (int i = 0; i < pointCount; ++i) {
		float unipolar = 0.f;
		if (i <= attackEnd) {
			const float t = float(i) / float(attackEnd);
			unipolar = t + 0.35f * t * (1.f - t);
		}
		else {
			const float t = float(i - attackEnd) / float(releaseLength);
			const float tail = 1.f - t;
			unipolar = tail * tail;
		}
		wavePoints[i].store(2.f * clamp(unipolar, 0.f, 1.f) - 1.f, std::memory_order_relaxed);
	}
	waveCustomized = true;
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
	if (envelopeMode.load(std::memory_order_relaxed)) {
		setEnvelopeArShape();
	}
	else {
		setFactoryShape(selectedShape);
	}
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
		wavetableMip[0][i] = y;
		maxAbs = std::max(maxAbs, std::fabs(y));
	}
	const float inv = 1.f / maxAbs;
	for (int i = 0; i < kWyrmTableSize; ++i) {
		wavetableMip[0][i] = clamp(wavetableMip[0][i] * inv, -1.f, 1.f);
	}

	// Build progressively band-limited mip tables for high-frequency playback.
	for (int level = 1; level < kWyrmTableMipLevels; ++level) {
		for (int i = 0; i < kWyrmTableSize; ++i) {
			const int im1 = (i - 1 + kWyrmTableSize) % kWyrmTableSize;
			const int ip1 = (i + 1) % kWyrmTableSize;
			wavetableMip[level][i] = clamp(
				0.25f * wavetableMip[level - 1][im1] + 0.5f * wavetableMip[level - 1][i] + 0.25f * wavetableMip[level - 1][ip1],
				-1.f,
				1.f
			);
		}
	}
}

float Wyrm::lookupWave(float ph, float phaseStep) const {
	float p = ph;
	if (p >= 1.f) {
		p -= 1.f;
	}
	else if (p < 0.f) {
		p += 1.f;
	}
	const float stepIdx = std::fabs(phaseStep) * float(kWyrmTableSize);
	float lod = 0.f;
	float scale = stepIdx;
	while (scale >= 2.f && lod < float(kWyrmTableMipLevels - 1)) {
		scale *= 0.5f;
		lod += 1.f;
	}
	const int level0 = clamp(int(lod), 0, kWyrmTableMipLevels - 1);
	const int level1 = std::min(level0 + 1, kWyrmTableMipLevels - 1);
	const float levelMix = clamp(scale - 1.f, 0.f, 1.f);
	const float x = p * float(kWyrmTableSize);
	const int i0 = int(x);
	const int i1 = (i0 + 1 < kWyrmTableSize) ? (i0 + 1) : 0;
	const float t = x - float(i0);
	const float a0 = std::fma((wavetableMip[level0][i1] - wavetableMip[level0][i0]), t, wavetableMip[level0][i0]);
	const float a1 = std::fma((wavetableMip[level1][i1] - wavetableMip[level1][i0]), t, wavetableMip[level1][i0]);
	return std::fma((a1 - a0), levelMix, a0);
}

float Wyrm::rockDx(float ph, const WyrmRock& rock) const {
	float dx = levi_math::wrap01(ph) - levi_math::wrap01(rock.phase);
	if (dx > 0.5f) {
		dx -= 1.f;
	}
	else if (dx < -0.5f) {
		dx += 1.f;
	}
	return dx;
}

float Wyrm::rockClearancePhase(const WyrmRock& rock) const {
	return rockClearancePhase(rock, kWyrmRockClearance);
}

float Wyrm::rockClearancePhase(const WyrmRock& rock, float clearanceValue) const {
	return clearanceValue * rock.radiusPhase / std::max(rock.radiusValue, 1e-4f);
}

float Wyrm::rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue) const {
	return rockEdgeY(rock, dx, clearanceValue, (clearanceValue > 0.f) ? rockClearancePhase(rock, clearanceValue) : 0.f);
}

float Wyrm::rockEdgeY(const WyrmRock& rock, float dx, float clearanceValue, float clearancePhase) const {
	const float radiusPhase = rock.radiusPhase + std::max(0.f, clearancePhase);
	const float radiusValue = kWyrmRockValueScale * (rock.radiusValue + clearanceValue);
	if (std::fabs(dx) >= radiusPhase) {
		return 0.f;
	}
	const float nx = dx / std::max(radiusPhase, 1e-4f);
	return radiusValue * std::sqrt(std::max(0.f, 1.f - nx * nx));
}

void Wyrm::rebuildRockBoundaryCache(int rockIndex) {
	if (rockIndex < 0 || rockIndex >= kWyrmMaxRocks) {
		return;
	}
	const WyrmRock& rock = rocks[rockIndex];
	WyrmRockBoundaryCache& cache = rockBoundaryCaches[rockIndex];
	cache.valid = true;
	cache.phase = rock.phase;
	cache.value = rock.value;
	cache.radiusPhase = rock.radiusPhase;
	cache.radiusValue = rock.radiusValue;
	const float rx = rock.radiusPhase + rockClearancePhase(rock);
	for (int i = 0; i < kWyrmRockBoundarySamples; ++i) {
		const float t = float(i) / float(kWyrmRockBoundarySamples - 1);
		const float dx = (-rx) + 2.f * rx * t;
		const float edgeY = rockEdgeY(rock, dx, kWyrmRockClearance);
		cache.lower[i] = rock.value - edgeY;
		cache.upper[i] = rock.value + edgeY;
	}
}

void Wyrm::rebuildAllRockBoundaryCaches() {
	for (int i = 0; i < rockCount; ++i) {
		rebuildRockBoundaryCache(i);
	}
	for (int i = rockCount; i < kWyrmMaxRocks; ++i) {
		rockBoundaryCaches[i].valid = false;
	}
}

void Wyrm::publishRockState() {
	const int currentIndex = activeRockStateIndex.load(std::memory_order_relaxed);
	const int nextIndex = 1 - currentIndex;
	WyrmRockStateSnapshot& next = activeRockState[nextIndex];
	next.rockCount = rockCount;
	next.liftedRock = liftedRock;
	next.rocks = rocks;
	next.rockBoundaryCaches = rockBoundaryCaches;
	for (int i = 0; i < kWyrmMaxRocks; ++i) {
		const WyrmRock& rock = next.rocks[i];
		const float clearancePhase = kWyrmRockClearance * rock.radiusPhase / std::max(rock.radiusValue, 1e-4f);
		const float rx = rock.radiusPhase + std::max(0.f, clearancePhase);
		next.wrappedPhase[i] = levi_math::wrap01(rock.phase);
		next.defaultClearancePhase[i] = clearancePhase;
		next.defaultRx[i] = rx;
		next.defaultInvRx[i] = 1.f / std::max(rx, 1e-4f);
		next.defaultRadiusValue[i] = kWyrmRockValueScale * (rock.radiusValue + kWyrmRockClearance);
	}
	activeRockStateIndex.store(nextIndex, std::memory_order_release);
}

const WyrmRockStateSnapshot& Wyrm::getActiveRockState() const {
	const int index = activeRockStateIndex.load(std::memory_order_acquire);
	return activeRockState[index];
}

bool Wyrm::cachedRockBoundsAtPhase(int rockIndex, float ph, float* lower, float* upper) const {
	if (rockIndex < 0 || rockIndex >= rockCount) {
		return false;
	}
	const WyrmRock& rock = rocks[rockIndex];
	const WyrmRockBoundaryCache& cache = rockBoundaryCaches[rockIndex];
	const bool cacheMatches =
		cache.valid &&
		cache.phase == rock.phase &&
		cache.value == rock.value &&
		cache.radiusPhase == rock.radiusPhase &&
		cache.radiusValue == rock.radiusValue;
	if (!cacheMatches) {
		return rockBoundsAtPhase(rock, ph, lower, upper);
	}
	const float rx = rock.radiusPhase + rockClearancePhase(rock);
	const float dx = rockDx(ph, rock);
	if (std::fabs(dx) >= rx) {
		return false;
	}
	const float x = (0.5f + 0.5f * dx / std::max(rx, 1e-4f)) * float(kWyrmRockBoundarySamples - 1);
	const int i0 = clamp(int(std::floor(x)), 0, kWyrmRockBoundarySamples - 1);
	const int i1 = std::min(i0 + 1, kWyrmRockBoundarySamples - 1);
	const float t = x - float(i0);
	if (lower) {
		*lower = cache.lower[i0] + (cache.lower[i1] - cache.lower[i0]) * t;
	}
	if (upper) {
		*upper = cache.upper[i0] + (cache.upper[i1] - cache.upper[i0]) * t;
	}
	return true;
}

bool Wyrm::cachedRockBoundsAtPhase(const WyrmRockStateSnapshot& state, int rockIndex, float ph, float* lower, float* upper) {
	if (rockIndex < 0 || rockIndex >= state.rockCount) {
		return false;
	}
	const WyrmRock& rock = state.rocks[rockIndex];
	const WyrmRockBoundaryCache& cache = state.rockBoundaryCaches[rockIndex];
	const bool cacheMatches =
		cache.valid &&
		cache.phase == rock.phase &&
		cache.value == rock.value &&
		cache.radiusPhase == rock.radiusPhase &&
		cache.radiusValue == rock.radiusValue;
	if (!cacheMatches) {
		const float rx = state.defaultRx[rockIndex];
		const float dxRaw = levi_math::wrap01(ph) - state.wrappedPhase[rockIndex];
		float dx = dxRaw;
		if (dx > 0.5f) dx -= 1.f;
		else if (dx < -0.5f) dx += 1.f;
		if (std::fabs(dx) >= rx) {
			return false;
		}
		const float nx = dx * state.defaultInvRx[rockIndex];
		const float radiusValue = state.defaultRadiusValue[rockIndex];
		const float edgeY = radiusValue * std::sqrt(std::max(0.f, 1.f - nx * nx));
		if (edgeY <= 0.f) {
			return false;
		}
		if (lower) *lower = rock.value - edgeY;
		if (upper) *upper = rock.value + edgeY;
		return true;
	}
	const float rx = state.defaultRx[rockIndex];
	float dx = levi_math::wrap01(ph) - state.wrappedPhase[rockIndex];
	if (dx > 0.5f) dx -= 1.f;
	else if (dx < -0.5f) dx += 1.f;
	if (std::fabs(dx) >= rx) {
		return false;
	}
	const float x = (0.5f + 0.5f * dx * state.defaultInvRx[rockIndex]) * float(kWyrmRockBoundarySamples - 1);
	const int i0 = clamp(int(std::floor(x)), 0, kWyrmRockBoundarySamples - 1);
	const int i1 = std::min(i0 + 1, kWyrmRockBoundarySamples - 1);
	const float t = x - float(i0);
	if (lower) {
		*lower = cache.lower[i0] + (cache.lower[i1] - cache.lower[i0]) * t;
	}
	if (upper) {
		*upper = cache.upper[i0] + (cache.upper[i1] - cache.upper[i0]) * t;
	}
	return true;
}

bool Wyrm::rockBoundsAtPhase(const WyrmRock& rock, float ph, float* lower, float* upper) const {
	return rockBoundsAtPhase(rock, ph, kWyrmRockClearance, lower, upper);
}

bool Wyrm::rockBoundsAtPhase(const WyrmRock& rock, float ph, float clearanceValue, float* lower, float* upper) const {
	return rockBoundsAtPhase(rock, ph, clearanceValue, (clearanceValue > 0.f) ? rockClearancePhase(rock, clearanceValue) : 0.f, lower, upper);
}

bool Wyrm::rockBoundsAtPhase(const WyrmRock& rock, float ph, float clearanceValue, float clearancePhase, float* lower, float* upper) const {
	const float edgeY = rockEdgeY(rock, rockDx(ph, rock), clearanceValue, clearancePhase);
	if (edgeY <= 0.f) {
		return false;
	}
	if (lower) *lower = rock.value - edgeY;
	if (upper) *upper = rock.value + edgeY;
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

void Wyrm::sculptWaveAroundRock(int rockIndex, const WyrmRock* previousRock) {
	if (rockIndex < 0 || rockIndex >= rockCount || pointCount <= 0) return;
	const WyrmRock& rock = rocks[rockIndex];
	std::array<int, kWyrmPointCountMax> sideVote {};
	std::array<float, kWyrmPointCountMax> local {};
	std::array<bool, kWyrmPointCountMax> touched {};
	std::array<bool, kWyrmPointCountMax> rockSegmentHit {};
	for (int i = 0; i < pointCount; ++i) {
		local[i] = wavePoints[i].load(std::memory_order_relaxed);
	}
	const float pointSpacing = 1.f / float(std::max(pointCount, 1));
	const float rx = rock.radiusPhase + rockClearancePhase(rock);
	const float sculptWindow = rx + 2.f * pointSpacing;

	auto addSegmentVote = [&](const WyrmRock& testRock, int i0, int i1, bool trackCurrentRock) {
		const float ph0 = (float(i0) + 0.5f) / float(pointCount);
		const float ph1 = (float(i1) + 0.5f) / float(pointCount);
		bool preferUpper = false;
		if (segmentIntersectsRockBounds(testRock, ph0, local[i0], ph1, local[i1], &preferUpper)) {
			const int vote = preferUpper ? 1 : -1;
			sideVote[i0] += vote;
			sideVote[i1] += vote;
			if (trackCurrentRock) {
				rockSegmentHit[i0] = true;
			}
		}
	};

	for (int i = 0; i < pointCount; ++i) {
		addSegmentVote(rock, i, (i + 1) % pointCount, true);
	}
	if (previousRock) {
		for (int i = 0; i < pointCount; ++i) {
			addSegmentVote(*previousRock, i, (i + 1) % pointCount, false);
		}
	}

	bool changed = false;
	for (int i = 0; i < pointCount; ++i) {
		const float ph = (float(i) + 0.5f) / float(pointCount);
		const float dx = std::fabs(rockDx(ph, rock));
		const bool nearRock = (dx <= sculptWindow);
		if (!nearRock && sideVote[i] == 0) {
			continue;
		}

		float lower = 0.f;
		float upper = 0.f;
		const bool hasBounds = rockBoundsAtPhase(rock, ph, &lower, &upper);
		if (!hasBounds && sideVote[i] == 0) {
			continue;
		}
		if (!hasBounds) {
			const float guard = kWyrmRockValueScale * (rock.radiusValue + kWyrmRockClearance);
			lower = rock.value - guard;
			upper = rock.value + guard;
		}

		const float y = local[i];
		const bool inside = (y > lower && y < upper);
		if (!inside && sideVote[i] == 0 && !nearRock) {
			continue;
		}

		bool preferUpper = sideVote[i] > 0;
		if (sideVote[i] == 0) {
			preferUpper = y >= rock.value;
		}
		const float sculpted = clamp(preferUpper ? upper : lower, -1.f, 1.f);
		if ((preferUpper && y < sculpted) || (!preferUpper && y > sculpted)) {
			wavePoints[i].store(sculpted, std::memory_order_relaxed);
			local[i] = sculpted;
			touched[i] = true;
			changed = true;
		}
	}

	// Enforce that intersecting segments cannot remain crossing between points.
	for (int pass = 0; pass < 2; ++pass) {
		bool passChanged = false;
		for (int i = 0; i < pointCount; ++i) {
			if (!rockSegmentHit[i]) {
				continue;
			}
			const int j = (i + 1) % pointCount;
			const float ph0 = (float(i) + 0.5f) / float(pointCount);
			const float ph1 = (float(j) + 0.5f) / float(pointCount);
			bool preferUpper = false;
			if (!segmentIntersectsRockBounds(rock, ph0, local[i], ph1, local[j], &preferUpper)) {
				continue;
			}

			auto projectEndpoint = [&](int idx, float ph, bool preferUp) {
				float lower = 0.f;
				float upper = 0.f;
				if (!rockBoundsAtPhase(rock, ph, &lower, &upper)) {
					return;
				}
				const float y = local[idx];
				const float projected = clamp(preferUp ? upper : lower, -1.f, 1.f);
				if ((preferUp && y < projected) || (!preferUp && y > projected)) {
					local[idx] = projected;
					touched[idx] = true;
					passChanged = true;
				}
			};

			projectEndpoint(i, ph0, preferUpper);
			projectEndpoint(j, ph1, preferUpper);
		}
		if (!passChanged) {
			break;
		}
		changed = true;
	}

	// The drawn body is sampled between stored points. When a rock sits between
	// points, visual collision resolution can bend the curve without any point
	// center entering the rock. Imprint those between-point bends back into the
	// adjacent points so dragging a rock leaves the expected deformation behind.
	for (int i = 0; i < pointCount; ++i) {
		const int j = (i + 1) % pointCount;
		for (float t : {0.25f, 0.5f, 0.75f}) {
			const float ph = levi_math::wrap01((float(i) + 0.5f + t) / float(pointCount));
			if (std::fabs(rockDx(ph, rock)) > rx + pointSpacing) {
				continue;
			}

			float lower = 0.f;
			float upper = 0.f;
			if (!rockBoundsAtPhase(rock, ph, &lower, &upper)) {
				continue;
			}

			const float y = catmullPeriodic(local, pointCount, ph);
			if (y <= lower || y >= upper) {
				continue;
			}

			bool preferUpper = y >= rock.value;
			const int segmentVote = sideVote[i] + sideVote[j];
			if (segmentVote != 0) {
				preferUpper = segmentVote > 0;
			}

			const float projected = clamp(preferUpper ? upper : lower, -1.f, 1.f);
			const float correction = projected - y;
			if (std::fabs(correction) <= 1e-6f) {
				continue;
			}

			auto moveEndpoint = [&](int idx) {
				const float nextValue = clamp(local[idx] + correction, -1.f, 1.f);
				if (std::fabs(nextValue - local[idx]) > 1e-6f) {
					local[idx] = nextValue;
					wavePoints[idx].store(nextValue, std::memory_order_relaxed);
					touched[idx] = true;
					changed = true;
				}
			};
			moveEndpoint(i);
			moveEndpoint(j);
		}
	}

	if (changed) {
		for (int pass = 0; pass < 2; ++pass) {
			std::array<float, kWyrmPointCountMax> smoothed = local;
			for (int i = 0; i < pointCount; ++i) {
				const int prev = (i + pointCount - 1) % pointCount;
				const int next = (i + 1) % pointCount;
				if (!touched[i] && !touched[prev] && !touched[next]) {
					continue;
				}

				const float ph = (float(i) + 0.5f) / float(pointCount);
				if (std::fabs(rockDx(ph, rock)) > rx + pointSpacing) {
					continue;
				}

				const float relaxed = 0.5f * local[i] + 0.25f * (local[prev] + local[next]);
				float candidate = clamp(0.65f * local[i] + 0.35f * relaxed, -1.f, 1.f);

				float lower = 0.f;
				float upper = 0.f;
				if (rockBoundsAtPhase(rock, ph, &lower, &upper)) {
					const bool preferUpper = (sideVote[i] > 0) || (sideVote[i] == 0 && local[i] >= rock.value);
					if (candidate > lower && candidate < upper) {
						candidate = preferUpper ? upper : lower;
					}
				}

				smoothed[i] = candidate;
			}
			for (int i = 0; i < pointCount; ++i) {
				if (std::fabs(smoothed[i] - local[i]) > 1e-6f) {
					local[i] = smoothed[i];
					wavePoints[i].store(local[i], std::memory_order_relaxed);
				}
			}
		}
	}

	if (changed) {
		waveCustomized = true;
		waveVersion.fetch_add(1u, std::memory_order_release);
	}
}

float Wyrm::applyRockPush(float base, float ph) const {
	return resolveAgainstRocks(base, base, ph);
}

float Wyrm::applyRockClamp(float base, float ph, float offset) const {
	if (rockCount <= 0 || std::fabs(offset) <= 1e-6f) {
		return offset;
	}
	return resolveAgainstRocks(base, base + offset, ph) - base;
}

float Wyrm::applyRockPush(const WyrmRockStateSnapshot& state, float base, float ph) {
	return resolveAgainstRocks(state, base, base, ph, kWyrmRockClearance, -1.f);
}

float Wyrm::applyRockClamp(const WyrmRockStateSnapshot& state, float base, float ph, float offset) {
	if (state.rockCount <= 0 || std::fabs(offset) <= 1e-6f) {
		return offset;
	}
	return resolveAgainstRocks(state, base, base + offset, ph, kWyrmRockClearance, -1.f) - base;
}

float Wyrm::resolveAgainstRocks(float anchorY, float desiredY, float ph, float clearanceValue) const {
	return resolveAgainstRocks(anchorY, desiredY, ph, clearanceValue, -1.f);
}

float Wyrm::resolveAgainstRocks(float anchorY, float desiredY, float ph, float clearanceValue, float clearancePhase) const {
	if (rockCount <= 0) {
		return clamp(desiredY, -1.f, 1.f);
	}
	const bool useCachedDefault = (clearanceValue == kWyrmRockClearance && clearancePhase < 0.f);
	float y = clamp(desiredY, -1.f, 1.f);
	for (int pass = 0; pass < 3; ++pass) {
		bool changed = false;
		for (int i = 0; i < rockCount; ++i) {
			if (i == liftedRock) continue;
			const WyrmRock& rock = rocks[i];
			const float effectiveClearancePhase = (clearancePhase >= 0.f)
				? clearancePhase
				: ((clearanceValue > 0.f) ? rockClearancePhase(rock, clearanceValue) : 0.f);
			const float rx = rock.radiusPhase + std::max(0.f, effectiveClearancePhase);
			const float dx = rockDx(ph, rock);
			// Fast phase-window reject before any bounds interpolation/projection work.
			if (std::fabs(dx) >= rx) {
				continue;
			}

			float lower = 0.f;
			float upper = 0.f;
			bool hasBounds = false;
			if (useCachedDefault) {
				hasBounds = cachedRockBoundsAtPhase(i, ph, &lower, &upper);
			}
			else {
				const float invRx = 1.f / std::max(rx, 1e-4f);
				const float nx = dx * invRx;
				const float radiusValue = kWyrmRockValueScale * (rock.radiusValue + clearanceValue);
				const float edgeY = radiusValue * std::sqrt(std::max(0.f, 1.f - nx * nx));
				if (edgeY > 0.f) {
					lower = rock.value - edgeY;
					upper = rock.value + edgeY;
					hasBounds = true;
				}
			}
			if (!hasBounds) continue;
			const bool anchorInside = (anchorY > lower && anchorY < upper);
			const bool yInside = (y > lower && y < upper);
			const bool crossesUp = (anchorY <= lower && y >= lower);
			const bool crossesDown = (anchorY >= upper && y <= upper);
			if (anchorInside || yInside || crossesUp || crossesDown) {
				const float projected = foldAwayFromRock(anchorY, y, lower, upper);
				if (std::fabs(projected - y) > 1e-6f) {
					y = projected;
					changed = true;
				}
			}
		}
		if (!changed) {
			break;
		}
	}
	return clamp(y, -1.f, 1.f);
}

float Wyrm::resolveAgainstRocks(const WyrmRockStateSnapshot& state, float anchorY, float desiredY, float ph, float clearanceValue, float clearancePhase) {
	if (state.rockCount <= 0) {
		return clamp(desiredY, -1.f, 1.f);
	}
	const bool useCachedDefault = (clearanceValue == kWyrmRockClearance && clearancePhase < 0.f);
	const float phWrapped = levi_math::wrap01(ph);
	std::array<uint8_t, kWyrmMaxRocks> rockHasBounds {};
	std::array<float, kWyrmMaxRocks> rockLower {};
	std::array<float, kWyrmMaxRocks> rockUpper {};
	for (int i = 0; i < state.rockCount; ++i) {
		if (i == state.liftedRock) continue;
		const WyrmRock& rock = state.rocks[i];
		const float effectiveClearancePhase = (clearancePhase >= 0.f)
			? clearancePhase
			: ((clearanceValue > 0.f) ? state.defaultClearancePhase[i] : 0.f);
		const float rx = useCachedDefault ? state.defaultRx[i] : (rock.radiusPhase + std::max(0.f, effectiveClearancePhase));
		const float invRx = useCachedDefault ? state.defaultInvRx[i] : (1.f / std::max(rx, 1e-4f));
		float dx = phWrapped - state.wrappedPhase[i];
		if (dx > 0.5f) dx -= 1.f;
		else if (dx < -0.5f) dx += 1.f;
		if (std::fabs(dx) >= rx) {
			continue;
		}
		float lower = 0.f;
		float upper = 0.f;
		bool hasBounds = false;
		if (useCachedDefault) {
			hasBounds = cachedRockBoundsAtPhase(state, i, ph, &lower, &upper);
		}
		else {
			const float nx = dx * invRx;
			const float radiusValue = kWyrmRockValueScale * (rock.radiusValue + clearanceValue);
			const float edgeY = radiusValue * std::sqrt(std::max(0.f, 1.f - nx * nx));
			if (edgeY > 0.f) {
				lower = rock.value - edgeY;
				upper = rock.value + edgeY;
				hasBounds = true;
			}
		}
		if (hasBounds) {
			rockHasBounds[i] = 1u;
			rockLower[i] = lower;
			rockUpper[i] = upper;
		}
	}
	float y = clamp(desiredY, -1.f, 1.f);
	for (int pass = 0; pass < 3; ++pass) {
		bool changed = false;
		for (int i = 0; i < state.rockCount; ++i) {
			if (!rockHasBounds[i]) continue;
			const float lower = rockLower[i];
			const float upper = rockUpper[i];
			const bool anchorInside = (anchorY > lower && anchorY < upper);
			const bool yInside = (y > lower && y < upper);
			const bool crossesUp = (anchorY <= lower && y >= lower);
			const bool crossesDown = (anchorY >= upper && y <= upper);
			if (anchorInside || yInside || crossesUp || crossesDown) {
				const float projected = foldAwayFromRock(anchorY, y, lower, upper);
				if (std::fabs(projected - y) > 1e-6f) {
					y = projected;
					changed = true;
				}
			}
		}
		if (!changed) {
			break;
		}
	}
	return clamp(y, -1.f, 1.f);
}

json_t* Wyrm::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "editorLocked", json_boolean(editorLocked.load(std::memory_order_relaxed)));
	json_object_set_new(root, "sandViewEnabled", json_boolean(sandViewEnabled.load(std::memory_order_relaxed)));
	json_object_set_new(root, "renderMode", json_integer(renderMode.load(std::memory_order_relaxed)));
	json_object_set_new(root, "sandBackend", json_integer(sandBackend.load(std::memory_order_relaxed)));
	json_object_set_new(root, "sandDetail", json_integer(sandDetail.load(std::memory_order_relaxed)));
	json_object_set_new(root, "sandPersistence", json_integer(sandPersistence.load(std::memory_order_relaxed)));
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
	json_t* lockJ = json_object_get(root, "editorLocked");
	if (lockJ) editorLocked.store(json_is_true(lockJ), std::memory_order_relaxed);
	json_t* sandViewJ = json_object_get(root, "sandViewEnabled");
	if (sandViewJ) sandViewEnabled.store(json_is_true(sandViewJ), std::memory_order_relaxed);
	json_t* renderModeJ = json_object_get(root, "renderMode");
	if (renderModeJ) {
		const int mode = clamp(int(json_integer_value(renderModeJ)), WYRM_RENDER_NANOVG, WYRM_RENDER_OPENGL_SHDR);
		renderMode.store(mode, std::memory_order_relaxed);
	}
	json_t* sandBackendJ = json_object_get(root, "sandBackend");
	if (sandBackendJ) {
		const int v = clamp(int(json_integer_value(sandBackendJ)), WYRMSAND_NANOVG_CELLS, WYRMSAND_SHADER_FEEDBACK);
		// Legacy "NanoVG Cells" backend now maps to NanoVG Image.
		const int backend = (v == WYRMSAND_NANOVG_CELLS) ? WYRMSAND_NANOVG_IMAGE : v;
		sandBackend.store(backend, std::memory_order_relaxed);
		// If no explicit renderMode was saved, infer mode from backend for compatibility.
		if (!renderModeJ) {
			int mode = WYRM_RENDER_NANOVG;
			if (backend == WYRMSAND_OPENGL_TEXTURE) mode = WYRM_RENDER_OPENGL;
			else if (backend == WYRMSAND_SHADER_FEEDBACK) mode = WYRM_RENDER_OPENGL_SHDR;
			renderMode.store(mode, std::memory_order_relaxed);
		}
	}
	json_t* sandDetailJ = json_object_get(root, "sandDetail");
	if (sandDetailJ) {
		const int v = clamp(int(json_integer_value(sandDetailJ)), WYRMSAND_DETAIL_LOW, WYRMSAND_DETAIL_AUTO);
		sandDetail.store(v, std::memory_order_relaxed);
	}
	json_t* sandPersistenceJ = json_object_get(root, "sandPersistence");
	if (sandPersistenceJ) {
		const int v = clamp(int(json_integer_value(sandPersistenceJ)), WYRMSAND_PERSISTENCE_SHORT, WYRMSAND_PERSISTENCE_LONG);
		sandPersistence.store(v, std::memory_order_relaxed);
	}
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
			if (phaseJ) rocks[i].phase = levi_math::wrap01(float(json_number_value(phaseJ)));
			if (valueJ) rocks[i].value = clamp(float(json_number_value(valueJ)), -1.f, 1.f);
			if (radiusPhaseJ) rocks[i].radiusPhase = clamp(float(json_number_value(radiusPhaseJ)), 0.02f, 0.09f);
			if (radiusValueJ) rocks[i].radiusValue = clamp(float(json_number_value(radiusValueJ)), 0.06f, 0.24f);
			if (seedJ) rocks[i].seed = uint32_t(json_integer_value(seedJ));
		}
		rebuildAllRockBoundaryCaches();
	}
	publishRockState();
}

void Wyrm::process(const ProcessArgs& args) {
	using PerfClock = std::chrono::steady_clock;
	const bool measurePerf = isDragonKingDebugEnabled() && perfMeasureDivider.process();
	const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
	bool wavetableRebuilt = false;
	const uint32_t v = waveVersion.load(std::memory_order_acquire);
	if (v != appliedWaveVersion) {
		rebuildWavetable();
		appliedWaveVersion = v;
		wavetableRebuilt = true;
	}

	const int channels = std::max(1, std::max({
		inputs[VOCT_INPUT].getChannels(),
		inputs[FM_INPUT].getChannels(),
		inputs[SYNC_INPUT].getChannels(),
		inputs[FOLD_CV_INPUT].getChannels(),
		inputs[SLITHER_CV_INPUT].getChannels(),
		inputs[SLITHER_SPEED_CV_INPUT].getChannels()
	}));
	outputs[OUT_OUTPUT].setChannels(channels);
	outputs[RAW_OUTPUT].setChannels(channels);

	const float knobNorm = levi_math::clamp01(params[FREQ_PARAM].getValue());
	const bool lfoModeNow = params[LFO_MODE_PARAM].getValue() > 0.5f;
	const bool envelopeModeNow = params[ENV_MODE_PARAM].getValue() > 0.5f;
	const bool softSyncModeNow = params[SYNC_MODE_PARAM].getValue() > 0.5f;
	const bool coarseStepModeNow = params[COARSE_STEP_MODE_PARAM].getValue() > 0.5f;
	lfoMode.store(lfoModeNow, std::memory_order_relaxed);
	envelopeMode.store(envelopeModeNow, std::memory_order_relaxed);
	lights[LFO_MODE_LIGHT].setBrightness(lfoModeNow ? 0.5f : 0.f);
	lights[SYNC_MODE_LIGHT].setBrightness(softSyncModeNow ? 0.5f : 0.f);
	lights[ENV_MODE_LIGHT].setBrightness(envelopeModeNow ? 0.5f : 0.f);
	lights[COARSE_STEP_MODE_LIGHT].setBrightness(coarseStepModeNow ? 0.5f : 0.f);
	const bool slowRateModeNow = lfoModeNow || envelopeModeNow;
	float baseFreq = wyrmBaseFrequencyFromKnob(knobNorm, slowRateModeNow);
	if (coarseStepModeNow) {
		const float coarsePitch = std::log2(std::max(baseFreq, 1e-6f) / dsp::FREQ_C4);
		baseFreq = dsp::FREQ_C4 * dsp::approxExp2_taylor5(std::round(coarsePitch));
	}
	const float fmAtten = finiteOr(params[FM_ATTEN_PARAM].getValue());
	const float fine = finiteOr(params[FINE_PARAM].getValue()) / 1200.f;
	const float displayRateNoFm = clamp(
		baseFreq * rack::dsp::exp2_taylor5(fine),
		0.005f,
		0.45f * args.sampleRate);
	if (envelopeModeNow) {
		displayEnvelopeTimeMs.store(1000.f / std::max(displayRateNoFm, 1e-6f), std::memory_order_relaxed);
	}
	if (envelopeModeNow != envelopeModeWasActive) {
		if (envelopeModeNow) {
			setEnvelopeArShape();
			rebuildWavetable();
			appliedWaveVersion = waveVersion.load(std::memory_order_acquire);
		}
		for (int c = 0; c < kWyrmMaxChannels; ++c) {
			phase[c] = 0.f;
			phaseDir[c] = 1.f;
			envelopeRunning[c] = false;
			envelopeTriggers[c].reset();
		}
		displayPhase.store(0.f, std::memory_order_relaxed);
		displayPhaseFrequencyHz.store(0.f, std::memory_order_relaxed);
		displayEnvelopeRunning.store(false, std::memory_order_relaxed);
		publishedEnvelopeRunning = false;
		envelopeModeWasActive = envelopeModeNow;
	}
	const float foldBase = finiteOr(params[FOLD_PARAM].getValue());
	const float slitherAmountKnob = levi_math::clamp01(params[SLITHER_PARAM].getValue());
	const float slitherSpeedKnob = levi_math::clamp01(params[SLITHER_SPEED_PARAM].getValue());
	const WyrmRockStateSnapshot& activeRockStateNow = getActiveRockState();
	const bool hasRocks = activeRockStateNow.rockCount > 0;
	const bool voctPoly = inputs[VOCT_INPUT].getChannels() > 1;
	const bool fmPoly = inputs[FM_INPUT].getChannels() > 1;
	const bool slitherAmountCvPoly = inputs[SLITHER_CV_INPUT].getChannels() > 1;
	const bool slitherSpeedCvPoly = inputs[SLITHER_SPEED_CV_INPUT].getChannels() > 1;
	const bool foldCvPoly = inputs[FOLD_CV_INPUT].getChannels() > 1;
	const bool voctConnected = inputs[VOCT_INPUT].isConnected();
	const bool fmConnected = inputs[FM_INPUT].isConnected();
	const bool syncConnected = inputs[SYNC_INPUT].isConnected();
	const bool slitherAmountCvConnected = inputs[SLITHER_CV_INPUT].isConnected();
	const bool slitherSpeedCvConnected = inputs[SLITHER_SPEED_CV_INPUT].isConnected();
	const bool foldCvConnected = inputs[FOLD_CV_INPUT].isConnected();
	auto readPolyOrMonoVoltage = [&](InputId id, int channel, bool poly, float monoValue) {
		if (poly) {
			return finiteOr(inputs[id].getPolyVoltage(channel));
		}
		return monoValue;
	};
	auto readBipolarCvNorm = [&](InputId id, bool connected, int channel, bool poly, float monoNorm) {
		if (!connected) {
			return 0.f;
		}
		if (poly) {
			return clamp(finiteOr(inputs[id].getPolyVoltage(channel)) / 10.f, -1.f, 1.f);
		}
		return monoNorm;
	};
	const float monoSlitherAmountCvNorm = slitherAmountCvConnected ? clamp(finiteOr(inputs[SLITHER_CV_INPUT].getVoltage()) / 10.f, -1.f, 1.f) : 0.f;
	const float monoSlitherSpeedCvNorm = slitherSpeedCvConnected ? clamp(finiteOr(inputs[SLITHER_SPEED_CV_INPUT].getVoltage()) / 10.f, -1.f, 1.f) : 0.f;
	const float monoFoldCvNorm = foldCvConnected ? clamp(finiteOr(inputs[FOLD_CV_INPUT].getVoltage()) / 10.f, -1.f, 1.f) : 0.f;
	auto effectiveSlitherAmount = [&](int channel) {
		const float cv = readBipolarCvNorm(SLITHER_CV_INPUT, slitherAmountCvConnected, channel, slitherAmountCvPoly, monoSlitherAmountCvNorm);
		return clamp(slitherAmountKnob + cv, 0.f, 1.f);
	};
	auto effectiveSlitherSpeed = [&](int channel) {
		const float cv = readBipolarCvNorm(SLITHER_SPEED_CV_INPUT, slitherSpeedCvConnected, channel, slitherSpeedCvPoly, monoSlitherSpeedCvNorm);
		return slitherSpeedFactor(clamp(slitherSpeedKnob + cv, 0.f, 1.f));
	};
	auto effectiveFoldAmt = [&](int channel) {
		const float cv = readBipolarCvNorm(FOLD_CV_INPUT, foldCvConnected, channel, foldCvPoly, monoFoldCvNorm);
		return clamp(foldBase + cv, 0.f, 2.f);
	};
	const float monoVoct = voctConnected ? finiteOr(inputs[VOCT_INPUT].getVoltage()) : 0.f;
	const float monoFmVoltage = fmConnected ? finiteOr(inputs[FM_INPUT].getVoltage()) : 0.f;
	const float monoSlitherAmount = effectiveSlitherAmount(0);
	const float monoSlitherSpeed = effectiveSlitherSpeed(0);
	const float monoFoldAmt = effectiveFoldAmt(0);
	const bool slitherActiveNow = slitherAmountCvConnected || slitherAmountKnob > 1e-5f;
	const bool foldActiveNow = foldCvConnected || foldBase > 1e-5f;
	const bool monoPitchModulation = !voctPoly && !fmPoly;
	const bool canUsePlainFastPath =
		!envelopeModeNow &&
		!hasRocks &&
		!slitherActiveNow &&
		!foldActiveNow &&
		monoPitchModulation &&
		!slitherSpeedCvPoly;
	float fastPathHz = 0.f;
	float fastPathDisplayHzNoFm = 0.f;
	float fastPathPhaseStep = 0.f;
	float fastPathSlitherStep = 0.f;
	float phaseDisplay = 0.f;
	float phaseFrequencyDisplay = 0.f;
	if (canUsePlainFastPath) {
		fastPathDisplayHzNoFm = clamp(baseFreq * rack::dsp::exp2_taylor5(monoVoct + fine), 0.005f, 0.45f * args.sampleRate);
		float hz = baseFreq * rack::dsp::exp2_taylor5(monoVoct + monoFmVoltage * fmAtten + fine);
		hz = finiteOr(hz, baseFreq);
		fastPathHz = clamp(hz, 0.005f, 0.45f * args.sampleRate);
		fastPathPhaseStep = fastPathHz * args.sampleTime;
		const float slitherBaseHz = lfoModeNow ? clamp(fastPathHz, 0.01f, 8.f) : clamp(0.125f * fastPathHz, 0.15f, 8.f);
		const float slitherHz = clamp(slitherBaseHz * monoSlitherSpeed, 0.01f, 16.f);
		fastPathSlitherStep = slitherHz * args.sampleTime;
	}

	for (int c = 0; c < channels; ++c) {
		const float voct = readPolyOrMonoVoltage(VOCT_INPUT, c, voctPoly, monoVoct);
		if (envelopeModeNow && envelopeTriggers[c].process(voct)) {
			phase[c] = 0.f;
			phaseDir[c] = 1.f;
			envelopeRunning[c] = true;
			if (c == 0) {
				displayPhase.store(0.f, std::memory_order_relaxed);
			}
		}
		if (syncConnected) {
			const float s = inputs[SYNC_INPUT].getPolyVoltage(c);
			if (syncTriggers[c].process(s) && !envelopeModeNow) {
				if (softSyncModeNow) {
					phaseDir[c] = -phaseDir[c];
					if (std::fabs(phaseDir[c]) < 0.5f) {
						phaseDir[c] = -1.f;
					}
				}
				else {
					phase[c] = 0.f;
					slitherPhase[c] = 0.f;
					phaseDir[c] = 1.f;
				}
			}
		}
		if (canUsePlainFastPath) {
			phase[c] = levi_math::wrap01Fast(phase[c] + (softSyncModeNow ? (phaseDir[c] * fastPathPhaseStep) : fastPathPhaseStep));
			slitherPhase[c] = levi_math::wrap01Fast(slitherPhase[c] + fastPathSlitherStep);
			if (c == 0) {
				displayFrequencyHz.store(fastPathDisplayHzNoFm, std::memory_order_relaxed);
				displaySlitherAmount.store(monoSlitherAmount, std::memory_order_relaxed);
				displaySlitherSpeedFactor.store(monoSlitherSpeed, std::memory_order_relaxed);
				displaySlitherPhase.store(slitherPhase[c], std::memory_order_relaxed);
			}
			const float raw = clamp(finiteOr(lookupWave(phase[c], fastPathPhaseStep)), -1.f, 1.f);
			outputs[RAW_OUTPUT].setVoltage(5.f * raw, c);
			outputs[OUT_OUTPUT].setVoltage(5.f * raw, c);
			if (c == 0) {
				phaseDisplay = phase[c];
				phaseFrequencyDisplay = fastPathHz;
			}
			continue;
		}
		const float fm = readPolyOrMonoVoltage(FM_INPUT, c, fmPoly, monoFmVoltage) * fmAtten;
		const float slitherAmount = slitherAmountCvPoly ? effectiveSlitherAmount(c) : monoSlitherAmount;
		const float slitherSpeed = slitherSpeedCvPoly ? effectiveSlitherSpeed(c) : monoSlitherSpeed;
		const float pitchOrTimeMod = envelopeModeNow ? (fm + fine) : (voct + fm + fine);
		float hz = baseFreq * rack::dsp::exp2_taylor5(pitchOrTimeMod);
		hz = finiteOr(hz, baseFreq);
		hz = clamp(hz, 0.005f, 0.45f * args.sampleRate);
		if (c == 0) {
			const float displayHzNoFm = envelopeModeNow
				? displayRateNoFm
				: clamp(baseFreq * rack::dsp::exp2_taylor5(voct + fine), 0.005f, 0.45f * args.sampleRate);
			displayFrequencyHz.store(displayHzNoFm, std::memory_order_relaxed);
			displaySlitherAmount.store(slitherAmount, std::memory_order_relaxed);
			displaySlitherSpeedFactor.store(slitherSpeed, std::memory_order_relaxed);
		}
		const float phaseStep = hz * args.sampleTime;
		if (!envelopeModeNow) {
			phase[c] = levi_math::wrap01Fast(phase[c] + (softSyncModeNow ? (phaseDir[c] * phaseStep) : phaseStep));
		}
		const float slitherBaseHz = slowRateModeNow ? clamp(hz, 0.01f, 8.f) : clamp(0.125f * hz, 0.15f, 8.f);
		const float slitherHz = clamp(slitherBaseHz * slitherSpeed, 0.01f, 16.f);
		slitherPhase[c] = levi_math::wrap01Fast(slitherPhase[c] + slitherHz * args.sampleTime);
		if (c == 0) {
			displaySlitherPhase.store(slitherPhase[c], std::memory_order_relaxed);
		}
		const bool renderEnvelopeVoice = !envelopeModeNow || envelopeRunning[c];
		float raw = 0.f;
		if (renderEnvelopeVoice) {
			const float baseWave = finiteOr(lookupWave(phase[c], phaseStep));
			const float base = hasRocks ? finiteOr(applyRockPush(activeRockStateNow, baseWave, phase[c])) : baseWave;
			float slither = 0.f;
			if (slitherAmount > 1e-5f) {
				const float slitherTarget = slitherOffset(phase[c], slitherPhase[c], slitherAmount);
				slither = hasRocks ? finiteOr(applyRockClamp(activeRockStateNow, base, phase[c], slitherTarget)) : slitherTarget;
			}
			raw = clamp(finiteOr(base + slither), -1.f, 1.f);
		}
		const float foldAmt = foldCvPoly ? effectiveFoldAmt(c) : monoFoldAmt;
		float folded = raw;
		if (foldAmt > 1e-5f) {
			folded = clamp(finiteOr(levi_math::softClip(foldWave(raw, foldAmt)) * kWyrmFoldMakeupGain), -1.f, 1.f);
		}
		const float rawVoltage = envelopeModeNow
			? (renderEnvelopeVoice ? 5.f * (raw + 1.f) : 0.f)
			: 5.f * raw;
		const float foldedVoltage = envelopeModeNow
			? (renderEnvelopeVoice ? 5.f * (folded + 1.f) : 0.f)
			: 5.f * folded;
		outputs[RAW_OUTPUT].setVoltage(rawVoltage, c);
		outputs[OUT_OUTPUT].setVoltage(foldedVoltage, c);
		if (envelopeModeNow && envelopeRunning[c]) {
			const float nextPhase = phase[c] + phaseStep;
			if (nextPhase >= 1.f) {
				phase[c] = 0.f;
				envelopeRunning[c] = false;
			}
			else {
				phase[c] = nextPhase;
			}
		}
		if (c == 0) {
			phaseDisplay = phase[c];
			phaseFrequencyDisplay = (!envelopeModeNow || envelopeRunning[c]) ? hz : 0.f;
		}
	}
	const bool displayEnvelopeRunningNow = envelopeModeNow && envelopeRunning[0];
	if (displayEnvelopeRunningNow != publishedEnvelopeRunning) {
		displayEnvelopeRunning.store(displayEnvelopeRunningNow, std::memory_order_relaxed);
		publishedEnvelopeRunning = displayEnvelopeRunningNow;
	}

	phaseTracerPublishTimer += args.sampleTime;
	constexpr float phaseTracerPublishInterval = 1.f / 120.f;
	if (phaseTracerPublishTimer >= phaseTracerPublishInterval) {
		phaseTracerPublishTimer -= phaseTracerPublishInterval;
		if (phaseTracerPublishTimer >= phaseTracerPublishInterval) {
			phaseTracerPublishTimer = 0.f;
		}
		displayPhase.store(phaseDisplay, std::memory_order_relaxed);
		displayPhaseFrequencyHz.store(phaseFrequencyDisplay, std::memory_order_relaxed);
	}

	if (isDragonKingDebugEnabled()) {
		perfChannels.store(channels, std::memory_order_relaxed);
		perfFmConnected.store(fmConnected, std::memory_order_relaxed);
		perfFoldActive.store(foldActiveNow, std::memory_order_relaxed);
		perfSlitherActive.store(slitherActiveNow, std::memory_order_relaxed);
		perfLfoMode.store(lfoModeNow, std::memory_order_relaxed);
		if (wavetableRebuilt) {
			perfWavetableRebuilt.store(true, std::memory_order_release);
		}
		if (measurePerf) {
			const uint64_t elapsedNs = uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStart).count());
			perfAudioProcessNs.fetch_add(elapsedNs, std::memory_order_relaxed);
			perfAudioSampledCount.fetch_add(1u, std::memory_order_relaxed);
			debug_terminal::recordAudioProcessTiming(perfAudioProcessMinNs, perfAudioProcessMaxNs, elapsedNs);
		}
	}
}

float WyrmFreqQuantity::getDisplayValue() {
	auto* wyrm = static_cast<Wyrm*>(module);
	const bool envelopeModeNow = wyrm ? (wyrm->params[Wyrm::ENV_MODE_PARAM].getValue() > 0.5f) : false;
	if (envelopeModeNow) {
		return 1000.f / std::max(wyrmBaseFrequencyFromKnob(getValue(), true), 1e-6f);
	}
	const bool lfoModeNow = wyrm ? (wyrm->params[Wyrm::LFO_MODE_PARAM].getValue() > 0.5f) : false;
	return wyrmBaseFrequencyFromKnob(getValue(), lfoModeNow);
}

void WyrmFreqQuantity::setDisplayValue(float displayValue) {
	auto* wyrm = static_cast<Wyrm*>(module);
	const bool envelopeModeNow = wyrm ? (wyrm->params[Wyrm::ENV_MODE_PARAM].getValue() > 0.5f) : false;
	if (envelopeModeNow) {
		const float durationMs = std::max(displayValue, 1e-3f);
		setImmediateValue(wyrmKnobValueForFrequency(1000.f / durationMs, true));
		return;
	}
	const bool lfoModeNow = wyrm ? (wyrm->params[Wyrm::LFO_MODE_PARAM].getValue() > 0.5f) : false;
	setImmediateValue(wyrmKnobValueForFrequency(displayValue, lfoModeNow));
}

std::string WyrmFreqQuantity::getDisplayValueString() {
	auto* wyrm = static_cast<Wyrm*>(module);
	const bool envelopeModeNow = wyrm ? (wyrm->params[Wyrm::ENV_MODE_PARAM].getValue() > 0.5f) : false;
	const float value = getDisplayValue();
	if (envelopeModeNow) {
		if (value < 10.f) return string::f("%.2f ms", value);
		if (value < 100.f) return string::f("%.1f ms", value);
		return string::f("%.0f ms", value);
	}
	const float hz = value;
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
