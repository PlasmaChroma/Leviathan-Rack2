#include "PuffyWidget.hpp"

#include "PanelSvgUtils.hpp"
#include "PuffyDrawDiagnostics.hpp"
#include "PuffyFishWidget.hpp"
#include "PuffyTransferPreviewWidget.hpp"
#include "PuffyVisualPalette.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PreviewSurface.hpp"
#include "visual/VisualAssets.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {

static const char* const kPuffyCharacterLabels[puffy::kCharacterCount] = {
	"BLOOM",
	"SPINE",
	"FRENZY",
	"RIPTIDE",
	"VOID",
	"SWARM",
	"TEETH"
};

struct PuffyViewportGradient final : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const float inset = mm2px(0.16f);
		const float width = std::max(0.f, box.size.x - 2.f * inset);
		const float height = std::max(0.f, box.size.y - 2.f * inset);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, inset, inset, width, height);
		nvgFillPaint(args.vg, nvgLinearGradient(
			args.vg,
			inset,
			inset,
			inset + width,
			inset + height,
			nvgRGB(38, 31, 53),
			nvgRGB(21, 18, 31)));
		nvgFill(args.vg);
	}
};

struct PuffyCharacterMenuItem final : MenuItem {
	Puffy* module = nullptr;
	bool negativePart = true;
	int character = int(puffy::Character::Bloom);

	void onAction(const event::Action& e) override {
		if (module) {
			const int paramId = negativePart
				? Puffy::CHARACTER_PARAM
				: Puffy::POSITIVE_CHARACTER_PARAM;
			module->params[paramId].setValue(float(character));
			module->synchronizeCharacterSelectionFromUi(negativePart);
		}
		MenuItem::onAction(e);
	}

	void step() override {
		if (module) {
			const int paramId = negativePart
				? Puffy::CHARACTER_PARAM
				: Puffy::POSITIVE_CHARACTER_PARAM;
			rightText = int(std::lround(module->params[paramId].getValue()))
				== character ? "✓" : "";
		}
		MenuItem::step();
	}

	void draw(const DrawArgs& args) override {
		const std::string label = text;
		text.clear();
		MenuItem::draw(args);
		text = label;
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		// Draw the label ourselves so it inherits the character color, while the
		// base MenuItem still supplies Rack's hover and checkmark behavior.
		nvgFontSize(args.vg, 14.f);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, puffy_visual::characterTint(character));
		nvgText(args.vg, 10.f, 0.5f * box.size.y, label.c_str(), nullptr);
	}
};

struct PuffyCharacterMenuButton final : ParamWidget {
	bool negativePart = true;
	bool isHovered = false;

	Puffy* getPuffyModule() const {
		return dynamic_cast<Puffy*>(module);
	}

	void step() override {
		ParamWidget::step();
		Puffy* puffyModule = getPuffyModule();
		if (puffyModule && !negativePart) {
			const bool linked = puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
			visible = !linked;
			if (linked) {
				isHovered = false;
			}
		}
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		ParamWidget::onEnter(e);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		ParamWidget::onLeave(e);
	}

