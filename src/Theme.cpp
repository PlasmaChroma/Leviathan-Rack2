#include "plugin.hpp"

#include "theme/ThemePersistence.hpp"
#include "theme/ThemePresets.hpp"
#include "theme/ThemeService.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {

using leviathan::theme::FactoryPreset;
using leviathan::theme::ThemeColor;
using leviathan::theme::ThemeRole;
using leviathan::theme::ThemeSnapshot;

struct ThemeModule : Module {
	enum ParamId { PARAMS_LEN };
	enum InputId { INPUTS_LEN };
	enum OutputId { OUTPUTS_LEN };
	enum LightId { LIGHTS_LEN };

	ThemeModule() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	}
};

NVGcolor nvgThemeColor(ThemeColor color, unsigned char alpha = 255) {
	return nvgRGBA(color.r, color.g, color.b, alpha);
}

ThemeColor colorForRole(const ThemeSnapshot& snapshot, ThemeRole role) {
	switch (role) {
		case ThemeRole::Input: return snapshot.colors.input;
		case ThemeRole::Output: return snapshot.colors.output;
		case ThemeRole::Text: return snapshot.colors.text;
		default: return {};
	}
}

// Repainting every semantic glass surface is library-wide work. The editor
// remains locally smooth while global drag publications are limited to 1 Hz.
constexpr double kThemeGlobalPublishIntervalSec = 1.0;

void rgbToHsv(ThemeColor color, float* hue, float* saturation, float* value) {
	const float r = color.r / 255.f;
	const float g = color.g / 255.f;
	const float b = color.b / 255.f;
	const float hi = std::max(r, std::max(g, b));
	const float lo = std::min(r, std::min(g, b));
	const float range = hi - lo;
	float h = 0.f;
	if (range > 1e-6f) {
		if (hi == r) h = std::fmod((g - b) / range, 6.f);
		else if (hi == g) h = (b - r) / range + 2.f;
		else h = (r - g) / range + 4.f;
		h /= 6.f;
		if (h < 0.f) h += 1.f;
	}
	if (hue) *hue = h;
	if (saturation) *saturation = hi > 1e-6f ? range / hi : 0.f;
	if (value) *value = hi;
}

ThemeColor hsvToRgb(float hue, float saturation, float value) {
	hue = hue - std::floor(hue);
	saturation = clamp(saturation, 0.f, 1.f);
	value = clamp(value, 0.f, 1.f);
	const float scaled = hue * 6.f;
	const int sector = int(std::floor(scaled));
	const float fraction = scaled - sector;
	const float p = value * (1.f - saturation);
	const float q = value * (1.f - saturation * fraction);
	const float t = value * (1.f - saturation * (1.f - fraction));
	float r = value, g = t, b = p;
	switch (sector % 6) {
		case 0: r = value; g = t; b = p; break;
		case 1: r = q; g = value; b = p; break;
		case 2: r = p; g = value; b = t; break;
		case 3: r = p; g = q; b = value; break;
		case 4: r = t; g = p; b = value; break;
		case 5: r = value; g = p; b = q; break;
	}
	return ThemeColor(
		std::uint8_t(std::round(r * 255.f)),
		std::uint8_t(std::round(g * 255.f)),
		std::uint8_t(std::round(b * 255.f)));
}

struct ThemeEditor final : TransparentWidget {
	enum DragTarget { DragNone, DragSv, DragHue, DragTexture };

	widget::FramebufferWidget* framebuffer = nullptr;
	visual_assets::FractalGlassOverlay* themePreviewOverlay = nullptr;
	ThemeRole selectedRole = ThemeRole::Input;
	ThemeRole textPreviewBackgroundRole = ThemeRole::Input;
	DragTarget dragTarget = DragNone;
	std::uint64_t observedGeneration = 0u;
	bool pickerValid = false;
	ThemeRole pickerRole = ThemeRole::None;
	ThemeColor pickerColor;
	float pickerHue = 0.f;
	float pickerSaturation = 0.f;
	float pickerValue = 0.f;
	float retainedHue[3] = {0.f, 0.f, 0.f};
	float pickerTextureAmount = 1.f;
	bool pickerTextureValid = false;
	bool textureHovered = false;
	double lastGlobalPublishAt = NAN;

