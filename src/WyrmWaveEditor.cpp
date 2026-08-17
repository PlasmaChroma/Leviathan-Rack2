#include "Wyrm.hpp"
#include "WyrmRenderGeometry.hpp"
#include "WyrmSand.hpp"
#include "DebugTerminalTransport.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"

#include <chrono>
#include <unordered_map>
#include <vector>

namespace {
constexpr double kWyrmDebugTerminalSubmitIntervalSec = debug_terminal::kTimingRangeSubmitIntervalSec;
std::unordered_map<uint32_t, double> gWyrmDebugTerminalLastSubmitSec;

inline unsigned char wyrmClampU8(int v) {
	return (unsigned char) clamp(v, 0, 255);
}
}

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;
	Vec lastPointEditPos;
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
	Vec lastBoxSize = Vec(-1.f, -1.f);
	int lastHoverRock = -2;
	uint32_t lastWaveVersion = 0;
	int lastPointCount = -1;
	int lastRockStateIndex = -1;
	bool lastSandEnabledState = false;
	int lastRenderMode = -1;
	int lastSandBackend = -1;
	int lastSandDetail = -1;
	int lastSandPersistence = -1;
	bool lastEditorLocked = false;
	bool lastEnvelopeMode = false;
	float lastEditorDrawUs = 0.f;
	float lastStepUsEma = 0.f;
	debug_terminal::UiTimingRangeAccumulator stepUsRange;
	debug_terminal::UiTimingRangeAccumulator drawUsRange;
	float lastSandUpdateUs = 0.f;
	float lastSandDrawUs = 0.f;
	int lastBodySampleCount = 0;
	std::array<float, kWyrmPointCountMax> cachedDisplayWaveValues {};
	int cachedDisplayWaveCount = 0;
	bool cachedDisplayWaveValid = false;
	uint32_t cachedDisplayWaveVersion = 0;
	int cachedDisplayRockStateIndex = -1;
	Vec cachedDisplaySize = Vec(-1.f, -1.f);
	float cachedDisplaySlitherPhase = -1.f;
	float cachedDisplaySlitherAmount = -1.f;
	std::shared_ptr<wyrm_render::DisplayGeometryCache> geometryCache;
	NVGcontext* waveMaterialContext = nullptr;
	int waveMaterialImage = -1;
	int waveMaterialW = 0;
	int waveMaterialH = 0;
	int waveMaterialCount = -1;
	bool waveMaterialEnvelopeMode = false;
	int waveMaterialUploadedW = 0;
	int waveMaterialUploadedH = 0;
	bool waveMaterialDirty = true;
	std::vector<unsigned char> waveMaterialPixels;

	explicit WyrmWaveEditor(
		Wyrm* m,
		std::shared_ptr<WyrmSand> sandState,
		std::shared_ptr<wyrm_render::DisplayGeometryCache> sharedGeometryCache = nullptr)
		: module(m)
		, sand(sandState ? std::move(sandState) : std::make_shared<WyrmSand>())
		, geometryCache(sharedGeometryCache
			? std::move(sharedGeometryCache)
			: std::make_shared<wyrm_render::DisplayGeometryCache>()) {}

	~WyrmWaveEditor() override {
		nvg_gfx_lifecycle::resetOwnedNvgImage(
			waveMaterialContext,
			waveMaterialImage,
			waveMaterialUploadedW,
			waveMaterialUploadedH,
			nullptr,
			false
		);
	}

	bool sandEnabled() const {
		return module && module->sandViewEnabled.load(std::memory_order_relaxed);
	}

	float pointEdgeInset() const {
		return wyrm_render::kPointEdgeInsetPx;
	}

	float pointDrawWidth() const {
		return wyrm_render::pointDrawWidth(box.size);
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
		return levi_math::wrap01(clamp((x - pointEdgeInset()) / std::max(pointDrawWidth(), 1e-6f), 0.f, 0.9999f));
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
		float clearance = 0.f;
		wyrm_render::visualRockClearance(box.size, &clearance, nullptr);
		return clearance;
	}

	float visualRockPhaseClearance() const {
		float clearance = 0.f;
		wyrm_render::visualRockClearance(box.size, nullptr, &clearance);
		return clearance;
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
		visualSlitherPhase = levi_math::wrap01(visualSlitherPhase + 0.65f * speedFactor * elapsed);
	}

	float effectiveSlitherAmount() const {
		if (!module) {
			return 0.f;
		}
		return levi_math::clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
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
			: WYRMSAND_NANOVG_IMAGE;
		const int detailSetting = module
			? module->sandDetail.load(std::memory_order_relaxed)
			: WYRMSAND_DETAIL_AUTO;
		if (backendSetting != WYRMSAND_OPENGL_TEXTURE && backendSetting != WYRMSAND_SHADER_FEEDBACK) {
			sand->draw(vg, box.size, sandEnabled(), backendSetting, detailSetting);
		}
	}

	static NVGcolor mixColor(NVGcolor a, NVGcolor b, float t) {
		t = levi_math::clamp01(t);
		return nvgRGBAf(
			a.r + (b.r - a.r) * t,
			a.g + (b.g - a.g) * t,
			a.b + (b.b - a.b) * t,
			a.a + (b.a - a.a) * t
		);
	}

	static void compositeOver(const NVGcolor& src, float* dst) {
		const float outA = src.a + dst[3] * (1.f - src.a);
		if (outA <= 1e-6f) {
			dst[0] = dst[1] = dst[2] = dst[3] = 0.f;
			return;
		}
		const float outR = src.r * src.a + dst[0] * dst[3] * (1.f - src.a);
		const float outG = src.g * src.a + dst[1] * dst[3] * (1.f - src.a);
		const float outB = src.b * src.a + dst[2] * dst[3] * (1.f - src.a);
		dst[0] = outR / outA;
		dst[1] = outG / outA;
		dst[2] = outB / outA;
		dst[3] = outA;
	}

	void rebuildWaveMaterialPixels(int count, bool envelopeVisual) {
		const int w = std::max(1, int(std::ceil(box.size.x)));
		const int h = std::max(1, int(std::ceil(box.size.y)));
		count = std::max(1, count);
		waveMaterialW = w;
		waveMaterialH = h;
		waveMaterialCount = count;
		waveMaterialEnvelopeMode = envelopeVisual;
		waveMaterialDirty = true;
		waveMaterialPixels.assign(size_t(w) * size_t(h) * 4u, 0u);

		const float inset = pointEdgeInset();
		const float drawWidth = std::max(1.f, box.size.x - 2.f * inset);
		const float dx = drawWidth / float(count);
		const float midY = 0.5f * box.size.y;
		const NVGcolor posNear = nvgRGBA(28, 204, 217, 46);
		const NVGcolor posFar = nvgRGBA(42, 228, 255, 152);
		const NVGcolor negNear = nvgRGBA(115, 72, 224, 50);
		const NVGcolor negFar = nvgRGBA(150, 92, 255, 162);
		const NVGcolor posShade = nvgRGBA(0, 56, 72, 132);
		const NVGcolor negShade = nvgRGBA(40, 24, 112, 92);

		for (int py = 0; py < h; ++py) {
			const float y = std::min(box.size.y, float(py) + 0.5f);
			const bool positive = envelopeVisual || y < midY;
			const float t = envelopeVisual
				? levi_math::clamp01((box.size.y - y) / std::max(box.size.y, 1.f))
				: (positive
					? levi_math::clamp01((midY - y) / std::max(midY, 1.f))
					: levi_math::clamp01((y - midY) / std::max(box.size.y - midY, 1.f)));
			const NVGcolor base = positive ? mixColor(posNear, posFar, t) : mixColor(negNear, negFar, t);
			for (int px = 0; px < w; ++px) {
				const float x = std::min(box.size.x, float(px) + 0.5f);
				const float columnF = (x - inset) / std::max(dx, 1e-6f);
				const int column = int(std::floor(columnF));
				float out[4] = {0.f, 0.f, 0.f, 0.f};
				compositeOver(base, out);
				if (column >= 0 && column < count && (column & 1) != 0) {
					compositeOver(positive ? posShade : negShade, out);
				}
				const size_t offset = (size_t(py) * size_t(w) + size_t(px)) * 4u;
				waveMaterialPixels[offset + 0u] = wyrmClampU8(int(std::lround(out[0] * out[3] * 255.f)));
				waveMaterialPixels[offset + 1u] = wyrmClampU8(int(std::lround(out[1] * out[3] * 255.f)));
				waveMaterialPixels[offset + 2u] = wyrmClampU8(int(std::lround(out[2] * out[3] * 255.f)));
				waveMaterialPixels[offset + 3u] = wyrmClampU8(int(std::lround(out[3] * 255.f)));
			}
		}
	}

	int ensureWaveMaterialImage(NVGcontext* vg, int count, bool envelopeVisual) {
		if (!vg || box.size.x <= 1.f || box.size.y <= 1.f || count <= 0) {
			return -1;
		}
		const int targetW = std::max(1, int(std::ceil(box.size.x)));
		const int targetH = std::max(1, int(std::ceil(box.size.y)));
		if (waveMaterialW != targetW || waveMaterialH != targetH || waveMaterialCount != count
			|| waveMaterialEnvelopeMode != envelopeVisual || waveMaterialPixels.empty()) {
			rebuildWaveMaterialPixels(count, envelopeVisual);
		}
		if (waveMaterialContext != vg) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				waveMaterialContext,
				waveMaterialImage,
				waveMaterialUploadedW,
				waveMaterialUploadedH,
				vg,
				false
			);
			waveMaterialContext = vg;
		}
		if (waveMaterialImage >= 0 &&
			!nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, waveMaterialImage, waveMaterialUploadedW, waveMaterialUploadedH)) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				waveMaterialContext,
				waveMaterialImage,
				waveMaterialUploadedW,
				waveMaterialUploadedH,
				vg,
				true
			);
			waveMaterialContext = vg;
		}
		if (waveMaterialImage < 0) {
			waveMaterialImage = nvgCreateImageRGBA(vg, waveMaterialW, waveMaterialH, NVG_IMAGE_PREMULTIPLIED, waveMaterialPixels.data());
			waveMaterialContext = vg;
			waveMaterialUploadedW = waveMaterialW;
			waveMaterialUploadedH = waveMaterialH;
			waveMaterialDirty = false;
		}
		else if (waveMaterialUploadedW != waveMaterialW || waveMaterialUploadedH != waveMaterialH) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				waveMaterialContext,
				waveMaterialImage,
				waveMaterialUploadedW,
				waveMaterialUploadedH,
				vg,
				true
			);
			waveMaterialImage = nvgCreateImageRGBA(vg, waveMaterialW, waveMaterialH, NVG_IMAGE_PREMULTIPLIED, waveMaterialPixels.data());
			waveMaterialContext = vg;
			waveMaterialUploadedW = waveMaterialW;
			waveMaterialUploadedH = waveMaterialH;
			waveMaterialDirty = false;
		}
		else if (waveMaterialDirty) {
			nvgUpdateImage(vg, waveMaterialImage, waveMaterialPixels.data());
			waveMaterialDirty = false;
		}
		return waveMaterialImage;
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
			float d = levi_math::wrap01(to) - levi_math::wrap01(from);
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
			rock.phase = levi_math::wrap01(previousRock.phase + dPhase * t);
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
		if (!APP || !APP->scene) {
			return Vec();
		}
		auto* self = const_cast<WyrmWaveEditor*>(this);
		const Vec absoluteOrigin = self->getAbsoluteOffset(Vec());
		const float absoluteZoom = std::max(self->getAbsoluteZoom(), 1e-6f);
		return APP->scene->getMousePos().minus(absoluteOrigin).div(absoluteZoom);
	}

	void applyPointFromPos(Vec pos) {
		if (!module || module->editorLocked.load(std::memory_order_relaxed)) return;
		const int idx = indexFromX(pos.x);
		const float targetDisplayValue = valueFromY(pos.y);
		const float writeSlitherPhase = pointEditActive ? pointEditSlitherPhase : visualSlitherPhase;
		auto writeDisplayValue = [&](int pointIndex, float displayValue) {
			module->setWavePoint(pointIndex, displayValue - slitherOffsetForIndex(pointIndex, writeSlitherPhase));
		};
		if (lastIndex >= 0 && lastIndex != idx) {
			const int lo = std::min(lastIndex, idx);
			const int hi = std::max(lastIndex, idx);
			const float dragDx = pos.x - lastPointEditPos.x;
			for (int i = lo; i <= hi; ++i) {
				const float sampleX = pointX(i, module->pointCount);
				const float t = std::fabs(dragDx) > 1e-6f
					? clamp((sampleX - lastPointEditPos.x) / dragDx, 0.f, 1.f)
					: 1.f;
				const float sampleY = lastPointEditPos.y + (pos.y - lastPointEditPos.y) * t;
				writeDisplayValue(i, valueFromY(sampleY));
			}
		}
		else {
			writeDisplayValue(idx, targetDisplayValue);
		}
		lastIndex = idx;
		lastPointEditPos = pos;
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

	void step() override {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = module && isDragonKingDebugEnabled();
		const PerfClock::time_point stepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		TransparentWidget::step();
		const double nowSec = system::getTime();
		advanceVisualSlitherPhase(nowSec);
		if (module) {
			module->uiSlitherPhase.store(visualSlitherPhase, std::memory_order_relaxed);
		}
		auto* framebuffer = dynamic_cast<widget::FramebufferWidget*>(parent);
		if (!framebuffer) {
			return;
		}

		bool dirty = false;
		if (std::fabs(box.size.x - lastBoxSize.x) > 1e-4f || std::fabs(box.size.y - lastBoxSize.y) > 1e-4f) {
			lastBoxSize = box.size;
			dirty = true;
		}

		if (!module) {
			framebuffer->setDirty();
			return;
		}

		const bool sandEnabledNow = sandEnabled();
		const int renderModeNow = module->renderMode.load(std::memory_order_relaxed);
		const int sandBackendNow = module->sandBackend.load(std::memory_order_relaxed);
		const int sandDetailNow = module->sandDetail.load(std::memory_order_relaxed);
		const int sandPersistenceNow = module->sandPersistence.load(std::memory_order_relaxed);
		const bool editorLockedNow = module->editorLocked.load(std::memory_order_relaxed);
		const bool envelopeModeNow = module->envelopeMode.load(std::memory_order_relaxed);
		const uint32_t waveVersionNow = module->waveVersion.load(std::memory_order_acquire);
		const int pointCountNow = module->pointCount;
		const int rockStateIndexNow = module->activeRockStateIndex.load(std::memory_order_acquire);
		if (waveVersionNow != lastWaveVersion || pointCountNow != lastPointCount || rockStateIndexNow != lastRockStateIndex) {
			dirty = true;
		}
		if (sandEnabledNow != lastSandEnabledState || renderModeNow != lastRenderMode || sandBackendNow != lastSandBackend || sandDetailNow != lastSandDetail || sandPersistenceNow != lastSandPersistence) {
			dirty = true;
		}
		if (editorLockedNow != lastEditorLocked) {
			dirty = true;
		}
		if (envelopeModeNow != lastEnvelopeMode) {
			dirty = true;
		}

		const Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= box.size.x && mouseLocal.y >= 0.f && mouseLocal.y <= box.size.y);
		const int hoverRockNow = (draggingRock >= 0) ? draggingRock : (mouseInside ? rockIndexAt(mouseLocal) : -1);
		if (hoverRockNow != lastHoverRock) {
			dirty = true;
		}

		if (pointEditActive || draggingRock >= 0) {
			dirty = true;
		}
		if (effectiveSlitherAmount() > 1e-5f) {
			dirty = true;
		}
		if (sandEnabledNow && sand && sand->hasActiveVisual()) {
			dirty = true;
		}

		lastWaveVersion = waveVersionNow;
		lastPointCount = pointCountNow;
		lastRockStateIndex = rockStateIndexNow;
		lastSandEnabledState = sandEnabledNow;
		lastRenderMode = renderModeNow;
		lastSandBackend = sandBackendNow;
		lastSandDetail = sandDetailNow;
		lastSandPersistence = sandPersistenceNow;
		lastEditorLocked = editorLockedNow;
		lastEnvelopeMode = envelopeModeNow;
		lastHoverRock = hoverRockNow;

		if (dirty) {
			framebuffer->setDirty();
		}

		if (measurePerf) {
			uint32_t debugId = module->debugInstanceId;
			double& lastSubmitSec = gWyrmDebugTerminalLastSubmitSec[debugId];
				if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kWyrmDebugTerminalSubmitIntervalSec) {
					module->perfAudioSampledCount.exchange(0, std::memory_order_acq_rel);
					module->perfAudioProcessNs.exchange(0, std::memory_order_acq_rel);
					const uint64_t bodySampleCacheHits = module->perfBodySampleCacheHits.exchange(0, std::memory_order_acq_rel);
					const uint64_t bodySampleCacheMisses = module->perfBodySampleCacheMisses.exchange(0, std::memory_order_acq_rel);
					const float sandGlUs = module->perfSandGlUs.load(std::memory_order_relaxed);
					lastSubmitSec = nowSec;
					debug_terminal::submitWyrmMetrics(
						debugId,
						debug_terminal::consumeAudioProcessTiming(module->perfAudioProcessMinNs, module->perfAudioProcessMaxNs),
						stepUsRange.consume(),
						drawUsRange.consume(),
						lastEditorDrawUs,
						lastSandUpdateUs,
						lastSandDrawUs,
						sandGlUs,
						module->perfChannels.load(std::memory_order_relaxed),
						lastBodySampleCount,
						bodySampleCacheHits,
						bodySampleCacheMisses
					);
				}
			}
		if (measurePerf) {
			const float stepUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - stepStart).count()) * 0.001f;
			lastStepUsEma = (lastStepUsEma > 0.f) ? (lastStepUsEma + (stepUs - lastStepUsEma) * 0.18f) : stepUs;
			stepUsRange.add(stepUs);
		}
	}

	void draw(const DrawArgs& args) override {
		if (!args.vg) return;
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = module && isDragonKingDebugEnabled();
		const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		float sandUpdateUs = 0.f;
		float sandDrawUs = 0.f;
		const double nowSec = system::getTime();
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

			const bool hasModule = (module != nullptr);
			nvgSave(args.vg);
			nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
			const int count = hasModule ? module->pointCount : kWyrmPointCountDefault;
			const float dx = pointStep(count);
		Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= box.size.x && mouseLocal.y >= 0.f && mouseLocal.y <= box.size.y);
		hoveredRock = (draggingRock >= 0) ? draggingRock : (mouseInside ? rockIndexAt(mouseLocal) : -1);

		const float midY = 0.5f * box.size.y;
			const bool drawBodyNanoVG = !module || module->renderMode.load(std::memory_order_relaxed) == WYRM_RENDER_NANOVG;
			const int bodySampleCount = hasModule
				? wyrm_render::nanoVgBodySampleCount(box.size, count)
				: count;
			std::vector<Vec> previewBodyPoints;
			std::vector<uint8_t> previewNearRock;
			if (drawBodyNanoVG && hasModule && geometryCache) {
				geometryCache->ensure(module, box.size, bodySampleCount);
			}
			else if (drawBodyNanoVG && !hasModule) {
				previewBodyPoints.resize(size_t(bodySampleCount));
				previewNearRock.assign(size_t(bodySampleCount), 0u);
				for (int i = 0; i < bodySampleCount; ++i) {
					const float phase = (float(i) + 0.5f) / float(bodySampleCount);
					previewBodyPoints[size_t(i)] = Vec(
						pointEdgeInset() + phase * pointDrawWidth(),
						(0.5f - 0.5f * std::sin(2.f * float(M_PI) * phase)) * box.size.y);
				}
			}
			const std::vector<Vec>& bodyPathPoints = hasModule
				? geometryCache->points
				: previewBodyPoints;
			const std::vector<uint8_t>& bodyPathNearRock = hasModule
				? geometryCache->nearRock
				: previewNearRock;
			const int cachedBodySamples = int(bodyPathPoints.size());

			const bool envelopeVisual = hasModule && module->envelopeMode.load(std::memory_order_relaxed);
			const bool drawWaveArea = (!sandEnabled()) && drawBodyNanoVG && cachedBodySamples >= 2;
			const int waveMaterialImageHandle = drawWaveArea ? ensureWaveMaterialImage(args.vg, count, envelopeVisual) : -1;
			const bool useWaveMaterialImage = waveMaterialImageHandle >= 0;
			auto emitPolarityFill = [&](bool positive) {
				if (!drawWaveArea) {
					return;
				}
				auto inside = [&](const Vec& p) {
					return positive ? (p.y < midY - 1e-4f) : (p.y > midY + 1e-4f);
				};
				auto zeroCrossing = [&](const Vec& a, const Vec& b) {
					const float dy = b.y - a.y;
					const float t = (std::fabs(dy) > 1e-6f) ? clamp((midY - a.y) / dy, 0.f, 1.f) : 0.f;
					return Vec(a.x + (b.x - a.x) * t, midY);
				};
				bool open = false;
				auto beginAt = [&](const Vec& p) {
					nvgMoveTo(args.vg, p.x, midY);
					nvgLineTo(args.vg, p.x, p.y);
					open = true;
				};
				auto closeAt = [&](const Vec& p) {
					nvgLineTo(args.vg, p.x, p.y);
					nvgLineTo(args.vg, p.x, midY);
					nvgClosePath(args.vg);
					open = false;
				};

				nvgBeginPath(args.vg);
				for (int i = 0; i < cachedBodySamples - 1; ++i) {
					const Vec p0 = bodyPathPoints[i];
					const Vec p1 = bodyPathPoints[i + 1];
					const bool in0 = inside(p0);
					const bool in1 = inside(p1);
					if (in0 && !open) {
						beginAt(p0);
					}
					if (in0 && in1) {
						nvgLineTo(args.vg, p1.x, p1.y);
					}
					else if (in0 && !in1) {
						closeAt(zeroCrossing(p0, p1));
					}
					else if (!in0 && in1) {
						const Vec cross = zeroCrossing(p0, p1);
						beginAt(cross);
						nvgLineTo(args.vg, p1.x, p1.y);
					}
				}
				if (open) {
					closeAt(bodyPathPoints[cachedBodySamples - 1]);
				}
				if (useWaveMaterialImage) {
					const NVGpaint material = nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, waveMaterialImageHandle, 1.f);
					nvgFillPaint(args.vg, material);
				}
				else {
					const NVGpaint gradient = positive
						? nvgLinearGradient(args.vg, 0.f, midY, 0.f, 0.f, nvgRGBA(28, 204, 217, 46), nvgRGBA(42, 228, 255, 152))
						: nvgLinearGradient(args.vg, 0.f, midY, 0.f, box.size.y, nvgRGBA(115, 72, 224, 50), nvgRGBA(150, 92, 255, 162));
					nvgFillPaint(args.vg, gradient);
				}
				nvgFill(args.vg);
			};
			auto emitEnvelopeFill = [&]() {
				if (!drawWaveArea) {
					return;
				}
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, bodyPathPoints[0].x, bodyPathPoints[0].y);
				for (int i = 1; i < cachedBodySamples; ++i) {
					nvgLineTo(args.vg, bodyPathPoints[i].x, bodyPathPoints[i].y);
				}
				nvgLineTo(args.vg, bodyPathPoints[cachedBodySamples - 1].x, box.size.y);
				nvgLineTo(args.vg, bodyPathPoints[0].x, box.size.y);
				nvgClosePath(args.vg);
				if (useWaveMaterialImage) {
					const NVGpaint material = nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, waveMaterialImageHandle, 1.f);
					nvgFillPaint(args.vg, material);
				}
				else {
					const NVGpaint gradient = nvgLinearGradient(
						args.vg, 0.f, box.size.y, 0.f, 0.f,
						nvgRGBA(28, 204, 217, 46), nvgRGBA(42, 228, 255, 152));
					nvgFillPaint(args.vg, gradient);
				}
				nvgFill(args.vg);
			};
			if (envelopeVisual) {
				emitEnvelopeFill();
			}
			else {
				emitPolarityFill(true);
				emitPolarityFill(false);
			}

			auto emitAlternatingPolarityShade = [&](bool positive) {
				if (!drawWaveArea || useWaveMaterialImage || count <= 0) {
					return;
				}
				auto inside = [&](const Vec& p) {
					return positive ? (p.y < midY - 1e-4f) : (p.y > midY + 1e-4f);
				};
				auto zeroCrossing = [&](const Vec& a, const Vec& b) {
					const float dy = b.y - a.y;
					const float t = (std::fabs(dy) > 1e-6f) ? clamp((midY - a.y) / dy, 0.f, 1.f) : 0.f;
					return Vec(a.x + (b.x - a.x) * t, midY);
				};
				auto sampleBodyPointAtX = [&](float x) {
					const float phase = clamp((x - pointEdgeInset()) / std::max(pointDrawWidth(), 1e-6f), 0.f, 1.f);
					const float sampleIndex = phase * float(cachedBodySamples) - 0.5f;
					if (sampleIndex <= 0.f) {
						return Vec(x, bodyPathPoints[0].y);
					}
					if (sampleIndex >= float(cachedBodySamples - 1)) {
						return Vec(x, bodyPathPoints[cachedBodySamples - 1].y);
					}
					const int i0 = clamp(int(std::floor(sampleIndex)), 0, cachedBodySamples - 2);
					const float t = sampleIndex - float(i0);
					const float y = bodyPathPoints[i0].y + (bodyPathPoints[i0 + 1].y - bodyPathPoints[i0].y) * t;
					return Vec(x, y);
				};
				bool open = false;
				auto beginAt = [&](const Vec& p) {
					nvgMoveTo(args.vg, p.x, midY);
					nvgLineTo(args.vg, p.x, p.y);
					open = true;
				};
				auto closeAt = [&](const Vec& p) {
					nvgLineTo(args.vg, p.x, p.y);
					nvgLineTo(args.vg, p.x, midY);
					nvgClosePath(args.vg);
					open = false;
				};

				std::vector<Vec> columnPoints;
				columnPoints.reserve(16);
				nvgBeginPath(args.vg);
				int sampleCursor = 0;
				for (int column = 1; column < count; column += 2) {
					const float x0 = pointEdgeInset() + float(column) * dx;
					const float x1 = std::min(pointEdgeInset() + float(column + 1) * dx, pointEdgeInset() + pointDrawWidth());
					columnPoints.clear();
					columnPoints.push_back(sampleBodyPointAtX(x0));
					while (sampleCursor < cachedBodySamples && bodyPathPoints[sampleCursor].x <= x0) {
						++sampleCursor;
					}
					for (int sample = sampleCursor; sample < cachedBodySamples; ++sample) {
						const Vec p = bodyPathPoints[sample];
						if (p.x >= x1) {
							break;
						}
						columnPoints.push_back(p);
					}
					columnPoints.push_back(sampleBodyPointAtX(x1));

					open = false;
					for (size_t i = 0; i + 1 < columnPoints.size(); ++i) {
						const Vec p0 = columnPoints[i];
						const Vec p1 = columnPoints[i + 1];
						const bool in0 = inside(p0);
						const bool in1 = inside(p1);
						if (in0 && !open) {
							beginAt(p0);
						}
						if (in0 && in1) {
							nvgLineTo(args.vg, p1.x, p1.y);
						}
						else if (in0 && !in1) {
							closeAt(zeroCrossing(p0, p1));
						}
						else if (!in0 && in1) {
							const Vec cross = zeroCrossing(p0, p1);
							beginAt(cross);
							nvgLineTo(args.vg, p1.x, p1.y);
						}
					}
					if (open) {
						closeAt(columnPoints.back());
					}
				}
				nvgFillColor(args.vg, positive ? nvgRGBA(0, 56, 72, 132) : nvgRGBA(40, 24, 112, 92));
				nvgFill(args.vg);
			};
			if (!envelopeVisual) {
				emitAlternatingPolarityShade(true);
				emitAlternatingPolarityShade(false);
			}

			if (!envelopeVisual) {
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 0.f, midY);
				nvgLineTo(args.vg, box.size.x, midY);
				nvgStrokeWidth(args.vg, 1.f);
				nvgStrokeColor(args.vg, nvgRGBA(240, 180, 42, 150));
				nvgStroke(args.vg);
			}

			auto emitRoundedBodyPath = [&]() {
				const float roundCosThreshold = -0.25f;
				if (cachedBodySamples <= 0) {
					return;
				}
				if (cachedBodySamples == 1) {
					const Vec p = bodyPathPoints[0];
					nvgMoveTo(args.vg, p.x, p.y);
					return;
				}

				const Vec pStart = bodyPathPoints[0];
				nvgMoveTo(args.vg, pStart.x, pStart.y);

				for (int i = 1; i < cachedBodySamples - 1; ++i) {
					const Vec p0 = bodyPathPoints[i - 1];
					const Vec p1 = bodyPathPoints[i];
					const Vec p2 = bodyPathPoints[i + 1];
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
					if (bodyPathNearRock[i]) {
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

				const Vec pEnd = bodyPathPoints[cachedBodySamples - 1];
				nvgLineTo(args.vg, pEnd.x, pEnd.y);
			};

			if (drawBodyNanoVG) {
				nvgLineJoin(args.vg, NVG_ROUND);
				nvgLineCap(args.vg, NVG_ROUND);
				nvgBeginPath(args.vg);
				emitRoundedBodyPath();

				nvgStrokeWidth(args.vg, 4.0f);
				nvgStrokeColor(args.vg, nvgRGBA(74, 54, 24, 205));
				nvgStroke(args.vg);
				nvgStrokeWidth(args.vg, 2.6f);
				nvgStrokeColor(args.vg, nvgRGBA(167, 132, 72, 230));
				nvgStroke(args.vg);
				nvgStrokeWidth(args.vg, 1.15f);
				nvgStrokeColor(args.vg, nvgRGBA(246, 215, 136, 225));
				nvgStroke(args.vg);
			}

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
			lastEditorDrawUs = editorDrawUs;
			lastSandUpdateUs = sandUpdateUs;
			lastSandDrawUs = sandDrawUs;
			lastBodySampleCount = bodySampleCount;
			drawUsRange.add(editorDrawUs + module->perfSandGlUs.load(std::memory_order_relaxed));
		}
	}
};

