#include "Wyrm.hpp"

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;
	int hoveredRock = -1;
	int draggingRock = -1;
	int dragRockMouseMode = -1;
	int rockDragSide = 0;
	float visualSlitherPhase = 0.f;
	double lastVisualUpdateSec = -1.0;
	Vec rockDragOffset;

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
		return wrap01(clamp(x / box.size.x, 0.f, 0.9999f));
	}

	float yFromValue(float value) const {
		return (0.5f - 0.5f * clamp(value, -1.f, 1.f)) * box.size.y;
	}

	Vec rockCenter(const WyrmRock& rock) const {
		return Vec(rock.phase * box.size.x, yFromValue(rock.value));
	}

	Vec rockPixelRadius(const WyrmRock& rock) const {
		return Vec(std::max(5.f, rock.radiusPhase * box.size.x), std::max(5.f, kWyrmRockValueScale * rock.radiusValue * box.size.y));
	}

	float baseWaveAtPhase(float phase) const {
		if (!module || module->pointCount <= 0) return 0.f;
		std::array<float, kWyrmPointCountMax> local {};
		for (int i = 0; i < module->pointCount; ++i) {
			local[i] = module->getWavePoint(i);
		}
		return clamp(catmullPeriodic(local, module->pointCount, phase), -1.f, 1.f);
	}

	float constrainedRockValueForDrag(const WyrmRock& rock, float phase, float value) const {
		if (!module || dragRockMouseMode != ROCK_MOUSE_DRAGS || rockDragSide == 0) {
			return value;
		}
		if (rockDragSide > 0) {
			return clamp(std::max(value, baseWaveAtPhase(phase)), -1.f, 1.f);
		}
		return clamp(std::min(value, baseWaveAtPhase(phase)), -1.f, 1.f);
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
		const float phase = phaseFromX(adjusted.x);
		const float value = valueFromY(adjusted.y);
		rock.phase = phase;
		rock.value = constrainedRockValueForDrag(rock, phase, value);
		if (dragRockMouseMode == ROCK_MOUSE_DRAGS) {
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
				const WyrmRock& rock = module->rocks[rockIndex];
				dragRockMouseMode = module->rockMouseMode;
				if ((e.mods & GLFW_MOD_SHIFT) != 0) {
					dragRockMouseMode = (dragRockMouseMode == ROCK_MOUSE_DRAGS) ? ROCK_MOUSE_LIFTS : ROCK_MOUSE_DRAGS;
				}
				rockDragSide = (rock.value >= baseWaveAtPhase(rock.phase)) ? 1 : -1;
				if (dragRockMouseMode == ROCK_MOUSE_LIFTS) {
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
			dragRockMouseMode = -1;
			rockDragSide = 0;
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
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		const int count = module->pointCount;
		const float dx = pointStep(count);
		const float graphColumnWidth = std::min(2.0f, dx);

		Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= box.size.x && mouseLocal.y >= 0.f && mouseLocal.y <= box.size.y);
		const int hoveredColumn = mouseInside ? indexFromX(mouseLocal.x) : -1;
		float hoveredColumnCenterX = 0.f;
		bool hoveredColumnCenterValid = false;
		hoveredRock = (draggingRock >= 0) ? draggingRock : (mouseInside ? rockIndexAt(mouseLocal) : -1);
		if (mouseInside) {
			const float guideY = clamp(mouseLocal.y, 0.f, box.size.y);
			const int hoverIdx = hoveredColumn;
			const int count = module->pointCount;
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
			const float y = (0.5f - 0.5f * displayWavePoint(i)) * box.size.y;
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

			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, 2.1f);
			nvgFillColor(args.vg, nvgRGBA(235, 204, 128, 245));
			nvgFill(args.vg);
		}

		auto emitRoundedBodyPath = [&]() {
			const float roundCosThreshold = -0.25f;
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