	void onButton(const event::Button& e) override {
		Puffy* puffyModule = getPuffyModule();
		if (!puffyModule || e.button != GLFW_MOUSE_BUTTON_LEFT
			|| e.action != GLFW_PRESS) {
			ParamWidget::onButton(e);
			return;
		}
		const bool charactersLinked = puffyModule
			->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f;
		ui::Menu* menu = createMenu();
		menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
		menu->addChild(createMenuLabel(
			charactersLinked
				? "Both Polarities"
				: (negativePart ? "Negative Character" : "Positive Character")));
		for (int character = 0; character < puffy::kCharacterCount; ++character) {
			auto* item = new PuffyCharacterMenuItem();
			item->module = puffyModule;
			item->negativePart = negativePart;
			item->character = character;
			item->text = kPuffyCharacterLabels[character];
			menu->addChild(item);
		}
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		Puffy* puffyModule = getPuffyModule();
		const int paramId = negativePart
			? Puffy::CHARACTER_PARAM
			: Puffy::POSITIVE_CHARACTER_PARAM;
		const int character = puffyModule
			? clamp(int(std::lround(puffyModule->params[paramId].getValue())),
				0, puffy::kCharacterCount - 1)
			: int(puffy::Character::Bloom);
		const NVGcolor tint = puffy_visual::characterTint(character);
		NVGcolor borderTint = tint;
		float strokeWidth = 0.85f;
		NVGcolor fillColor = nvgRGBA(8, 7, 13, 218);
		if (isHovered) {
			borderTint.a = 1.0f;
			strokeWidth = 1.35f;
			fillColor = nvgRGBA(24, 20, 36, 240);
		}
		else {
			borderTint.a = 0.82f;
			strokeWidth = 0.85f;
		}
		const float radius = 0.5f * box.size.y;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, radius);
		nvgFillColor(args.vg, fillColor);
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, borderTint);
		nvgStrokeWidth(args.vg, strokeWidth);
		nvgStroke(args.vg);
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		nvgFontSize(args.vg, 8.5f);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		const float leftPadding = 4.5f;
		const float arrowAreaWidth = 8.f;
		const float labelCenterX = leftPadding + 0.5f * std::max(1.f, box.size.x - leftPadding - arrowAreaWidth);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, tint);
		nvgText(args.vg, labelCenterX, 0.5f * box.size.y,
			kPuffyCharacterLabels[character], nullptr);
		// Keep the disclosure arrow fixed near the right edge inside the pill.
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, box.size.x - 4.5f, 0.5f * box.size.y, "▾", nullptr);
	}
};

struct PuffyPolarityLinkButton final : ParamWidget {
	bool isHovered = false;
	ui::Tooltip* tooltip = nullptr;

	PuffyPolarityLinkButton() {
		box.size = Vec(16.f, 16.f);
	}

	~PuffyPolarityLinkButton() override {
		destroyTooltip();
	}

	std::string getTooltipString() {
		auto* puffyModule = dynamic_cast<Puffy*>(module);
		const bool isLinked = puffyModule
			? (puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f)
			: (getParamQuantity() ? getParamQuantity()->getValue() > 0.5f : true);
		return isLinked ? "Unlink Polarity" : "Link Polarity";
	}

	void createTooltip() {
		if (!settings::tooltips || tooltip || !APP || !APP->scene) {
			return;
		}
		tooltip = new ui::Tooltip();
		tooltip->text = getTooltipString();
		APP->scene->addChild(tooltip);
	}