struct WyrmEditorAnimationOverlay final : TransparentWidget {
	Wyrm* module = nullptr;
	bool tracerDotVisible = false;
	std::string sandPath;
	std::shared_ptr<window::Image> sandImage;
	NVGcontext* sandImageContext = nullptr;

	explicit WyrmEditorAnimationOverlay(Wyrm* m)
		: module(m)
		, sandPath(asset::plugin(pluginInstance, "res/icon/sand_white_cs_96c.png")) {
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		sandImage.reset();
		sandImageContext = e.vg;
		TransparentWidget::onContextCreate(e);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		sandImage.reset();
		sandImageContext = nullptr;
		TransparentWidget::onContextDestroy(e);
	}

	float pointEdgeInset() const {
		return wyrm_render::kPointEdgeInsetPx;
	}

	float pointDrawWidth() const {
		return wyrm_render::pointDrawWidth(box.size);
	}

	float visualRockClearance() const {
		float clearance = 0.f;
		wyrm_render::visualRockClearance(box.size, &clearance, nullptr);
		return clearance;
	}

	float visualRockPhaseClearance() const {
		float clearance = 0.f;
		wyrm_render::visualRockClearance(box.size, nullptr, &clearance);
		return clearance;
	}

	Vec currentLocalMousePos() const {
		if (!APP || !APP->scene) {
			return Vec();
		}
		auto* self = const_cast<WyrmEditorAnimationOverlay*>(this);
		const Vec absoluteOrigin = self->getAbsoluteOffset(Vec());
		const float absoluteZoom = std::max(self->getAbsoluteZoom(), 1e-6f);
		return APP->scene->getMousePos().minus(absoluteOrigin).div(absoluteZoom);
	}

