#include "Wyrm.hpp"

#include <vector>

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;
	int hoveredRock = -1;
	int draggingRock = -1;
	int dragRockMouseMode = -1;
	bool pointEditActive = false;
	float pointEditSlitherPhase = 0.f;
	float visualSlitherPhase = 0.f;
	float renderedSlitherPhase = 0.f;
	double lastVisualUpdateSec = -1.0;
	double lastSandUpdateSec = -1.0;
	Vec rockDragOffset;
	WyrmRock previousDragRock {};
	bool sandInitialized = false;
	int sandW = 0;
	int sandH = 0;
	std::vector<float> sandDepth;
	std::vector<float> sandEnergy;
	std::vector<float> sandBaseNoise;
	std::array<Vec, kWyrmPointCountMax> previousWyrmPath {};
	int previousWyrmPathCount = 0;

	explicit WyrmWaveEditor(Wyrm* m) {
		module = m;
	}

	bool sandEnabled() const {
		return module && module->sandViewEnabled.load(std::memory_order_relaxed);
	}

	void resetSandHistory() {
		previousWyrmPathCount = 0;
		lastSandUpdateSec = -1.0;
	}

	void ensureSandField() {
		const int targetW = clamp(int(box.size.x * 0.65f), 64, 128);
		const int targetH = clamp(int(box.size.y * 0.65f), 32, 72);
		if (sandInitialized && sandW == targetW && sandH == targetH) {
			return;
		}
		sandW = targetW;
		sandH = targetH;
		const int cellCount = sandW * sandH;
		sandDepth.assign(cellCount, 0.f);
		sandEnergy.assign(cellCount, 0.f);
		sandBaseNoise.resize(cellCount);
		for (int y = 0; y < sandH; ++y) {
			for (int x = 0; x < sandW; ++x) {
				const uint32_t seed = 0x6d2b79f5u ^ uint32_t(x * 73856093) ^ uint32_t(y * 19349663);
				const float grain = hashUnit(seed);
				const float dune = 0.5f + 0.5f * std::sin(0.17f * float(x) + 0.31f * float(y));
				sandBaseNoise[y * sandW + x] = 0.72f * grain + 0.28f * dune;
			}
		}
		sandInitialized = true;
		resetSandHistory();
	}

	void stampSand(Vec pos, float radiusPx, float depthDelta, float energyDelta) {
		if (!sandInitialized || sandW <= 0 || sandH <= 0 || box.size.x <= 1.f || box.size.y <= 1.f) {
			return;
		}
		const float cellW = box.size.x / float(sandW);
		const float cellH = box.size.y / float(sandH);
		const int x0 = clamp(int(std::floor((pos.x - radiusPx) / cellW)), 0, sandW - 1);
		const int x1 = clamp(int(std::ceil((pos.x + radiusPx) / cellW)), 0, sandW - 1);
		const int y0 = clamp(int(std::floor((pos.y - radiusPx) / cellH)), 0, sandH - 1);
		const int y1 = clamp(int(std::ceil((pos.y + radiusPx) / cellH)), 0, sandH - 1);
		const float invRadius = 1.f / std::max(radiusPx, 1e-4f);
		for (int gy = y0; gy <= y1; ++gy) {
			const float cy = (float(gy) + 0.5f) * cellH;
			for (int gx = x0; gx <= x1; ++gx) {
				const float cx = (float(gx) + 0.5f) * cellW;
				const float dx = cx - pos.x;
				const float dy = cy - pos.y;
				const float dist = std::sqrt(dx * dx + dy * dy);
				const float falloff = smoother01(1.f - dist * invRadius);
				if (falloff <= 0.f) {
					continue;
				}
				const int idx = gy * sandW + gx;
				sandDepth[idx] = clamp(sandDepth[idx] + depthDelta * falloff, -1.f, 1.f);
				sandEnergy[idx] = clamp(sandEnergy[idx] + energyDelta * falloff, 0.f, 1.f);
			}
		}
	}

	void disturbSandSegment(Vec a, Vec b, float troughStrength, float ridgeStrength, float energyStrength) {
		if (!sandInitialized || sandW <= 0 || sandH <= 0 || box.size.x <= 1.f || box.size.y <= 1.f) {
			return;
		}
		Vec ab = b.minus(a);
		float len = std::sqrt(ab.x * ab.x + ab.y * ab.y);
		if (len < 1e-3f) {
			stampSand(a, 4.f, -troughStrength, energyStrength);
			return;
		}
		ab = ab.div(len);
		const Vec normal(-ab.y, ab.x);
		const float cellW = box.size.x / float(sandW);
		const float cellH = box.size.y / float(sandH);
		const float bodyRadiusPx = 3.1f;
		const float ridgeOffsetPx = 3.8f;
		const float effectRadiusPx = bodyRadiusPx + ridgeOffsetPx + 1.2f;
		const float minX = std::min(a.x, b.x) - effectRadiusPx;
		const float maxX = std::max(a.x, b.x) + effectRadiusPx;
		const float minY = std::min(a.y, b.y) - effectRadiusPx;
		const float maxY = std::max(a.y, b.y) + effectRadiusPx;
		const int x0 = clamp(int(std::floor(minX / cellW)), 0, sandW - 1);
		const int x1 = clamp(int(std::ceil(maxX / cellW)), 0, sandW - 1);
		const int y0 = clamp(int(std::floor(minY / cellH)), 0, sandH - 1);
		const int y1 = clamp(int(std::ceil(maxY / cellH)), 0, sandH - 1);
		for (int gy = y0; gy <= y1; ++gy) {
			const float cy = (float(gy) + 0.5f) * cellH;
			for (int gx = x0; gx <= x1; ++gx) {
				const float cx = (float(gx) + 0.5f) * cellW;
				const Vec p(cx, cy);
				const Vec ap = p.minus(a);
				const float along = clamp(ap.x * ab.x + ap.y * ab.y, 0.f, len);
				const Vec nearest = a.plus(ab.mult(along));
				const float signedSide = (p.x - nearest.x) * normal.x + (p.y - nearest.y) * normal.y;
				const float absSide = std::fabs(signedSide);
				const float trough = smoother01(1.f - absSide / bodyRadiusPx);
				const float ridge = smoother01(1.f - std::fabs(absSide - ridgeOffsetPx) / 2.4f);
				const float energy = smoother01(1.f - absSide / effectRadiusPx);
				if (trough <= 0.f && ridge <= 0.f && energy <= 0.f) {
					continue;
				}
				const int idx = gy * sandW + gx;
				sandDepth[idx] = clamp(sandDepth[idx] - troughStrength * trough + ridgeStrength * ridge, -1.f, 1.f);
				sandEnergy[idx] = clamp(sandEnergy[idx] + energyStrength * energy, 0.f, 1.f);
			}
		}
	}

	void updateSand(double nowSec) {
		if (!sandEnabled() || !std::isfinite(nowSec)) {
			resetSandHistory();
			return;
		}
		ensureSandField();
		if (lastSandUpdateSec < 0.0 || !std::isfinite(lastSandUpdateSec)) {
			lastSandUpdateSec = nowSec;
		}
		const float elapsed = clamp(float(nowSec - lastSandUpdateSec), 0.f, 0.25f);
		lastSandUpdateSec = nowSec;
		const float depthDecay = std::exp(-0.55f * elapsed);
		const float energyDecay = std::exp(-3.5f * elapsed);
		for (int i = 0, n = sandW * sandH; i < n; ++i) {
			sandDepth[i] *= depthDecay;
			sandEnergy[i] *= energyDecay;
		}
		if (!module || module->pointCount <= 1) {
			previousWyrmPathCount = 0;
			return;
		}

		const int count = clamp(module->pointCount, 2, kWyrmPointCountMax);
		std::array<Vec, kWyrmPointCountMax> currentPath {};
		for (int i = 0; i < count; ++i) {
			currentPath[i] = Vec(pointX(i, count), yFromValue(displayWavePoint(i)));
		}

		const float slitherAmount = effectiveSlitherAmount();
		const bool animateDisturbance = slitherAmount > 1e-4f;
		if (animateDisturbance && previousWyrmPathCount == count) {
			const float troughStrength = (0.018f + 0.052f * slitherAmount) * std::min(1.f, elapsed * 60.f);
			const float ridgeStrength = (0.010f + 0.032f * slitherAmount) * std::min(1.f, elapsed * 60.f);
			for (int i = 0; i < count - 1; ++i) {
				const Vec delta = currentPath[i].minus(previousWyrmPath[i]);
				const float motion = clamp(std::sqrt(delta.x * delta.x + delta.y * delta.y) / 7.f, 0.f, 1.f);
				const float energyStrength = (0.015f + 0.075f * motion) * slitherAmount;
				disturbSandSegment(currentPath[i], currentPath[i + 1], troughStrength, ridgeStrength, energyStrength);
			}
		}

		for (int i = 0; i < count; ++i) {
			previousWyrmPath[i] = currentPath[i];
		}
		previousWyrmPathCount = count;
	}

	void drawFlatBackground(NVGcontext* vg) const {
		nvgBeginPath(vg);
		nvgRect(vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(vg, nvgRGBA(14, 14, 14, 205));
		nvgFill(vg);
	}

	void drawSandBackground(NVGcontext* vg) {
		if (!sandEnabled()) {
			drawFlatBackground(vg);
			return;
		}
		ensureSandField();
		nvgBeginPath(vg);
		nvgRect(vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(vg, nvgRGBA(72, 50, 28, 224));
		nvgFill(vg);
		const float cellW = box.size.x / float(std::max(sandW, 1));
		const float cellH = box.size.y / float(std::max(sandH, 1));
		for (int gy = 0; gy < sandH; ++gy) {
			for (int gx = 0; gx < sandW; ++gx) {
				const int idx = gy * sandW + gx;
				const float grain = sandBaseNoise[idx];
				const float depth = clamp(sandDepth[idx], -1.f, 1.f);
				const float energy = clamp(sandEnergy[idx], 0.f, 1.f);
				float shade = 0.68f + 0.24f * grain + 0.28f * std::max(depth, 0.f) - 0.35f * std::max(-depth, 0.f);
				shade = clamp(shade + 0.22f * energy, 0.f, 1.25f);
				const int r = clamp(int(118.f * shade + 30.f * energy), 0, 255);
				const int g = clamp(int(82.f * shade + 22.f * energy), 0, 255);
				const int b = clamp(int(42.f * shade + 10.f * grain), 0, 255);
				const int alpha = clamp(116 + int(78.f * std::fabs(depth)) + int(64.f * energy), 72, 235);
				nvgBeginPath(vg);
				nvgRect(vg, float(gx) * cellW, float(gy) * cellH, cellW + 0.5f, cellH + 0.5f);
				nvgFillColor(vg, nvgRGBA(r, g, b, alpha));
				nvgFill(vg);
				if (energy > 0.42f && hashUnit(uint32_t(idx) ^ 0xa53c9e7du) > 0.62f) {
					nvgBeginPath(vg);
					nvgCircle(vg, (float(gx) + 0.5f) * cellW, (float(gy) + 0.5f) * cellH, 0.35f + 0.75f * energy);
					nvgFillColor(vg, nvgRGBA(245, 204, 126, int(90.f * energy)));
					nvgFill(vg);
				}
			}
		}
	}

	float pointEdgeInset() const {
		return 2.2f;
	}

	float pointDrawWidth() const {
		return std::max(1.f, box.size.x - 2.f * pointEdgeInset());
	}

	float pointStep(int count) const {
		return pointDrawWidth() / float(std::max(count, 1));
	}

	float pointX(int index, int count) const {
		return pointEdgeInset() + (float(index) + 0.5f) * pointStep(count);
	}

	int indexFromX(float x) const {
		if (box.size.x <= 1.f) return 0;
		const int count = module ? module->pointCount : kWyrmPointCountDefault;
		const float dx = pointStep(count);
		const float column = (x - pointEdgeInset()) / std::max(dx, 1e-6f);
		return clamp(int(std::floor(column)), 0, count - 1);
	}

	float valueFromY(float y) const {
		if (box.size.y <= 1.f) return 0.f;
		const float n = clamp(y / box.size.y, 0.f, 1.f);
		return clamp(1.f - 2.f * n, -1.f, 1.f);
	}

	float phaseFromX(float x) const {
		if (box.size.x <= 1.f) return 0.f;
		return wrap01(clamp((x - pointEdgeInset()) / std::max(pointDrawWidth(), 1e-6f), 0.f, 0.9999f));
	}

	float yFromValue(float value) const {
		return (0.5f - 0.5f * clamp(value, -1.f, 1.f)) * box.size.y;
	}

	Vec rockCenter(const WyrmRock& rock) const {
		return Vec(pointEdgeInset() + rock.phase * pointDrawWidth(), yFromValue(rock.value));
	}

	Vec rockPixelRadius(const WyrmRock& rock) const {
		return Vec(std::max(5.f, rock.radiusPhase * pointDrawWidth()), std::max(5.f, kWyrmRockValueScale * rock.radiusValue * box.size.y));
	}

	float visualRockClearance() const {
		if (box.size.y <= 1.f) {
			return kWyrmRockClearance;
		}
		const float maxBodyStrokePx = 4.f;
		const float maxRockStrokePx = 2.2f;
		const float pixelClearance = 0.5f * maxBodyStrokePx + 0.5f * maxRockStrokePx + 0.75f;
		const float valueClearance = 2.f * pixelClearance / box.size.y;
		return std::max(kWyrmRockClearance, valueClearance / std::max(kWyrmRockValueScale, 1e-4f));
	}

	float visualRockPhaseClearance() const {
		if (pointDrawWidth() <= 1.f) {
			return 0.f;
		}
		const float maxBodyStrokePx = 4.f;
		const float maxRockStrokePx = 2.2f;
		const float pixelClearance = 0.5f * maxBodyStrokePx + 0.5f * maxRockStrokePx + 0.75f;
		return pixelClearance / pointDrawWidth();
	}

	bool visualRockBoundsAtPhase(const WyrmRock& rock, float phase, float* lower, float* upper) const {
		if (!module) {
			return false;
		}
		return module->rockBoundsAtPhase(rock, phase, visualRockClearance(), visualRockPhaseClearance(), lower, upper);
	}

	int rockDragModeForMods(int mods) const {
		int mode = module ? module->rockMouseMode : ROCK_MOUSE_DRAGS;
		if ((mods & GLFW_MOD_SHIFT) != 0) {
			mode = (mode == ROCK_MOUSE_DRAGS) ? ROCK_MOUSE_LIFTS : ROCK_MOUSE_DRAGS;
		}
		return mode;
	}

	void updateActiveRockDragMode(int mods) {
		if (!module || draggingRock < 0) {
			return;
		}
		const int nextMode = rockDragModeForMods(mods);
		if (nextMode == dragRockMouseMode) {
			return;
		}
		if (module->liftedRock == draggingRock) {
			module->liftedRock = -1;
			module->publishRockState();
		}
		dragRockMouseMode = nextMode;
		if (dragRockMouseMode == ROCK_MOUSE_LIFTS) {
			module->liftedRock = draggingRock;
			module->publishRockState();
		}
		previousDragRock = module->rocks[draggingRock];
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
		const float speedFactor = module->displaySlitherSpeedFactor.load(std::memory_order_relaxed);
		visualSlitherPhase = wrap01(visualSlitherPhase + 0.65f * speedFactor * elapsed);
	}

	float effectiveSlitherAmount() const {
		if (!module) {
			return 0.f;
		}
		return clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
	}

	float slitherOffsetForIndex(int index) const {
		return slitherOffsetForIndex(index, visualSlitherPhase);
	}

	float slitherOffsetForIndex(int index, float travelPhase) const {
		if (!module || module->pointCount <= 0) return 0.f;
		const float amount = effectiveSlitherAmount();
		if (amount <= 1e-5f) return 0.f;
		const float phase = (float(index) + 0.5f) / float(module->pointCount);
		return slitherOffset(phase, travelPhase, amount);
	}

	float slitherOffsetForPhase(float phase) const {
		if (!module) return 0.f;
		const float amount = effectiveSlitherAmount();
		if (amount <= 1e-5f) return 0.f;
		return slitherOffset(phase, visualSlitherPhase, amount);
	}

	float displayWavePoint(int index) const {
		if (!module) return 0.f;
		const float phase = (float(index) + 0.5f) / float(module->pointCount);
		const float clearance = visualRockClearance();
		const float phaseClearance = visualRockPhaseClearance();
		const float base = module->resolveAgainstRocks(module->getWavePoint(index), module->getWavePoint(index), phase, clearance, phaseClearance);
		return module->resolveAgainstRocks(base, base + slitherOffsetForIndex(index), phase, clearance, phaseClearance);
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
		const WyrmRock previousRock = rock;
		const Vec previousCenter = rockCenter(previousRock);
		const float phase = phaseFromX(adjusted.x);
		const float value = valueFromY(adjusted.y);
		auto shortestPhaseDelta = [](float from, float to) {
			float d = wrap01(to) - wrap01(from);
			if (d > 0.5f) d -= 1.f;
			if (d < -0.5f) d += 1.f;
			return d;
		};

		const float dPhase = shortestPhaseDelta(previousRock.phase, phase);
		const float dValue = value - previousRock.value;
		// Trace the drag path to avoid tunneling through the waveform on fast moves.
		const float phaseStep = std::max(0.004f, 0.18f * previousRock.radiusPhase);
		const float valueStep = std::max(0.01f, 0.18f * kWyrmRockValueScale * previousRock.radiusValue);
		const int phaseSteps = int(std::ceil(std::fabs(dPhase) / phaseStep));
		const int valueSteps = int(std::ceil(std::fabs(dValue) / valueStep));
		const int steps = clamp(std::max(1, std::max(phaseSteps, valueSteps)), 1, 16);

		WyrmRock prevStepRock = previousRock;
		for (int s = 1; s <= steps; ++s) {
			const float t = float(s) / float(steps);
			rock.phase = wrap01(previousRock.phase + dPhase * t);
			rock.value = clamp(previousRock.value + dValue * t, -1.f, 1.f);
			module->rebuildRockBoundaryCache(rockIndex);
			if (dragRockMouseMode == ROCK_MOUSE_DRAGS) {
				module->sculptWaveAroundRock(rockIndex, &prevStepRock);
			}
			prevStepRock = rock;
		}
		module->publishRockState();
		previousDragRock = rock;
		const Vec newCenter = rockCenter(rock);
		const Vec radius = rockPixelRadius(rock);
		const float stampRadius = clamp(0.35f * std::max(radius.x, radius.y), 5.f, 18.f);
		const bool liftMode = (dragRockMouseMode == ROCK_MOUSE_LIFTS);
		stampSand(previousCenter, stampRadius, liftMode ? -0.025f : -0.075f, liftMode ? 0.04f : 0.18f);
		stampSand(newCenter, stampRadius, liftMode ? -0.020f : 0.055f, liftMode ? 0.04f : 0.14f);
	}

	Vec currentLocalMousePos() const {
		if (!parent || !APP || !APP->scene || !APP->scene->rack) {
			return Vec();
		}
		return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
	}

	void applyPointFromPos(Vec pos) {
		if (!module || module->editorLocked.load(std::memory_order_relaxed)) return;
		const int idx = indexFromX(pos.x);
		const float targetDisplayValue = valueFromY(pos.y);
		const float writeSlitherPhase = pointEditActive ? pointEditSlitherPhase : visualSlitherPhase;
		auto writeDisplayValue = [&](int pointIndex) {
			module->setWavePoint(pointIndex, targetDisplayValue - slitherOffsetForIndex(pointIndex, writeSlitherPhase));
		};
		if (lastIndex >= 0 && lastIndex != idx) {
			const int lo = std::min(lastIndex, idx);
			const int hi = std::max(lastIndex, idx);
			for (int i = lo; i <= hi; ++i) {
				writeDisplayValue(i);
			}
		}
		else {
			writeDisplayValue(idx);
		}
		lastIndex = idx;
		stampSand(pos, 5.5f, -0.16f, 0.28f);
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
				pointEditActive = false;
				draggingRock = rockIndex;
				hoveredRock = rockIndex;
				dragRockMouseMode = rockDragModeForMods(e.mods);
				if (dragRockMouseMode == ROCK_MOUSE_LIFTS) {
					module->liftedRock = rockIndex;
					module->publishRockState();
				}
				previousDragRock = module->rocks[rockIndex];
				rockDragOffset = e.pos.minus(rockCenter(module->rocks[rockIndex]));
				e.consume(this);
				return;
			}
			pointEditActive = true;
			pointEditSlitherPhase = renderedSlitherPhase;
			applyPointFromPos(e.pos);
			e.consume(this);
			return;
		}
		if (e.action == GLFW_RELEASE) {
			lastIndex = -1;
			pointEditActive = false;
			const int releasedRock = draggingRock;
			updateActiveRockDragMode(e.mods);
			if (module->liftedRock == draggingRock) {
				module->liftedRock = -1;
				module->publishRockState();
			}
			if (releasedRock >= 0) {
				module->rebuildRockBoundaryCache(releasedRock);
				if (dragRockMouseMode == ROCK_MOUSE_DRAGS) {
					module->sculptWaveAroundRock(releasedRock, &previousDragRock);
				}
				else if (dragRockMouseMode == ROCK_MOUSE_LIFTS) {
					const Vec center = rockCenter(module->rocks[releasedRock]);
					const Vec radius = rockPixelRadius(module->rocks[releasedRock]);
					stampSand(center, clamp(0.45f * std::max(radius.x, radius.y), 6.f, 22.f), -0.11f, 0.34f);
				}
				module->publishRockState();
			}
			draggingRock = -1;
			dragRockMouseMode = -1;
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (module && draggingRock >= 0 && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			const int mods = (APP && APP->window) ? APP->window->getMods() : 0;
			updateActiveRockDragMode(mods);
			moveRockFromMouse(draggingRock, currentLocalMousePos());
			e.consume(this);
			return;
		}
		if (!module || module->editorLocked.load(std::memory_order_relaxed) || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onDragMove(e);
			return;
		}
		applyPointFromPos(currentLocalMousePos());
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		if (!args.vg) return;
		const double nowSec = system::getTime();
		advanceVisualSlitherPhase(nowSec);
		renderedSlitherPhase = visualSlitherPhase;
		updateSand(nowSec);

		drawSandBackground(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0.f, 0.5f * box.size.y);
		nvgLineTo(args.vg, box.size.x, 0.5f * box.size.y);
		nvgStrokeWidth(args.vg, 1.f);
		nvgStrokeColor(args.vg, nvgRGBA(240, 180, 42, 120));
		nvgStroke(args.vg);

		const bool hasModule = (module != nullptr);
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		const int count = hasModule ? module->pointCount : kWyrmPointCountDefault;
		const float dx = pointStep(count);
		const float graphColumnWidth = std::min(2.0f, dx);
		auto waveValueAt = [&](int i) {
			if (hasModule) {
				return displayWavePoint(i);
			}
			const float phase = (float(i) + 0.5f) / float(std::max(count, 1));
			return std::sin(2.f * float(M_PI) * phase);
		};
		std::array<float, kWyrmPointCountMax> bodyPoints {};
		if (hasModule) {
			for (int i = 0; i < module->pointCount; ++i) {
				bodyPoints[i] = module->getWavePoint(i);
			}
		}
		auto bodyWaveValueAtPhase = [&](float phase) {
			if (hasModule) {
				const float clearance = visualRockClearance();
				const float phaseClearance = visualRockPhaseClearance();
				const float raw = catmullPeriodic(bodyPoints, module->pointCount, phase);
				const float base = module->resolveAgainstRocks(raw, raw, phase, clearance, phaseClearance);
				return module->resolveAgainstRocks(base, base + slitherOffsetForPhase(phase), phase, clearance, phaseClearance);
			}
			return std::sin(2.f * float(M_PI) * phase);
		};
		auto phaseNearAnyRock = [&](float phase, float margin) {
			if (!hasModule) {
				return false;
			}
			for (int i = 0; i < module->rockCount; ++i) {
				const WyrmRock& rock = module->rocks[i];
				const float rx = rock.radiusPhase + visualRockPhaseClearance() + margin;
				if (std::fabs(module->rockDx(phase, rock)) <= rx) {
					return true;
				}
			}
			return false;
		};

		Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= box.size.x && mouseLocal.y >= 0.f && mouseLocal.y <= box.size.y);
		const int hoveredColumn = mouseInside ? indexFromX(mouseLocal.x) : -1;
		float hoveredColumnCenterX = 0.f;
		bool hoveredColumnCenterValid = false;
		hoveredRock = (draggingRock >= 0) ? draggingRock : (mouseInside ? rockIndexAt(mouseLocal) : -1);
		if (hasModule && mouseInside) {
			const float guideY = clamp(mouseLocal.y, 0.f, box.size.y);
			const int hoverIdx = hoveredColumn;
			const float dxHover = pointStep(count);
			const float x0 = pointEdgeInset() + float(hoverIdx) * dxHover;
			const float x1 = x0 + dxHover;

			nvgBeginPath(args.vg);
			nvgRect(args.vg, x0, 0.f, x1 - x0, box.size.y);
			nvgFillColor(args.vg, nvgRGBA(28, 204, 217, 72));
			nvgFill(args.vg);

			const float guideX = 0.5f * (x0 + x1);
			hoveredColumnCenterX = guideX;
			hoveredColumnCenterValid = true;
			nvgBeginPath(args.vg);
			nvgRect(args.vg, guideX - 0.5f * graphColumnWidth, 0.f, graphColumnWidth, box.size.y);
			nvgFillColor(args.vg, nvgRGBA(28, 204, 217, 238));
			nvgFill(args.vg);

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, 0.f, guideY);
			nvgLineTo(args.vg, box.size.x, guideY);
			nvgStrokeWidth(args.vg, 1.4f);
			nvgStrokeColor(args.vg, nvgRGBA(186, 154, 92, 96));
			nvgStroke(args.vg);
		}

		const float midY = 0.5f * box.size.y;
		auto xAt = [&](int i) {
			return pointX(i, count);
		};
		for (int i = 0; i < count; ++i) {
			const float y = (0.5f - 0.5f * waveValueAt(i)) * box.size.y;
			const bool hotColumn = (i == hoveredColumn);
			float x = xAt(i);
			if (hotColumn && hoveredColumnCenterValid) {
				x = hoveredColumnCenterX;
			}
			nvgBeginPath(args.vg);
			const float yTop = std::min(midY, y);
			const float yBottom = std::max(midY, y);
			nvgRect(args.vg, x - 0.5f * graphColumnWidth, yTop, graphColumnWidth, std::max(1e-4f, yBottom - yTop));
			nvgFillColor(args.vg, hotColumn ? nvgRGBA(28, 204, 217, 238) : nvgRGBA(34, 27, 70, 196));
			nvgFill(args.vg);

		}

		auto emitRoundedBodyPath = [&]() {
			const float roundCosThreshold = -0.25f;
			const int bodySampleCount = std::max(count, hasModule ? std::min(768, std::max(128, module->pointCount * 4)) : count);
			if (bodySampleCount <= 0) {
				return;
			}
			if (bodySampleCount == 1) {
				const float phase = 0.5f;
				const float x = pointEdgeInset() + phase * pointDrawWidth();
				const float y = (0.5f - 0.5f * bodyWaveValueAtPhase(phase)) * box.size.y;
				nvgMoveTo(args.vg, x, y);
				return;
			}

			auto pointAt = [&](int i) {
				const float phase = (float(i) + 0.5f) / float(bodySampleCount);
				return Vec(pointEdgeInset() + phase * pointDrawWidth(), (0.5f - 0.5f * bodyWaveValueAtPhase(phase)) * box.size.y);
			};

			const Vec pStart = pointAt(0);
			nvgMoveTo(args.vg, pStart.x, pStart.y);

			for (int i = 1; i < bodySampleCount - 1; ++i) {
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
				const float phase = (float(i) + 0.5f) / float(bodySampleCount);
				if (phaseNearAnyRock(phase, 1.5f / float(bodySampleCount))) {
					nvgLineTo(args.vg, p1.x, p1.y);
				}
				else if (cornerCos >= roundCosThreshold) {
					const Vec midOut = p1.plus(p2).mult(0.5f);
					nvgQuadTo(args.vg, p1.x, p1.y, midOut.x, midOut.y);
				}
				else {
					nvgLineTo(args.vg, p1.x, p1.y);
				}
			}

			const Vec pEnd = pointAt(bodySampleCount - 1);
			nvgLineTo(args.vg, pEnd.x, pEnd.y);
		};

		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 4.0f);
		nvgStrokeColor(args.vg, nvgRGBA(74, 54, 24, 205));
		nvgStroke(args.vg);

		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 2.6f);
		nvgStrokeColor(args.vg, nvgRGBA(167, 132, 72, 230));
		nvgStroke(args.vg);

		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 1.15f);
		nvgStrokeColor(args.vg, nvgRGBA(246, 215, 136, 225));
		nvgStroke(args.vg);

		for (int i = 0; hasModule && i < module->rockCount; ++i) {
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

		if (hasModule && draggingRock >= 0 && draggingRock < module->rockCount) {
			const Vec center = rockCenter(module->rocks[draggingRock]);
			const Vec radius = rockPixelRadius(module->rocks[draggingRock]);
			const bool liftMode = (dragRockMouseMode == ROCK_MOUSE_LIFTS);
			const NVGcolor arrowColor =
				liftMode
					? nvgRGBA(110, 228, 255, 235)
					: nvgRGBA(236, 226, 190, 225);
			auto drawArrow = [&](Vec dir, Vec normal) {
				const Vec start = center.plus(Vec(dir.x * (radius.x + 3.f), dir.y * (radius.y + 3.f)));
				const Vec end = center.plus(Vec(dir.x * (radius.x + 18.f), dir.y * (radius.y + 18.f)));
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, start.x, start.y);
				nvgLineTo(args.vg, end.x, end.y);
				nvgStrokeWidth(args.vg, 1.5f);
				nvgStrokeColor(args.vg, arrowColor);
				nvgStroke(args.vg);

				const Vec headA = end.minus(Vec(dir.x * 5.f, dir.y * 5.f)).plus(normal.mult(3.5f));
				const Vec headB = end.minus(Vec(dir.x * 5.f, dir.y * 5.f)).minus(normal.mult(3.5f));
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, end.x, end.y);
				nvgLineTo(args.vg, headA.x, headA.y);
				nvgMoveTo(args.vg, end.x, end.y);
				nvgLineTo(args.vg, headB.x, headB.y);
				nvgStrokeWidth(args.vg, 1.5f);
				nvgStrokeColor(args.vg, arrowColor);
				nvgStroke(args.vg);
			};
			drawArrow(Vec(1.f, 0.f), Vec(0.f, 1.f));
			drawArrow(Vec(-1.f, 0.f), Vec(0.f, 1.f));
			drawArrow(Vec(0.f, 1.f), Vec(1.f, 0.f));
			drawArrow(Vec(0.f, -1.f), Vec(1.f, 0.f));

			if (APP && APP->window && APP->window->uiFont) {
				const char* modeText = liftMode ? "LIFT" : "DRAG";
				const float labelX = center.x - radius.x - 16.f;
				const float labelY = center.y - radius.y + 1.f;
				nvgFontSize(args.vg, 8.0f);
				nvgFontFaceId(args.vg, APP->window->uiFont->handle);
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
				nvgFillColor(args.vg, arrowColor);
				nvgText(args.vg, labelX, labelY, modeText, nullptr);
			}
		}
		nvgResetScissor(args.vg);
		nvgRestore(args.vg);
	}
};

TransparentWidget* createWyrmWaveEditor(Wyrm* module) {
	return new WyrmWaveEditor(module);
}