	void destroyTooltip() {
		if (tooltip) {
			if (tooltip->parent) {
				tooltip->parent->removeChild(tooltip);
			}
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void refreshTooltip() {
		if (tooltip) {
			tooltip->text = getTooltipString();
		}
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		createTooltip();
		event::Enter eCopy = e;
		OpaqueWidget::onEnter(eCopy);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		destroyTooltip();
		event::Leave eCopy = e;
		OpaqueWidget::onLeave(eCopy);
	}

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			auto* puffyModule = dynamic_cast<Puffy*>(module);
			if (puffyModule) {
				const float curVal = puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue();
				const float newVal = (curVal > 0.5f) ? 0.f : 1.f;
				puffyModule->params[Puffy::CHARACTER_LINK_PARAM].setValue(newVal);
				if (newVal > 0.5f) {
					puffyModule->synchronizeCharacterSelectionFromUi(true);
				}
			}
			else if (getParamQuantity()) {
				const float curVal = getParamQuantity()->getValue();
				getParamQuantity()->setValue(curVal > 0.5f ? 0.f : 1.f);
			}
			refreshTooltip();
			e.consume(this);
			return;
		}
		ParamWidget::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		auto* puffyModule = dynamic_cast<Puffy*>(module);
		const bool isLinked = puffyModule
			? (puffyModule->params[Puffy::CHARACTER_LINK_PARAM].getValue() > 0.5f)
			: (getParamQuantity() ? getParamQuantity()->getValue() > 0.5f : true);

		// Icon color selection
		NVGcolor iconColor;
		if (isLinked) {
			iconColor = isHovered ? nvgRGB(255, 255, 255) : nvgRGB(150, 150, 165);
		}
		else {
			iconColor = isHovered ? nvgRGB(245, 245, 255) : nvgRGBA(140, 140, 160, 160);
		}

		// Draw larger chain icon with realistic physical proportions & 3D weave outlines
		nvgSave(args.vg);
		nvgTranslate(args.vg, box.size.x * 0.5f, box.size.y * 0.5f);
		nvgRotate(args.vg, -M_PI / 4.f);

		const float strokeW = 1.1f;
		const float outlineW = strokeW + 1.6f;
		const float linkW = 7.6f;
		const float linkH = 4.6f;
		const float linkR = 2.0f;
		const NVGcolor outlineColor = nvgRGBA(6, 5, 12, 245);

		if (isLinked) {
			const float offsetX1 = 2.2f; // Bottom-left link shifted slightly more left
			const float offsetX2 = 1.8f;
			const float offsetY = 0.5f;

			const Vec c1(-offsetX1, -offsetY); // Bottom-left link
			const Vec c2(offsetX2, offsetY);   // Top-right link (offset perpendicularly)

			// Draw Link 1 first, then Link 2 over it. The overlap outline is an
			// intentional depth cue at the lower crossover.
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);

			// At the opposite crossover, locally bring Link 1 back over Link 2.
			// Dark outline uses NVG_BUTT line caps so it stays hidden under Link 1's
			// grey core without creating dark line cuts across the top strand.
			nvgLineCap(args.vg, NVG_BUTT);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c1.x - 0.2f, c1.y - linkH * 0.5f);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f - linkR, c1.y - linkH * 0.5f);
			nvgArcTo(args.vg, c1.x + linkW * 0.5f, c1.y - linkH * 0.5f, c1.x + linkW * 0.5f, c1.y, linkR);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f, c1.y + 0.2f);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgLineCap(args.vg, NVG_ROUND);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, c1.x - 0.6f, c1.y - linkH * 0.5f);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f - linkR, c1.y - linkH * 0.5f);
			nvgArcTo(args.vg, c1.x + linkW * 0.5f, c1.y - linkH * 0.5f, c1.x + linkW * 0.5f, c1.y, linkR);
			nvgLineTo(args.vg, c1.x + linkW * 0.5f, c1.y + 0.5f);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);
		}
		else {
			const float offsetX = 4.8f;
			const float offsetY = 0.8f;

			const Vec c1(-offsetX, -offsetY);
			const Vec c2(offsetX, offsetY);

			// Link 1 (separated) - 3D outline + core
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c1.x - linkW * 0.5f, c1.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);

			// Link 2 (separated) - 3D outline + core
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, outlineColor);
			nvgStrokeWidth(args.vg, outlineW);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, c2.x - linkW * 0.5f, c2.y - linkH * 0.5f, linkW, linkH, linkR);
			nvgStrokeColor(args.vg, iconColor);
			nvgStrokeWidth(args.vg, strokeW);
			nvgStroke(args.vg);
		}

		nvgRestore(args.vg);
	}
};

bool loadPoint(
	const std::string& panelPath,
	const char* id,
	Vec fallbackMm,
	Vec* pointMm) {
	if (panel_svg::loadPointFromSvgMm(panelPath, id, pointMm)) {
		return true;
	}
	*pointMm = fallbackMm;
	return false;
}

} // namespace

std::string PuffyWidget::drawLogRootPath() {
	return system::join(asset::user(), "Leviathan/Puffy");
}

std::string PuffyWidget::drawLogDateTimeStamp() {
	std::time_t now = std::time(nullptr);
	std::tm tm {};
#if defined(_WIN32)
	localtime_s(&tm, &now);
#else
	localtime_r(&now, &tm);
#endif
	std::ostringstream out;
	out << std::put_time(&tm, "%Y%m%d_%H%M%S");
	return out.str();
}

