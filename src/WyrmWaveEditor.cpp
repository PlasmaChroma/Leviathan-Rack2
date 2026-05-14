#include "Wyrm.hpp"
#include "WyrmSand.hpp"
#include "DebugTerminalTransport.hpp"

#include <chrono>
#include <unordered_map>
#include <vector>

namespace {
constexpr double kWyrmDebugTerminalSubmitIntervalSec = 1.0 / 8.0;
std::unordered_map<uint32_t, double> gWyrmDebugTerminalLastSubmitSec;
}

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
	Vec rockDragOffset;
	WyrmRock previousDragRock {};
	std::shared_ptr<WyrmSand> sand;

	explicit WyrmWaveEditor(Wyrm* m, std::shared_ptr<WyrmSand> sandState) {
		module = m;
		sand = sandState ? sandState : std::make_shared<WyrmSand>();
	}

	bool sandEnabled() const {
		return module && module->sandViewEnabled.load(std::memory_order_relaxed);
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

	void updateSand(double nowSec) {
		if (!sandEnabled() || !std::isfinite(nowSec)) {
			sand->resetHistory();
			return;
		}
		const int detailSetting = module
			? module->sandDetail.load(std::memory_order_relaxed)
			: WYRMSAND_DETAIL_AUTO;
		if (!module || module->pointCount <= 1) {
			std::array<Vec, kWyrmPointCountMax> emptyPath {};
			sand->update(box.size, nowSec, detailSetting, emptyPath, 0, 0.f);
			return;
		}
		const int count = clamp(module->pointCount, 2, kWyrmPointCountMax);
		std::array<Vec, kWyrmPointCountMax> currentPath {};
		for (int i = 0; i < count; ++i) {
			currentPath[i] = Vec(pointX(i, count), yFromValue(displayWavePoint(i)));
		}
		sand->update(box.size, nowSec, detailSetting, currentPath, count, effectiveSlitherAmount());
	}

	void drawSandBackground(NVGcontext* vg) {
		const int backendSetting = module
			? module->sandBackend.load(std::memory_order_relaxed)
			: WYRMSAND_NANOVG_CELLS;
		const int detailSetting = module
			? module->sandDetail.load(std::memory_order_relaxed)
			: WYRMSAND_DETAIL_AUTO;
		if (backendSetting != WYRMSAND_OPENGL_TEXTURE && backendSetting != WYRMSAND_SHADER_FEEDBACK) {
			sand->draw(vg, box.size, sandEnabled(), backendSetting, detailSetting);
		}
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
		sand->stamp(box.size, previousCenter, stampRadius, liftMode ? -0.025f : -0.075f, liftMode ? 0.04f : 0.18f);
		sand->stamp(box.size, newCenter, stampRadius, liftMode ? -0.020f : 0.055f, liftMode ? 0.04f : 0.14f);
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
		sand->stamp(box.size, pos, 5.5f, -0.16f, 0.28f);
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
					sand->stamp(box.size, center, clamp(0.45f * std::max(radius.x, radius.y), 6.f, 22.f), -0.11f, 0.34f);
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
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = module && isDragonKingDebugEnabled();
		const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		float sandUpdateUs = 0.f;
		float sandDrawUs = 0.f;
		const double nowSec = system::getTime();
		advanceVisualSlitherPhase(nowSec);
		renderedSlitherPhase = visualSlitherPhase;
		const PerfClock::time_point sandUpdateStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		updateSand(nowSec);
		if (measurePerf) {
			sandUpdateUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - sandUpdateStart).count()) * 0.001f;
		}

		const PerfClock::time_point sandDrawStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		drawSandBackground(args.vg);
		if (measurePerf) {
			sandDrawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - sandDrawStart).count()) * 0.001f;
		}

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
		const int bodySampleCount = std::max(count, hasModule ? std::min(768, std::max(128, module->pointCount * 4)) : count);
		const bool drawWaveColumns = !sandEnabled();
		if (drawWaveColumns) {
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
		}

		auto emitRoundedBodyPath = [&]() {
			const float roundCosThreshold = -0.25f;
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

		if (measurePerf) {
			const float editorDrawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStart).count()) * 0.001f;
			uint32_t debugId = module->debugInstanceId;
			double& lastSubmitSec = gWyrmDebugTerminalLastSubmitSec[debugId];
			if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kWyrmDebugTerminalSubmitIntervalSec) {
				const uint64_t audioSampledCount = module->perfAudioSampledCount.exchange(0, std::memory_order_acq_rel);
				const uint64_t audioProcessNs = module->perfAudioProcessNs.exchange(0, std::memory_order_acq_rel);
				const float audioUs = (audioSampledCount > 0u) ? float(double(audioProcessNs) / double(audioSampledCount) * 0.001) : 0.f;
				const bool wavetableRebuilt = module->perfWavetableRebuilt.exchange(false, std::memory_order_acq_rel);
				lastSubmitSec = nowSec;
				debug_terminal::submitWyrmMetrics(
					debugId,
					editorDrawUs * 0.001f,
					editorDrawUs,
					sandUpdateUs,
					sandDrawUs,
					audioUs,
					module->perfChannels.load(std::memory_order_relaxed),
					module->pointCount,
					bodySampleCount,
					wavetableRebuilt
				);
			}
		}
	}
};

TransparentWidget* createWyrmWaveEditor(Wyrm* module) {
	return new WyrmWaveEditor(module, std::make_shared<WyrmSand>());
}

TransparentWidget* createWyrmWaveEditor(Wyrm* module, std::shared_ptr<WyrmSand> sandState) {
	return new WyrmWaveEditor(module, sandState);
}