	math::Rect roleRect(int index) const { return math::Rect(Vec(8.f + index * 56.f, 42.f), Vec(52.f, 29.f)); }
	math::Rect svRect() const { return math::Rect(Vec(9.f, 82.f), Vec(137.f, 124.f)); }
	math::Rect hueRect() const { return math::Rect(Vec(151.f, 82.f), Vec(20.f, 124.f)); }
	math::Rect textureRect() const {
		return math::Rect(
			Vec(10.f, 235.f),
			Vec(160.f, visual_assets::neonBarSliderAssetHeight(160.f)));
	}
	math::Rect presetRect(int index) const {
		return math::Rect(Vec(9.f + (index % 2) * 82.f, 274.f + (index / 2) * 29.f), Vec(78.f, 24.f));
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		visual_assets::onRasterContextCreate(e.vg);
		TransparentWidget::onContextCreate(e);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		visual_assets::onRasterContextDestroy(e.vg);
		TransparentWidget::onContextDestroy(e);
	}

	Vec currentLocalMousePos() const {
		if (!APP || !APP->scene) return Vec();
		auto* self = const_cast<ThemeEditor*>(this);
		const Vec origin = self->getAbsoluteOffset(Vec());
		const float zoom = std::max(self->getAbsoluteZoom(), 1e-6f);
		return APP->scene->getMousePos().minus(origin).div(zoom);
	}

	void dirty() {
		if (framebuffer) framebuffer->dirty = true;
	}

	int selectedRoleIndex() const {
		switch (selectedRole) {
			case ThemeRole::Input: return 0;
			case ThemeRole::Output: return 1;
			case ThemeRole::Text: return 2;
			default: return 0;
		}
	}

	void syncPicker(const ThemeSnapshot& snapshot) {
		const ThemeColor current = colorForRole(snapshot, selectedRole);
		if (pickerValid && pickerRole == selectedRole && pickerColor == current) return;
		float hue = 0.f;
		float saturation = 0.f;
		float value = 0.f;
		rgbToHsv(current, &hue, &saturation, &value);
		const int roleIndex = selectedRoleIndex();
		if (saturation > 1e-4f && value > 1e-4f)
			retainedHue[roleIndex] = hue;
		pickerHue = retainedHue[roleIndex];
		pickerSaturation = saturation;
		pickerValue = value;
		pickerColor = current;
		pickerRole = selectedRole;
		pickerValid = true;
	}

	void publishDragValue(bool force) {
		if (dragTarget == DragNone) return;
		const double now = system::getTime();
		if (!force && std::isfinite(now) && std::isfinite(lastGlobalPublishAt)
			&& now - lastGlobalPublishAt < kThemeGlobalPublishIntervalSec) {
			return;
		}
		if (dragTarget == DragSv || dragTarget == DragHue)
			leviathan::theme::setColor(selectedRole, pickerColor);
		else if (dragTarget == DragTexture && pickerTextureValid)
			leviathan::theme::setTextureAmount(pickerTextureAmount);
		lastGlobalPublishAt = now;
	}

	void applyDrag(Vec pos) {
		const ThemeSnapshot snapshot = leviathan::theme::read().snapshot;
		if (!pickerValid || pickerRole != selectedRole)
			syncPicker(snapshot);
		if (dragTarget == DragSv) {
			const math::Rect area = svRect();
			pickerSaturation = clamp((pos.x - area.pos.x) / area.size.x, 0.f, 1.f);
			pickerValue = 1.f - clamp((pos.y - area.pos.y) / area.size.y, 0.f, 1.f);
			pickerColor = hsvToRgb(pickerHue, pickerSaturation, pickerValue);
		}
		else if (dragTarget == DragHue) {
			const math::Rect area = hueRect();
			pickerHue = clamp((pos.y - area.pos.y) / area.size.y, 0.f, 0.999999f);
			retainedHue[selectedRoleIndex()] = pickerHue;
			pickerColor = hsvToRgb(pickerHue, pickerSaturation, pickerValue);
		}
		else if (dragTarget == DragTexture) {
			const math::Rect area = textureRect();
			const float normalized = visual_assets::neonBarSliderValueFromX(
				pos.x - area.pos.x, area.size.x);
			pickerTextureAmount = normalized * 2.f;
			pickerTextureValid = true;
			if (themePreviewOverlay)
				themePreviewOverlay->setTextureAmountPreview(pickerTextureAmount);
		}
		publishDragValue(false);
		dirty();
	}