void PuffyWidget::stopDrawLog() {
	if (drawLogFile.is_open()) {
		drawLogFile.close();
	}
	drawLogPath.clear();
	drawLogActive = false;
	drawLogRowCounter = 0u;
	consumePuffyDrawMetrics();
}

void PuffyWidget::syncDrawLog(bool enabled, std::uint32_t instanceId) {
	if (!enabled) {
		if (drawLogActive) {
			stopDrawLog();
		}
		return;
	}
	if (drawLogActive && drawLogFile.is_open()) {
		return;
	}
	const std::string root = drawLogRootPath();
	if (!system::createDirectories(root) && !system::isDirectory(root)) {
		WARN("Puffy failed to create draw log directory: %s", root.c_str());
		return;
	}
	static std::uint32_t openSequence = 0u;
	drawLogPath = system::join(
		root,
		"puffy_draw_" + std::to_string(instanceId) + "_"
			+ drawLogDateTimeStamp() + "_"
			+ std::to_string(openSequence++) + ".csv");
	drawLogFile.open(drawLogPath.c_str(), std::ios::out | std::ios::trunc);
	if (!drawLogFile.is_open()) {
		WARN("Puffy failed to open draw log CSV: %s", drawLogPath.c_str());
		drawLogPath.clear();
		return;
	}
	drawLogActive = true;
	drawLogRowCounter = 0u;
	consumePuffyDrawMetrics();
	drawLogFile << std::fixed << std::setprecision(3);
	drawLogFile
		<< "row,module_id,instance_id,time_sec,last_frame_us,total_draw_us,module_widget_draw_us,"
		<< "fish_draw_us,fish_other_us,body_ensure_us,body_recolor_us,body_upload_us,body_draw_us,"
		<< "body_transition_draw_us,body_transition_atlas_prewarm_us,"
		<< "fin_draw_us,eye_draw_us,transfer_draw_us,transfer_curve_draw_us,transfer_curve_rebuild_us,"
		<< "body_cache_hits,body_recolors,body_image_creates,body_image_updates,body_context_resets,"
		<< "body_fallback_draws,body_transition_draws,body_transition_atlas_creates,body_transition_atlas_resets,"
		<< "body_transition_atlas_prewarms,"
		<< "transfer_curve_rebuilds,negative_character,positive_character,"
		<< "effective_amount,input_activity,gain_reduction\n";
}

PuffyWidget::~PuffyWidget() {
	stopDrawLog();
}

void PuffyWidget::step() {
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
	ModuleWidget::step();
	if (measurePerf) {
		debugWidgetMetrics.recordStep(
			debug_terminal::elapsedUsSince(stepStart));
	}
}