	void drawHoverGuides(const DrawArgs& args) {
		if (!module || module->pointCount <= 0) {
			return;
		}
		const Vec mouseLocal = currentLocalMousePos();
		if (mouseLocal.x < 0.f || mouseLocal.x > box.size.x
			|| mouseLocal.y < 0.f || mouseLocal.y > box.size.y) {
			return;
		}
		const int count = module->pointCount;
		const float dx = pointDrawWidth() / float(std::max(count, 1));
		const int column = clamp(
			int(std::floor((mouseLocal.x - pointEdgeInset()) / std::max(dx, 1e-6f))),
			0, count - 1);
		const float x0 = pointEdgeInset() + float(column) * dx;
		const float x1 = x0 + dx;
		const float guideX = 0.5f * (x0 + x1);
		const float columnWidth = std::min(2.f, dx);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, x0, 0.f, x1 - x0, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(28, 204, 217, 72));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, guideX - 0.5f * columnWidth, 0.f, columnWidth, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(28, 204, 217, 238));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0.f, mouseLocal.y);
		nvgLineTo(args.vg, box.size.x, mouseLocal.y);
		nvgStrokeWidth(args.vg, 1.4f);
		nvgStrokeColor(args.vg, nvgRGBA(186, 154, 92, 96));
		nvgStroke(args.vg);
	}

	float displayWaveValueAtPhase(float phase) const {
		if (!module) {
			return 0.f;
		}
		std::array<float, kWyrmPointCountMax> points {};
		const int count = clamp(module->pointCount, 1, kWyrmPointCountMax);
		for (int i = 0; i < count; ++i) {
			points[i] = module->getWavePoint(i);
		}
		const float raw = catmullPeriodic(points, count, phase);
		const float clearance = visualRockClearance();
		const float phaseClearance = visualRockPhaseClearance();
		const float base = module->resolveAgainstRocks(
			raw, raw, phase, clearance, phaseClearance);
		const float slitherAmount = levi_math::clamp01(
			module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float slither = slitherAmount > 1e-5f
			? slitherOffset(
				phase,
				module->uiSlitherPhase.load(std::memory_order_relaxed),
				slitherAmount)
			: 0.f;
		return module->resolveAgainstRocks(
			base, base + slither, phase, clearance, phaseClearance);
	}

	int sandTextureHandle(NVGcontext* vg) {
		if (!vg || !APP || !APP->window) {
			return -1;
		}
		if (sandImageContext != vg) {
			sandImage.reset();
			sandImageContext = vg;
		}
		if (!sandImage) {
			sandImage = APP->window->loadImage(sandPath);
		}
		if (!sandImage || sandImage->handle < 0) {
			return -1;
		}
		const int mipmapHandle = visual_assets::loadRasterMipmapHandle(
			vg, sandImage, sandPath);
		return mipmapHandle >= 0 ? mipmapHandle : sandImage->handle;
	}

	void drawEnvelopeProgress(const DrawArgs& args, float phase) {
		const float progressX = pointEdgeInset();
		const float progressWidth = clamp(phase, 0.f, 1.f) * pointDrawWidth();
		if (progressWidth <= 0.f) {
			return;
		}

		const int sandHandle = sandTextureHandle(args.vg);
		nvgSave(args.vg);
		nvgIntersectScissor(args.vg, progressX, 0.f, progressWidth, box.size.y);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, progressX, 0.f, pointDrawWidth(), box.size.y);
		if (sandHandle >= 0) {
			nvgFillPaint(args.vg, nvgImagePattern(
				args.vg, progressX, 0.f, pointDrawWidth(), box.size.y,
				0.f, sandHandle, 0.32f));
		}
		else {
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 34));
		}
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}

	void drawTracerDot(const DrawArgs& args, float phase) {
		const float x = pointEdgeInset() + phase * pointDrawWidth();
		const float value = displayWaveValueAtPhase(phase);
		const float y = (0.5f - 0.5f * clamp(value, -1.f, 1.f)) * box.size.y;
		constexpr float dotRadius = 2.8f;

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, x, y, dotRadius);
		nvgFillColor(args.vg, nvgRGBA(148, 255, 64, 255));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, x, y, dotRadius + 0.7f);
		nvgStrokeWidth(args.vg, 1.15f);
		nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 235));
		nvgStroke(args.vg);
	}

	void draw(const DrawArgs& args) override {
		if (!module || !args.vg) {
			return;
		}
		const bool envelopeMode = module->envelopeMode.load(std::memory_order_relaxed);
		const float tracerFrequency = module->displayPhaseFrequencyHz.load(std::memory_order_relaxed);
		const bool slowTraceMode =
			module->lfoMode.load(std::memory_order_relaxed) || envelopeMode;
		if (!slowTraceMode || !std::isfinite(tracerFrequency) || tracerFrequency >= 2.4f) {
			tracerDotVisible = false;
		}
		else if (tracerFrequency > 0.f && tracerFrequency <= 2.f) {
			tracerDotVisible = true;
		}

		const float phase = levi_math::wrap01(
			module->displayPhase.load(std::memory_order_relaxed));
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		drawHoverGuides(args);
		if (envelopeMode
			&& module->displayEnvelopeRunning.load(std::memory_order_relaxed)) {
			drawEnvelopeProgress(args, phase);
		}
		if (tracerDotVisible) {
			drawTracerDot(args, phase);
		}
		nvgRestore(args.vg);
	}
};

TransparentWidget* createWyrmWaveEditor(Wyrm* module) {
	return new WyrmWaveEditor(module, std::make_shared<WyrmSand>());
}

TransparentWidget* createWyrmWaveEditor(Wyrm* module, std::shared_ptr<WyrmSand> sandState) {
	return new WyrmWaveEditor(module, sandState);
}

TransparentWidget* createWyrmWaveEditor(
	Wyrm* module,
	std::shared_ptr<WyrmSand> sandState,
	std::shared_ptr<wyrm_render::DisplayGeometryCache> geometryCache) {
	return new WyrmWaveEditor(module, std::move(sandState), std::move(geometryCache));
}

TransparentWidget* createWyrmEditorAnimationOverlay(Wyrm* module) {
	return new WyrmEditorAnimationOverlay(module);
}