	void commitDrag() {
		if (dragTarget != DragNone) {
			// Preserve the exact release position even if it falls between throttled
			// publications, then persist only that final global value.
			publishDragValue(true);
			leviathan::theme::persistence::saveToUserStorage();
		}
		if (dragTarget == DragTexture && themePreviewOverlay)
			themePreviewOverlay->setTextureAmountPreview(NAN);
		dragTarget = DragNone;
		pickerTextureValid = false;
		lastGlobalPublishAt = NAN;
	}

	void onButton(const event::Button& e) override {
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
			TransparentWidget::onButton(e);
			return;
		}
		if (e.action == GLFW_RELEASE) {
			if (dragTarget != DragNone) {
				commitDrag();
				e.consume(this);
				return;
			}
			TransparentWidget::onButton(e);
			return;
		}
		if (e.action != GLFW_PRESS) {
			TransparentWidget::onButton(e);
			return;
		}

		for (int i = 0; i < 3; ++i) {
			if (roleRect(i).contains(e.pos)) {
				selectedRole = i == 0 ? ThemeRole::Input : (i == 1 ? ThemeRole::Output : ThemeRole::Text);
				if (selectedRole == ThemeRole::Input || selectedRole == ThemeRole::Output)
					textPreviewBackgroundRole = selectedRole;
				pickerValid = false;
				dirty();
				e.consume(this);
				return;
			}
		}
		if (svRect().contains(e.pos)) dragTarget = DragSv;
		else if (hueRect().contains(e.pos)) dragTarget = DragHue;
		else if (textureRect().contains(e.pos)) dragTarget = DragTexture;
		if (dragTarget != DragNone) {
			lastGlobalPublishAt = NAN;
			pickerTextureValid = false;
			applyDrag(e.pos);
			e.consume(this);
			return;
		}