void PuffyWidget::draw(const DrawArgs& args) {
	using PerfClock = std::chrono::steady_clock;
	auto* puffyModule = static_cast<Puffy*>(module);
	const bool logDraw = puffyModule
		&& isDragonKingDebugEnabled()
		&& isPuffyDrawLoggingEnabled();
	const std::uint32_t instanceId = puffyModule
		? puffyModule->debugMetrics.instanceId : 0u;
	syncDrawLog(logDraw, instanceId);
	const PerfClock::time_point totalStart = logDraw
		? PerfClock::now() : PerfClock::time_point();
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
	ModuleWidget::draw(args);
	const PerfClock::time_point afterModuleDraw = logDraw
		? PerfClock::now() : PerfClock::time_point();
	if (!puffyModule) {
		return;
	}
	if (measurePerf) {
		debug_terminal::drawDebugInstanceId(
			args.vg, box.size, puffyModule->debugMetrics.instanceId);
		debugWidgetMetrics.recordDraw(
			debug_terminal::elapsedUsSince(drawStart));
		const double nowSec = system::getTime();
		if (debug_terminal::baselineSubmitDue(
				"Puffy", puffyModule->debugMetrics.instanceId, nowSec)) {
			debug_terminal::submitBaselineMetrics(
				"Puffy",
				puffyModule->debugMetrics.instanceId,
				puffyModule->debugMetrics.consumeProcessRange(),
				debugWidgetMetrics.consumeStepRange(),
				debugWidgetMetrics.consumeDrawRange());
		}
	}
	if (logDraw && drawLogActive && drawLogFile.is_open()) {
		const PuffyDrawMetrics metrics = consumePuffyDrawMetrics();
		PuffyVisualState visual;
		puffyModule->readVisualState(&visual);
		const float fishMeasuredUs = float(
			metrics.bodyEnsureNs
			+ metrics.bodyDrawNs
			+ metrics.bodyTransitionDrawNs
			+ metrics.bodyTransitionAtlasPrewarmNs
			+ metrics.finDrawNs
			+ metrics.eyeDrawNs) * 1e-3f;
		const float fishDrawUs = float(metrics.fishDrawNs) * 1e-3f;
		const double lastFrameSec = APP && APP->window
			? APP->window->getLastFrameDuration() : 0.0;
		drawLogFile
			<< drawLogRowCounter << ','
			<< puffyModule->id << ','
			<< instanceId << ','
			<< system::getTime() << ','
			<< lastFrameSec * 1e6 << ','
			<< float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - totalStart).count()) * 1e-3f << ','
			<< float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				afterModuleDraw - totalStart).count()) * 1e-3f << ','
			<< fishDrawUs << ','
			<< std::max(0.f, fishDrawUs - fishMeasuredUs) << ','
			<< float(metrics.bodyEnsureNs) * 1e-3f << ','
			<< float(metrics.bodyRecolorNs) * 1e-3f << ','
			<< float(metrics.bodyUploadNs) * 1e-3f << ','
			<< float(metrics.bodyDrawNs) * 1e-3f << ','
			<< float(metrics.bodyTransitionDrawNs) * 1e-3f << ','
			<< float(metrics.bodyTransitionAtlasPrewarmNs) * 1e-3f << ','
			<< float(metrics.finDrawNs) * 1e-3f << ','
			<< float(metrics.eyeDrawNs) * 1e-3f << ','
			<< float(metrics.transferDrawNs) * 1e-3f << ','
			<< float(metrics.transferCurveDrawNs) * 1e-3f << ','
			<< float(metrics.transferCurveRebuildNs) * 1e-3f << ','
			<< metrics.bodyCacheHits << ','
			<< metrics.bodyRecolors << ','
			<< metrics.bodyImageCreates << ','
			<< metrics.bodyImageUpdates << ','
			<< metrics.bodyContextResets << ','
			<< metrics.bodyFallbackDraws << ','
			<< metrics.bodyTransitionDraws << ','
			<< metrics.bodyTransitionAtlasCreates << ','
			<< metrics.bodyTransitionAtlasResets << ','
			<< metrics.bodyTransitionAtlasPrewarms << ','
			<< metrics.transferCurveRebuilds << ','
			<< visual.negativeCharacter << ','
			<< visual.positiveCharacter << ','
			<< visual.effectiveAmount << ','
			<< visual.inputActivity << ','
			<< visual.gainReduction << '\n';
		if ((drawLogRowCounter++ & 31u) == 0u) {
			drawLogFile.flush();
		}
	}
}

