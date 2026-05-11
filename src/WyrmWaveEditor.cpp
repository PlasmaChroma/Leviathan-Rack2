#include "Wyrm.hpp"

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;
	int hoveredRock = -1;
	int draggingRock = -1;
	int dragRockMouseMode = -1;
	float visualSlitherPhase = 0.f;
	double lastVisualUpdateSec = -1.0;
	Vec rockDragOffset;
	WyrmRock previousDragRock {};

	explicit WyrmWaveEditor(Wyrm* m) {
		module = m;
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

	bool visualRockBoundsAtPhase(const WyrmRock& rock, float phase, float* lower, float* upper) const {
		if (!module) {
			return false;
		}
		return module->rockBoundsAtPhase(rock, phase, visualRockClearance(), lower, upper);
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
		}
		dragRockMouseMode = nextMode;
		if (dragRockMouseMode == ROCK_MOUSE_LIFTS) {
			module->liftedRock = draggingRock;
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

	float slitherOffsetForPhase(float phase) const {
		if (!module) return 0.f;
		const float amount = clamp01(module->params[Wyrm::SLITHER_PARAM].getValue());
		if (amount <= 1e-5f) return 0.f;
		return slitherOffset(phase, visualSlitherPhase, amount);
	}

	float displayWavePoint(int index) const {
		if (!module) return 0.f;
		const float phase = (float(index) + 0.5f) / float(module->pointCount);
		const float clearance = visualRockClearance();
		const float base = module->resolveAgainstRocks(module->getWavePoint(index), module->getWavePoint(index), phase, clearance);
		return module->resolveAgainstRocks(base, base + slitherOffsetForIndex(index), phase, clearance);
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
		const float phase = phaseFromX(adjusted.x);
		const float value = valueFromY(adjusted.y);
		rock.phase = phase;
		rock.value = value;
		module->rebuildRockBoundaryCache(rockIndex);
		if (dragRockMouseMode == ROCK_MOUSE_DRAGS) {
			module->sculptWaveAroundRock(rockIndex, &previousRock);
		}
		previousDragRock = rock;
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
				dragRockMouseMode = rockDragModeForMods(e.mods);
				if (dragRockMouseMode == ROCK_MOUSE_LIFTS) {
					module->liftedRock = rockIndex;
				}
				previousDragRock = module->rocks[rockIndex];
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
			const int releasedRock = draggingRock;
			updateActiveRockDragMode(e.mods);
			if (module->liftedRock == draggingRock) {
				module->liftedRock = -1;
			}
			if (releasedRock >= 0) {
				module->rebuildRockBoundaryCache(releasedRock);
				if (dragRockMouseMode == ROCK_MOUSE_DRAGS) {
					module->sculptWaveAroundRock(releasedRock, &previousDragRock);
				}
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
				const float raw = catmullPeriodic(bodyPoints, module->pointCount, phase);
				const float base = module->resolveAgainstRocks(raw, raw, phase, clearance);
				return module->resolveAgainstRocks(base, base + slitherOffsetForPhase(phase), phase, clearance);
			}
			return std::sin(2.f * float(M_PI) * phase);
		};
		auto phaseNearAnyRock = [&](float phase, float margin) {
			if (!hasModule) {
				return false;
			}
			for (int i = 0; i < module->rockCount; ++i) {
				const WyrmRock& rock = module->rocks[i];
				const float clearance = visualRockClearance();
				const float rx = rock.radiusPhase + module->rockClearancePhase(rock, clearance) + margin;
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
			const NVGcolor arrowColor =
				(dragRockMouseMode == ROCK_MOUSE_LIFTS)
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
		}
		nvgResetScissor(args.vg);
		nvgRestore(args.vg);
	}
};

TransparentWidget* createWyrmWaveEditor(Wyrm* module) {
	return new WyrmWaveEditor(module);
}