		std::size_t presetCount = 0u;
		const FactoryPreset* presets = leviathan::theme::factoryPresets(&presetCount);
		for (std::size_t i = 0u; i < presetCount && i < 4u; ++i) {
			if (presetRect(int(i)).contains(e.pos)) {
				leviathan::theme::persistence::applyFactoryPresetAndSave(presets[i].id);
				dirty();
				e.consume(this);
				return;
			}
		}
		TransparentWidget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (dragTarget == DragNone || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			TransparentWidget::onDragMove(e);
			return;
		}
		// step() samples the captured pointer once per UI frame. Rack can route
		// drag-move events through the framebuffer inconsistently, and there may
		// be several such events in one frame.
		e.consume(this);
	}

	void onDragEnd(const event::DragEnd& e) override {
		if (dragTarget == DragNone || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			TransparentWidget::onDragEnd(e);
			return;
		}
		commitDrag();
		e.consume(this);
	}

	void onHover(const event::Hover& e) override {
		const bool hovered = textureRect().contains(e.pos);
		if (textureHovered != hovered) {
			textureHovered = hovered;
			dirty();
		}
		TransparentWidget::onHover(e);
	}

	void onLeave(const event::Leave& e) override {
		if (textureHovered) {
			textureHovered = false;
			dirty();
		}
		TransparentWidget::onLeave(e);
	}

	void step() override {
		if (dragTarget != DragNone) {
			const bool pressed = APP && APP->window && APP->window->win
				&& glfwGetMouseButton(APP->window->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
			if (pressed)
				applyDrag(currentLocalMousePos());
			else
				commitDrag();
		}
		const std::uint64_t current = leviathan::theme::generation();
		if (current != observedGeneration) {
			observedGeneration = current;
			dirty();
		}
		TransparentWidget::step();
	}

	void text(const DrawArgs& args, float x, float y, float size, const char* value,
		NVGcolor color, int align = NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE) const {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, size);
		nvgTextAlign(args.vg, align);
		nvgFillColor(args.vg, color);
		nvgText(args.vg, x, y, value, nullptr);
	}

	void button(const DrawArgs& args, math::Rect rect, const char* label,
		bool active = false, NVGcolor labelColor = nvgRGBA(238, 242, 248, 255)) const {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, 4.f);
		nvgFillColor(args.vg, active ? nvgRGBA(69, 54, 116, 245) : nvgRGBA(20, 25, 34, 245));
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, active ? 1.4f : 0.8f);
		nvgStrokeColor(args.vg, active ? nvgRGBA(220, 200, 255, 230) : nvgRGBA(96, 108, 125, 180));
		nvgStroke(args.vg);
		text(args, rect.pos.x + rect.size.x * 0.5f, rect.pos.y + rect.size.y * 0.5f,
			active ? 10.5f : 9.5f, label, labelColor);
	}

	void themedRoleButton(const DrawArgs& args, math::Rect rect,
	                     const char* label, bool active,
	                     ThemeColor previewColor,
	                     NVGcolor labelColor = nvgRGBA(255, 255, 255, 255)) const {
		// The live semantic glass and fractal texture are rendered underneath this
		// editor. This light local wash lets INPUT/OUTPUT follow the picker every
		// frame without publishing a library-wide theme update; its translucency
		// preserves the fractal texture and glass treatment underneath.
		const NVGpaint previewPaint = nvgLinearGradient(
			args.vg,
			rect.pos.x, rect.pos.y,
			rect.pos.x + rect.size.x, rect.pos.y + rect.size.y,
			nvgThemeColor(previewColor, 104),
			nvgThemeColor(previewColor, 68));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg,
			rect.pos.x + 0.8f, rect.pos.y + 0.8f,
			rect.size.x - 1.6f, rect.size.y - 1.6f, 3.4f);
		nvgFillPaint(args.vg, previewPaint);
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, 4.f);
		nvgStrokeWidth(args.vg, active ? 1.7f : 0.75f);
		nvgStrokeColor(args.vg, active ? nvgRGBA(244, 235, 255, 245)
		                                  : nvgRGBA(154, 170, 188, 150));
		nvgStroke(args.vg);
		const float centerX = rect.pos.x + rect.size.x * 0.5f;
		const float centerY = rect.pos.y + rect.size.y * 0.5f;
		// Match WYRM's dynamic TRIG/V/OCT label rather than static outlined
		// panel typography. Selection is expressed by the card, not the text.
		const float labelHeightPx = mm2px(3.8f);
		text(args, centerX, centerY, std::max(9.5f, labelHeightPx * 0.72f),
			label, labelColor);
	}

	void draw(const DrawArgs& args) override {
		const leviathan::theme::ThemeState state = leviathan::theme::read();
		const ThemeSnapshot& snapshot = state.snapshot;
		const bool draggingColor = dragTarget == DragSv || dragTarget == DragHue;
		if (!draggingColor)
			syncPicker(snapshot);
		const ThemeColor selected = draggingColor && pickerValid && pickerRole == selectedRole
			? pickerColor : colorForRole(snapshot, selectedRole);
		const float hue = pickerHue;
		const float saturation = pickerSaturation;
		const float value = pickerValue;

		text(args, box.size.x * 0.5f, 17.f, 18.f, "THEME", nvgRGBA(240, 235, 255, 255));
		text(args, box.size.x * 0.5f, 31.f, 8.5f, "GLOBAL CHROMA FIELD", nvgRGBA(129, 148, 166, 255));

		const char* roleNames[] = {"INPUT", "OUTPUT", "TEXT"};
		const ThemeRole roles[] = {ThemeRole::Input, ThemeRole::Output, ThemeRole::Text};
		for (int i = 0; i < 3; ++i) {
			if (roles[i] == ThemeRole::Input || roles[i] == ThemeRole::Output) {
				ThemeColor previewColor = colorForRole(snapshot, roles[i]);
				if (draggingColor && selectedRole == roles[i]) {
					previewColor = pickerColor;
				}
				themedRoleButton(
					args, roleRect(i), roleNames[i], selectedRole == roles[i],
					previewColor);
			}
			else {
				ThemeColor backgroundColor = colorForRole(
					snapshot, textPreviewBackgroundRole);
				if (draggingColor && selectedRole == textPreviewBackgroundRole)
					backgroundColor = pickerColor;
				ThemeColor textColor = colorForRole(snapshot, ThemeRole::Text);
				if (draggingColor && selectedRole == ThemeRole::Text)
					textColor = pickerColor;
				themedRoleButton(
					args, roleRect(i), roleNames[i], selectedRole == roles[i],
					backgroundColor, nvgThemeColor(textColor));
			}
		}

		const math::Rect sv = svRect();
		nvgBeginPath(args.vg);
		nvgRect(args.vg, sv.pos.x, sv.pos.y, sv.size.x, sv.size.y);
		nvgFillColor(args.vg, nvgThemeColor(hsvToRgb(hue, 1.f, 1.f)));
		nvgFill(args.vg);
		NVGpaint white = nvgLinearGradient(args.vg, sv.pos.x, 0.f, sv.pos.x + sv.size.x, 0.f,
			nvgRGBA(255, 255, 255, 255), nvgRGBA(255, 255, 255, 0));
		nvgFillPaint(args.vg, white);
		nvgFill(args.vg);
		NVGpaint black = nvgLinearGradient(args.vg, 0.f, sv.pos.y, 0.f, sv.pos.y + sv.size.y,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
		nvgFillPaint(args.vg, black);
		nvgFill(args.vg);

		const math::Rect hr = hueRect();
		for (int i = 0; i < 6; ++i) {
			const float y0 = hr.pos.y + hr.size.y * i / 6.f;
			const float y1 = hr.pos.y + hr.size.y * (i + 1) / 6.f;
			NVGpaint band = nvgLinearGradient(args.vg, 0.f, y0, 0.f, y1,
				nvgThemeColor(hsvToRgb(i / 6.f, 1.f, 1.f)),
				nvgThemeColor(hsvToRgb((i + 1) / 6.f, 1.f, 1.f)));
			nvgBeginPath(args.vg);
			nvgRect(args.vg, hr.pos.x, y0, hr.size.x, y1 - y0 + 0.5f);
			nvgFillPaint(args.vg, band);
			nvgFill(args.vg);
		}

		const float cursorX = sv.pos.x + saturation * sv.size.x;
		const float cursorY = sv.pos.y + (1.f - value) * sv.size.y;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cursorX, cursorY, 5.f);
		nvgStrokeWidth(args.vg, 2.f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 245));
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, hr.pos.x - 2.f, hr.pos.y + hue * hr.size.y - 1.5f, hr.size.x + 4.f, 3.f);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 245));
		nvgFill(args.vg);

		char hex[16];
		std::snprintf(hex, sizeof(hex), "#%02X%02X%02X", selected.r, selected.g, selected.b);
		text(args, 9.f, 220.f, 11.f, hex, nvgRGBA(235, 240, 247, 255), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		text(args, 171.f, 220.f, 9.f, "SELECTED ROLE", nvgRGBA(116, 132, 150, 255), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

		const math::Rect texture = textureRect();
		const float textureAmount = dragTarget == DragTexture && pickerTextureValid
			? pickerTextureAmount : snapshot.surface.textureAmount;
		char textureLabel[32];
		std::snprintf(textureLabel, sizeof(textureLabel), "Texture: %d%%",
			int(std::round(textureAmount * 100.f)));
		nvgSave(args.vg);
		nvgTranslate(args.vg, texture.pos.x, texture.pos.y);
		visual_assets::drawNeonBarSlider(
			args, Vec(texture.size.x, 28.f), textureAmount * 0.5f,
			textureHovered || dragTarget == DragTexture, textureLabel);
		nvgRestore(args.vg);

		text(args, 9.f, 268.f, 8.5f, "FACTORY FIELDS", nvgRGBA(150, 165, 181, 255), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		std::size_t presetCount = 0u;
		const FactoryPreset* presets = leviathan::theme::factoryPresets(&presetCount);
		for (std::size_t i = 0u; i < presetCount && i < 4u; ++i)
			button(args, presetRect(int(i)), presets[i].name, state.activePreset == presets[i].id);

	}
};

struct ThemeModuleWidget final : ModuleWidget {
	ThemeModuleWidget(ThemeModule* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/Theme.svg");
		setPanel(createPanel(panelPath));
		Widget* panelSurface = visual_assets::createPanelSurfaceEffectWidget(
			panelPath, box.size, -1.f, this);
		addChild(panelSurface);
		auto* themePreviewOverlay = visual_assets::addFractalGlassOverlay(
			this, panelPath, panelSurface);

		auto* framebuffer = new widget::FramebufferWidget;
		framebuffer->box.size = box.size;
		auto* editor = new ThemeEditor;
		editor->box.size = box.size;
		editor->framebuffer = framebuffer;
		editor->themePreviewOverlay = themePreviewOverlay;
		framebuffer->addChild(editor);
		addChild(framebuffer);

		visual_assets::addCompactLeviathanLogoBranding(this, panelPath);
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<CyanOrbScrew>(
			Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<CyanOrbScrew>(
			Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(
			Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}
};

} // namespace

Model* modelTheme = createModel<ThemeModule, ThemeModuleWidget>("Theme");