PuffyWidget::PuffyWidget(Puffy* module) {
	setModule(module);
	PreviewBuildLogTimer previewTimer("Puffy", module);
	visual_assets::SplitPanelRenderer splitPanel(
		this, "res/Puffy.panel.svg");
	const std::string& panelPath = splitPanel.panelPath();
	splitPanel.addLabels("res/Puffy.labels.svg");
	visual_assets::addFractalGlassOverlay(
		this, panelPath, splitPanel.panelSurfaceEffectWidget());
	math::Rect leviathanLogoRectMm(
		Vec(14.120335f, 118.43102f),
		Vec(32.71933f, 12.24054f));
	panel_svg::loadRectFromSvgMm(
		panelPath,
		"BRANDING_LEVIATHAN_LOGO_RASTER",
		&leviathanLogoRectMm);
	addChild(visual_assets::createAspectFitRasterImageWidget(
		"res/icon/Leviathan_Logo_S2.png",
		leviathanLogoRectMm));
	previewTimer.markPanelDone();
	previewTimer.setAtlasStatus(
		panel_svg::getAtlasStatusLabelForSvg(panelPath));

	auto anchor = [&](const char* id, Vec fallbackMm) {
		Vec point;
		loadPoint(panelPath, id, fallbackMm, &point);
		return mm2px(point);
	};

	math::Rect fishRectMm;
	if (!panel_svg::loadRectFromSvgMm(
			panelPath, "fish_rect", &fishRectMm)) {
		fishRectMm.pos = Vec(4.f, 20.f);
		fishRectMm.size = Vec(52.96f, 48.f);
	}
	addChild(visual_assets::createPreviewFrameEnhancementWidget(fishRectMm));
	const float viewportInsetMm = 0.2f;
	math::Rect fishContentRectMm = fishRectMm;
	fishContentRectMm.pos = fishContentRectMm.pos.plus(
		Vec(viewportInsetMm));
	fishContentRectMm.size = fishContentRectMm.size.minus(
		Vec(2.f * viewportInsetMm));
	auto* viewportFramebuffer = new widget::FramebufferWidget();
	viewportFramebuffer->box.pos = mm2px(fishContentRectMm.pos);
	viewportFramebuffer->box.size = mm2px(fishContentRectMm.size);
	viewportFramebuffer->dirtyOnSubpixelChange = false;
	auto* viewportGradient = new PuffyViewportGradient();
	viewportGradient->box.size = viewportFramebuffer->box.size;
	viewportFramebuffer->addChild(viewportGradient);
	addChild(viewportFramebuffer);

	auto* fish = new PuffyFishWidget(module);
	fish->box.pos = mm2px(fishContentRectMm.pos);
	fish->box.size = mm2px(fishContentRectMm.size);
	addChild(fish);

	const Vec characterMenuSize = mm2px(Vec(13.5f, 4.5f));
	const Vec characterMenuInset = mm2px(Vec(0.6f, 0.6f));
	auto* negativeCharacterMenu = createParam<PuffyCharacterMenuButton>(
		Vec(), module, Puffy::CHARACTER_PARAM);
	negativeCharacterMenu->negativePart = true;
	negativeCharacterMenu->box.size = characterMenuSize;
	negativeCharacterMenu->box.pos = fish->box.pos.plus(Vec(
		characterMenuInset.x,
		fish->box.size.y - characterMenuSize.y - characterMenuInset.y));
	addParam(negativeCharacterMenu);

	auto* polarityLinkIcon = createParam<PuffyPolarityLinkButton>(
		Vec(
			negativeCharacterMenu->box.pos.x,
			negativeCharacterMenu->box.pos.y - 14.f - 3.f),
		module,
		Puffy::CHARACTER_LINK_PARAM);
	addParam(polarityLinkIcon);

	auto* positiveCharacterMenu = createParam<PuffyCharacterMenuButton>(
		Vec(), module, Puffy::POSITIVE_CHARACTER_PARAM);
	positiveCharacterMenu->negativePart = false;
	positiveCharacterMenu->box.size = characterMenuSize;
	positiveCharacterMenu->box.pos = fish->box.pos.plus(Vec(
		fish->box.size.x - characterMenuSize.x - characterMenuInset.x,
		fish->box.size.y - characterMenuSize.y - characterMenuInset.y));
	addParam(positiveCharacterMenu);

	math::Rect transferPreviewRectMm;
	if (!panel_svg::loadRectFromSvgMm(
		panelPath, "transfer_preview_rect", &transferPreviewRectMm)) {
		transferPreviewRectMm.pos = Vec(4.5f, 67.f);
		transferPreviewRectMm.size = Vec(51.96f, 10.5f);
	}
	addChild(visual_assets::createPreviewFrameEnhancementWidget(
		transferPreviewRectMm));
	const float previewInsetMm = 0.2f;
	transferPreviewRectMm.pos = transferPreviewRectMm.pos.plus(
		Vec(previewInsetMm));
	transferPreviewRectMm.size = transferPreviewRectMm.size.minus(
		Vec(2.f * previewInsetMm));
	const Vec transferPreviewPos = mm2px(transferPreviewRectMm.pos);
	const Vec transferPreviewSize = mm2px(transferPreviewRectMm.size);
	auto* transferSurface = preview_surface::createCachedOpaqueGrid(
		transferPreviewSize);
	transferSurface->box.pos = transferPreviewPos;
	addChild(transferSurface);
	auto* transferPreview = new PuffyTransferPreviewWidget(module);
	transferPreview->box.pos = transferPreviewPos;
	transferPreview->box.size = transferPreviewSize;
	addChild(transferPreview);

	addParam(createParamCentered<LeviathanHaloKnob2>(
		anchor("puff_param", Vec(18.f, 83.5f)),
		module, Puffy::PUFF_PARAM));
	auto* sensitivityKnob = createParamCentered<Eclipse2Knob>(
		anchor("sensitivity_param", Vec(44.f, 83.5f)),
		module, Puffy::SENSITIVITY_PARAM);
	sensitivityKnob->setProgressRingBipolar(true);
	addParam(sensitivityKnob);
	addParam(createParamCentered<Eclipse2Knob>(
		anchor("mix_param", Vec(51.96f, 97.5f)),
		module, Puffy::MIX_PARAM));
	auto* puffCvAmountKnob = createParamCentered<Eclipse2Knob>(
		anchor("puff_cv_amount_param", Vec(30.48f, 96.f)),
		module, Puffy::PUFF_CV_AMOUNT_PARAM);
	puffCvAmountKnob->setProgressRingBipolar(true);
	addParam(puffCvAmountKnob);

	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_l", Vec(7.62f, 111.93589f)),
		module, Puffy::INPUT_L));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("input_r", Vec(22.86f, 111.93589f)),
		module, Puffy::INPUT_R));
	addInput(createInputCentered<Magitek2InputJack>(
		anchor("puff_cv_input", Vec(43.f, 108.f)),
		module, Puffy::PUFF_CV_INPUT));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_l", Vec(38.10f, 111.93589f)),
		module, Puffy::OUTPUT_L));
	addOutput(createOutputCentered<Magitek2OutputJack>(
		anchor("output_r", Vec(53.34f, 111.93589f)),
		module, Puffy::OUTPUT_R));

	addChild(createLightCentered<SmallAperture<RedApertureLight>>(
		anchor("limit_light", Vec(45.72f, 111.93589f)),
		module, Puffy::LIMIT_LIGHT));

	addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
	addChild(createWidget<CyanOrbScrew>(
		Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
	addChild(createWidget<CyanOrbScrew>(
		Vec(RACK_GRID_WIDTH, box.size.y - RACK_GRID_WIDTH)));
	addChild(createWidget<CyanOrbScrew>(
		Vec(
			box.size.x - 2.f * RACK_GRID_WIDTH,
			box.size.y - RACK_GRID_WIDTH)));
	previewTimer.markAnchorsDone();
}

void PuffyWidget::appendContextMenu(Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	auto* puffyModule = dynamic_cast<Puffy*>(module);
	if (!puffyModule) {
		return;
	}
	menu->addChild(new MenuSeparator());
	menu->addChild(createCheckMenuItem(
		"Auto Deflate",
		"",
		[puffyModule]() {
			return puffyModule->autoDeflateEnabled.load(
				std::memory_order_relaxed);
		},
		[puffyModule]() {
			const bool enabled = puffyModule->autoDeflateEnabled.load(
				std::memory_order_relaxed);
			puffyModule->autoDeflateEnabled.store(
				!enabled, std::memory_order_relaxed);
		}));
}

Model* modelPuffy = createModel<Puffy, PuffyWidget>("Puffy");
